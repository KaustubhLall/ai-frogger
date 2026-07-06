#include "sim/evaluator.h"
#include "sim/runner.h"
#include "core/metrics.h"
#include <stdio.h>

EvalResult evaluator_run(const FroggerConfig* cfg, AgentType type, int episodes, uint64_t seed) {
    Metrics metrics;
    metrics_init(&metrics);

    RunConfig rc;
    rc.config = *cfg;
    rc.agent_type = type;
    rc.seed = seed;
    rc.episodes = episodes;
    rc.record_replay = 0;

    runner_run(&rc, &metrics);

    EvalResult r;
    r.avg_reward = metrics.episodes > 0 ? metrics.total_reward / metrics.episodes : 0.0f;
    r.win_rate = metrics.episodes > 0 ? (float)metrics.wins / metrics.episodes : 0.0f;
    r.death_rate = metrics.episodes > 0 ? (float)metrics.deaths / metrics.episodes : 0.0f;
    r.avg_length = metrics.episodes > 0 ? (float)metrics.total_steps / metrics.episodes : 0.0f;
    r.episodes = metrics.episodes;
    return r;
}

void evaluator_compare(const FroggerConfig* cfg, int episodes, uint64_t seed) {
    AgentType types[] = {AGENT_RANDOM, AGENT_SCRIPTED, AGENT_HEURISTIC, AGENT_GREEDY};
    const char* names[] = {"random", "scripted", "heuristic", "greedy"};
    int n = (int)(sizeof(types) / sizeof(types[0]));

    printf("=== Agent Comparison ===\n");
    printf("%-15s %10s %10s %10s %10s\n", "Agent", "AvgReward", "WinRate", "DeathRate", "AvgLen");
    for (int i = 0; i < n; i++) {
        EvalResult r = evaluator_run(cfg, types[i], episodes, seed);
        printf("%-15s %10.4f %10.2f%% %10.2f%% %10.1f\n",
               names[i], r.avg_reward, r.win_rate * 100.0f, r.death_rate * 100.0f, r.avg_length);
    }
}
