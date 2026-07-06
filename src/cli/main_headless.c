#include "env/env.h"
#include "agents/agent.h"
#include "sim/runner.h"
#include "sim/evaluator.h"
#include "core/metrics.h"
#include "core/replay.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    AgentType agent_type = AGENT_HEURISTIC;
    int episodes = 100;
    uint64_t seed = 1337;
    int compare = 0;
    const char* replay_path = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--agent") == 0 && i + 1 < argc) {
            agent_type = agent_parse_type(argv[++i]);
        } else if (strcmp(argv[i], "--episodes") == 0 && i + 1 < argc) {
            episodes = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (uint64_t)strtoull(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--compare") == 0) {
            compare = 1;
        } else if (strcmp(argv[i], "--replay") == 0 && i + 1 < argc) {
            replay_path = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: frogger_headless [options]\n");
            printf("Options:\n");
            printf("  --agent <type>    Agent type: random, scripted, heuristic, greedy\n");
            printf("  --episodes <n>    Number of episodes (default 100)\n");
            printf("  --seed <n>        Random seed (default 1337)\n");
            printf("  --compare         Compare all agents\n");
            printf("  --replay <file>   Save replay to file\n");
            return 0;
        }
    }

    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = (int)seed;

    if (compare) {
        evaluator_compare(&cfg, episodes, seed);
        return 0;
    }

    printf("\n=== Headless Run ===\n");
    const char* type_names[] = {"random", "scripted", "heuristic", "greedy"};
    printf("Agent: %s | Episodes: %d | Seed: %llu\n\n",
           type_names[agent_type], episodes, (unsigned long long)seed);

    Metrics metrics;
    metrics_init(&metrics);

    RunConfig rc;
    rc.config = cfg;
    rc.agent_type = agent_type;
    rc.seed = seed;
    rc.episodes = episodes;
    rc.record_replay = (replay_path != NULL);

    runner_run(&rc, &metrics);
    metrics_print(&metrics);

    if (replay_path) {
        Replay replay;
        replay_init(&replay, &cfg, seed);
        /* Re-run one episode to record */
        FroggerEnv env;
        env_init(&env, &cfg);
        env_reset(&env, seed);
        Agent agent;
        agent_init(&agent, agent_type);
        agent_reset(&agent, seed);
        Observation obs;
        DebugSnapshot snap;
        for (int step = 0; step < cfg.max_steps; step++) {
            env_observe(&env, &obs);
            env_get_debug_snapshot(&env, &snap);
            Action a = agent_act(&agent, &obs, &snap);
            replay_record(&replay, a);
            StepResult r = env_step(&env, a);
            if (r.done) break;
        }
        replay_save(&replay, replay_path);
        printf("\nReplay saved to %s (%d actions)\n", replay_path, replay.action_count);
    }

    return 0;
}
