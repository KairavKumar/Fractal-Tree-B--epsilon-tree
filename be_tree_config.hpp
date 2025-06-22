#pragma once
#include <cstdint>

constexpr int NUM_DATA_PAIRS = 16;
constexpr int NUM_PIVOTS = 8;
constexpr int NUM_UPSERTS = 32;
constexpr int FLUSH_THRESHOLD = 8;
constexpr int LEAF_FLUSH_THRESHOLD = 8;
constexpr uint32_t KEY_NOT_FOUND = static_cast<uint32_t>(-1);

enum UpsertFunction { INSERT, DELETE, UPDATE, INVALID };
enum FlushResult { NO_SPLIT, SPLIT, ENSURE_SPACE };

struct BeUpsert {
    uint32_t key;
    UpsertFunction type;
    uint32_t parameter;
    uint32_t timestamp;
};
