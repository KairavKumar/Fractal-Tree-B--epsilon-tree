#pragma once
#include "be_tree_node.hpp"
#include <memory>

class BeTree {
public:
    BeTree();
    ~BeTree();

    void Insert(uint32_t key, uint32_t val);
    void Update(uint32_t key, uint32_t val);
    void Delete(uint32_t key);
    uint32_t Query(uint32_t key);

private:
    std::unique_ptr<BeNode> root;
    void FullFlush();
    void CreateNewRoot(uint32_t split_key, BeNode* new_node);
};
