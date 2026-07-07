#include "sim/benchmark.h"
#include "sim/runner.h"
#include "agents/agent.h"
#include <stdio.h>
#include <stdlib.h>

void benchmark_run(const FroggerConfig* cfg, int episodes, uint64_t seed) {
    Metrics* metrics = (Metrics*)malloc(sizeof(Metrics));
    metrics_init(metrics);

    RunConfig rc;
    rc.config = *cfg;
    rc.agent_type = AGENT_HEURISTIC;
    rc.seed = seed;
    rc.episodes = episodes;
    rc.record_replay = 0;

    runner_run(&rc, metrics);
    printf("=== Benchmark Results ===\n");
    metrics_print(metrics);
    free(metrics);
}
