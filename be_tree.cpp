#include "be_tree.hpp"
#include <cassert>


BeTree::BeTree() {
    root = std::make_unique<BeNode>(false, nullptr);
    auto* leaf1 = new BeNode(true, root.get());
    auto* leaf2 = new BeNode(true, root.get());
    root->keys.push_back(500000000);
    root->children.push_back(leaf1);
    root->children.push_back(leaf2);
}

BeTree::~BeTree() = default;

void BeTree::CreateNewRoot(uint32_t split_key, BeNode* new_node) {
    auto* old_root = root.release();
    root = std::make_unique<BeNode>(false, nullptr);
    root->keys.push_back(split_key);
    root->children.push_back(old_root);
    root->children.push_back(new_node);
    old_root->parent = root.get();
    new_node->parent = root.get();
}


void BeTree::FlushUntilSuccess(BeNode*& node, FlushResult& flush_res, uint32_t& split_key, BeNode*& new_node) {
    while (true) {
        node->FullFlushSetup();
        flush_res = node->FlushOneLevel(split_key, new_node);

        if (flush_res != ENSURE_SPACE) return;

        int child_idx = node->IndexOfKey(node->buffer.back().key);
        node = node->children[child_idx];
    }
}



void BeTree::FullFlush() {
    BeNode* node = root.get();
    FlushResult flush_res;
    uint32_t split_key;
    BeNode* new_node = nullptr;

    FlushUntilSuccess(node, flush_res, split_key, new_node);

    while (true) {
        if (flush_res == SPLIT) {
            if (node->AddPivot(split_key, new_node)) {
                split_key = node->SplitInternal(new_node);
                if (node->flush_size == 0) node = new_node;  
            } else {
                flush_res = NO_SPLIT;
            }
        }

        if (node->flush_size > 0) {
            flush_res = node->FlushOneLevel(split_key, new_node);
            assert(flush_res == NO_SPLIT || flush_res == SPLIT);
            continue;  
        }

        if (!node->parent) {
            if (flush_res == SPLIT) CreateNewRoot(split_key, new_node);
            break;
        }

        node = node->parent;
    }
}

