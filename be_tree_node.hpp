#pragma once
#include "be_tree_config.hpp"
#include <vector>
#include <cstdint>

struct BeNode {
    bool is_leaf;
    BeNode* parent;
    std::vector<uint32_t> keys;      // For leaves: keys; for internals: pivots
    std::vector<uint32_t> values;    // Only for leaves
    std::vector<BeNode*> children;   // Only for internals
    std::vector<BeUpsert> buffer;    // Upsert buffer
    uint32_t flush_size = 0;

    BeNode(bool leaf, BeNode* parent);

    int IndexOfKey(uint32_t key) const;
    bool UpsertLeaf(BeUpsert upserts[], int& num);
    uint32_t SplitLeaf(BeNode*& new_sibling);
    uint32_t SplitInternal(BeNode*& new_sibling);
    void FullFlushSetup();
    FlushResult FlushOneLevel(uint32_t& split_key, BeNode*& new_node);
    FlushResult FlushOneLeaf(BeNode* child, uint32_t& split_key, BeNode*& new_node);
    FlushResult FlushOneInternal(BeNode* child);
    bool AddPivot(uint32_t split_key, BeNode* new_node);
    uint32_t Query(uint32_t key);
    void Upsert(uint32_t key, UpsertFunction type, uint32_t val);
};
