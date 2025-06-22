#include "be_tree.hpp"

void BeTree::Delete(uint32_t key) {
    if (root->buffer.size() == NUM_UPSERTS) FullFlush();
    root->Upsert(key, DELETE, 0);
}
