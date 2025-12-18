#include <iostream>
#include "cache.hpp"

Cache::Cache(int32_t* memory)
    : memory_(memory) {}

int Cache::load(uint32_t addr, int32_t& out) {
    uint32_t index = addr % NUM_LINES;
    uint32_t tag   = addr / NUM_LINES;

    CacheLine& line = lines_[index];

    if (line.valid && line.tag == tag) {
        printf("Cache hit at address %u\n", addr);
        hits_++;
        out = line.data;
        return HIT_LATENCY;
    }
    printf("Cache miss at address %u\n", addr);
    // MISS → eviction happens here
    misses_++;
    out = memory_[addr];

    line.valid = true;
    line.tag = tag;
    line.data = out;

    return MISS_LATENCY;
}

int Cache::store(uint32_t addr, int32_t val) {
    uint32_t index = addr % NUM_LINES;
    uint32_t tag   = addr / NUM_LINES;

    CacheLine& line = lines_[index];

    memory_[addr] = val;  // always write to memory

    if (line.valid && line.tag == tag) {
        printf("Cache hit on store at address %u\n", addr);
        hits_++;
        line.data = val;
        return HIT_LATENCY;
    }
    printf("Cache miss on store at address %u\n", addr);
    misses_++;
    line.valid = true;
    line.tag = tag;
    line.data = val;

    return MISS_LATENCY;
}
