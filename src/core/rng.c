#include "core/rng.h"

static uint64_t splitmix64(uint64_t* state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

void rng_init(RNG* rng, uint64_t seed) {
    rng->state = seed ? seed : 0xDEADBEEFCAFEBABEULL;
    /* warm up */
    for (int i = 0; i < 4; i++) splitmix64(&rng->state);
}

uint64_t rng_next_u64(RNG* rng) {
    return splitmix64(&rng->state);
}

uint32_t rng_next_u32(RNG* rng) {
    return (uint32_t)(rng_next_u64(rng) >> 32);
}

int rng_range(RNG* rng, int lo, int hi) {
    if (hi <= lo) return lo;
    uint32_t range = (uint32_t)(hi - lo);
    return lo + (int)(rng_next_u32(rng) % range);
}

float rng_float(RNG* rng) {
    return (float)(rng_next_u64(rng) >> 40) / (float)(1ULL << 24);
}

float rng_float_range(RNG* rng, float lo, float hi) {
    return lo + rng_float(rng) * (hi - lo);
}
