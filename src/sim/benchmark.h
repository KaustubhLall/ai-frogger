#ifndef FROGGER_BENCHMARK_H
#define FROGGER_BENCHMARK_H

#include "core/config.h"
#include "core/metrics.h"

void benchmark_run(const FroggerConfig* cfg, int episodes, uint64_t seed);

#endif /* FROGGER_BENCHMARK_H */
