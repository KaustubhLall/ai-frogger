#include "core/config.h"
#include "sim/benchmark.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int episodes = 1000;
    uint64_t seed = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--episodes") == 0 && i + 1 < argc) {
            episodes = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (uint64_t)strtoull(argv[++i], NULL, 10);
        }
    }

    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = (int)seed;

    benchmark_run(&cfg, episodes, seed);
    return 0;
}
