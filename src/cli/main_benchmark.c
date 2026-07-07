#include "core/config.h"
#include "sim/benchmark.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(void) {
    printf("Usage: frogger_benchmark [options]\n");
    printf("Options:\n");
    printf("  --episodes <n>    Number of episodes (default 1000)\n");
    printf("  --seed <n>        Random seed (default 1)\n");
    printf("  -h, --help        Show this help text\n");
}

int main(int argc, char** argv) {
    int episodes = 1000;
    uint64_t seed = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--episodes") == 0 && i + 1 < argc) {
            episodes = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (uint64_t)strtoull(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }

    if (episodes <= 0) {
        fprintf(stderr, "--episodes must be positive\n");
        return 1;
    }

    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = (int)seed;

    benchmark_run(&cfg, episodes, seed);
    return 0;
}
