#pragma once
#include <vector>
#include <cstdint>

class Cache {
public:
    Cache(int32_t* memory);

    int load(uint32_t addr, int32_t& out);
    int store(uint32_t addr, int32_t val);

    int hits() const { return hits_; }
    int misses() const { return misses_; }

private:
    static constexpr int NUM_LINES = 64;
    static constexpr int HIT_LATENCY = 1;
    static constexpr int MISS_LATENCY = 50;

    struct CacheLine {
        bool valid = false;
        uint32_t tag = 0;
        int32_t data = 0;
    };

    CacheLine lines_[NUM_LINES];
    int32_t* memory_;   // backing memory

    int hits_ = 0;
    int misses_ = 0;
};
