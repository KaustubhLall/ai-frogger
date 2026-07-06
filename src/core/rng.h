#ifndef FROGGER_RNG_H
#define FROGGER_RNG_H

#include <stdint.h>

typedef struct {
    uint64_t state;
} RNG;

void rng_init(RNG* rng, uint64_t seed);
uint64_t rng_next_u64(RNG* rng);
uint32_t rng_next_u32(RNG* rng);
int rng_range(RNG* rng, int lo, int hi);
float rng_float(RNG* rng);
float rng_float_range(RNG* rng, float lo, float hi);

#endif /* FROGGER_RNG_H */
