#include "be_tree_node.hpp"
#include <algorithm>
#include <cassert>

static uint32_t all_timestamp = 0;

BeNode::BeNode(bool leaf, BeNode* parent)
    : is_leaf(leaf), parent(parent), flush_size(0) {
    if (is_leaf) {
        keys.reserve(NUM_DATA_PAIRS);
        values.reserve(NUM_DATA_PAIRS);
    } else {
        keys.reserve(NUM_PIVOTS);
        children.reserve(NUM_PIVOTS + 1);
    }
    buffer.reserve(NUM_UPSERTS);
}

int BeNode::IndexOfKey(uint32_t key) const {
    auto it = std::upper_bound(keys.begin(), keys.end(), key);
    return static_cast<int>(it - keys.begin());
}

bool BeNode::UpsertLeaf(BeUpsert upserts[], int& num) {
    assert(is_leaf);
    while (num > 0) {
        num--;
        int index = -1;
        for (int j = 0; j < static_cast<int>(keys.size()); ++j) {
            if (keys[j] == upserts[num].key) {
                index = j;
                break;
            }
        }
        switch (upserts[num].type) {
            case INSERT:
                assert(index < 0);
                keys.push_back(upserts[num].key);
                values.push_back(upserts[num].parameter);
                if (keys.size() == NUM_DATA_PAIRS) return true;
                break;
            case UPDATE:
                assert(index >= 0);
                values[index] = upserts[num].parameter;
                break;
            case DELETE:
                assert(index >= 0);
                keys.erase(keys.begin() + index);
                values.erase(values.begin() + index);
                break;
            default:
                assert(false);
        }
    }
    return false;
}

uint32_t BeNode::SplitLeaf(BeNode*& new_sibling) {
    assert(is_leaf);
    std::vector<std::pair<uint32_t, uint32_t>> pairs(keys.size());
    for (size_t i = 0; i < keys.size(); ++i)
        pairs[i] = {keys[i], values[i]};
    std::sort(pairs.begin(), pairs.end());
    size_t mid = pairs.size() / 2;
    new_sibling = new BeNode(true, parent);
    for (size_t i = mid; i < pairs.size(); ++i) {
        new_sibling->keys.push_back(pairs[i].first);
        new_sibling->values.push_back(pairs[i].second);
    }
    keys.resize(mid);
    values.resize(mid);
    return new_sibling->keys[0];
}

uint32_t BeNode::SplitInternal(BeNode*& new_sibling) {
    assert(!is_leaf);
    size_t mid = keys.size() / 2;
    new_sibling = new BeNode(false, parent);
    for (size_t i = mid + 1; i < keys.size(); ++i)
        new_sibling->keys.push_back(keys[i]);
    for (size_t i = mid + 1; i < children.size(); ++i)
        new_sibling->children.push_back(children[i]);
    keys.resize(mid);
    children.resize(mid + 1);
    for (auto it = buffer.begin(); it != buffer.end();) {
        if (it->key >= new_sibling->keys[0]) {
            new_sibling->buffer.push_back(*it);
            it = buffer.erase(it);
        } else {
            ++it;
        }
    }
    return new_sibling->keys[0];
}

void BeNode::FullFlushSetup() {
    assert(flush_size == 0 && !is_leaf);
    std::vector<int> nums(children.size(), 0);
    for (const auto& ups : buffer)
        ++nums[IndexOfKey(ups.key)];
    int to_flush = std::distance(nums.begin(), std::max_element(nums.begin(), nums.end()));
    std::stable_partition(buffer.begin(), buffer.end(),
        [&](const BeUpsert& u) { return IndexOfKey(u.key) != to_flush; });
    flush_size = nums[to_flush];
    std::sort(buffer.end() - flush_size, buffer.end(),
        [](const BeUpsert& a, const BeUpsert& b) { return a.timestamp > b.timestamp; });
}

FlushResult BeNode::FlushOneLeaf(BeNode* child, uint32_t& split_key, BeNode*& new_node) {
    assert(!is_leaf && child->is_leaf);

    BeUpsert* to_flush = buffer.data() + (buffer.size() - flush_size);
    int num_to_flush = std::min(static_cast<int>(flush_size), LEAF_FLUSH_THRESHOLD);

    if (child->UpsertLeaf(to_flush, num_to_flush)) {

        split_key = child->SplitLeaf(new_node);
        std::vector<BeUpsert> left_msgs;
        std::vector<BeUpsert> right_msgs;

        for (int i = 0; i < num_to_flush; ++i) {
            if (to_flush[i].key < split_key)
                left_msgs.push_back(to_flush[i]);
            else
                right_msgs.push_back(to_flush[i]);
        }

        if (!left_msgs.empty()) {
            int lnum = left_msgs.size();
            child->UpsertLeaf(left_msgs.data(), lnum);
        }

        if (!right_msgs.empty()) {
            int rnum = right_msgs.size();
            new_node->UpsertLeaf(right_msgs.data(), rnum);
        }

        buffer.resize(buffer.size() - num_to_flush);
        flush_size = 0;
        return SPLIT;
    }

    buffer.resize(buffer.size() - num_to_flush);
    flush_size = 0;
    return NO_SPLIT;
}

FlushResult BeNode::FlushOneInternal(BeNode* child) {
    assert(!is_leaf && !child->is_leaf);
    int num_empty = NUM_UPSERTS - child->buffer.size();
    int flush_num = (num_empty >= flush_size) ? flush_size :
                    (num_empty >= FLUSH_THRESHOLD) ? FLUSH_THRESHOLD : 0;
    if (flush_num == 0) return ENSURE_SPACE;
    std::copy(buffer.end() - flush_num, buffer.end(), std::back_inserter(child->buffer));
    buffer.resize(buffer.size() - flush_num);
    flush_size = 0;
    return NO_SPLIT;
}

FlushResult BeNode::FlushOneLevel(uint32_t& split_key, BeNode*& new_node) {
    int child_idx = IndexOfKey(buffer[buffer.size() - flush_size].key);
    BeNode* child = children[child_idx];
    if (child->is_leaf)
        return FlushOneLeaf(child, split_key, new_node);
    else
        return FlushOneInternal(child);
}

bool BeNode::AddPivot(uint32_t split_key, BeNode* new_node) {
    assert(!is_leaf && new_node);
    int pos = IndexOfKey(split_key);
    keys.insert(keys.begin() + pos, split_key);
    children.insert(children.begin() + pos + 1, new_node);
    return keys.size() == NUM_PIVOTS;
}

uint32_t BeNode::Query(uint32_t key) {
    if (is_leaf) {
        for (size_t i = 0; i < keys.size(); ++i)
            if (keys[i] == key) return values[i];
        return KEY_NOT_FOUND;
    }
    uint32_t ret = KEY_NOT_FOUND;
    uint32_t latest_ts = 0;
    bool found = false;
    for (const auto& ups : buffer) {
        if (ups.key == key && ups.timestamp >= latest_ts) {
            latest_ts = ups.timestamp;
            if (ups.type == DELETE)
                ret = KEY_NOT_FOUND;
            else
                ret = ups.parameter;
            found = true;
        }
    }
    if (found) return ret;
    int idx = IndexOfKey(key);
    return children[idx]->Query(key);
}

void BeNode::Upsert(uint32_t key, UpsertFunction type, uint32_t val) {
    assert(buffer.size() < NUM_UPSERTS);
    buffer.push_back({key, type, val, ++all_timestamp});
}
