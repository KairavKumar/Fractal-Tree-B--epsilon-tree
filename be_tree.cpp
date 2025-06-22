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

void BeTree::FullFlush() {
    BeNode* node = root.get();
    FlushResult flush_res;
    uint32_t split_key;
    BeNode* new_node = nullptr;
    do {
        node->FullFlushSetup();
        flush_res = node->FlushOneLevel(split_key, new_node);
    } while (flush_res == ENSURE_SPACE);

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
            FlushResult debug_res = node->FlushOneLevel(split_key, new_node);
            assert(debug_res == NO_SPLIT);
        }
        if (!node->parent) {
            if (flush_res == SPLIT) CreateNewRoot(split_key, new_node);
            break;
        }
        node = node->parent;
    }
}
