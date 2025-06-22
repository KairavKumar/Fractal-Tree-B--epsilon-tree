#include "be_tree.hpp"

void BeTree::Update(uint32_t key, uint32_t val) {
    if (root->buffer.size() == NUM_UPSERTS) FullFlush();
    root->Upsert(key, UPDATE, val);
}
