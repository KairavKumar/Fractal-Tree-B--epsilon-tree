#include "be_tree.hpp"

uint32_t BeTree::Query(uint32_t key) {
    return root->Query(key);
}
