#include "env/env.h"
#include "agents/agent.h"
#include "agents/neuro_agent.h"
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
    const char* weights_path = NULL;
    const char* csv_path = NULL;
    const char* json_path = NULL;
    const char* config_path = NULL;
    NeuroWeights neuro_weights;
    int has_neuro_weights = 0;

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
        } else if (strcmp(argv[i], "--weights") == 0 && i + 1 < argc) {
            weights_path = argv[++i];
        } else if (strcmp(argv[i], "--export-csv") == 0 && i + 1 < argc) {
            csv_path = argv[++i];
        } else if (strcmp(argv[i], "--export-json") == 0 && i + 1 < argc) {
            json_path = argv[++i];
        } else if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
            config_path = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: frogger_headless [options]\n");
            printf("Options:\n");
            printf("  --agent <type>    Agent type: random, scripted, heuristic, greedy, neuro\n");
            printf("  --episodes <n>    Number of episodes (default 100)\n");
            printf("  --seed <n>        Random seed (default 1337)\n");
            printf("  --compare         Compare all agents\n");
            printf("  --replay <file>   Save replay to file\n");
            printf("  --weights <file>  Load trained neuro weights (requires --agent neuro)\n");
            printf("  --export-csv <f>  Export per-episode metrics to CSV\n");
            printf("  --export-json <f> Export summary metrics to JSON\n");
            printf("  --config <file>   Load JSON config file (overrides defaults)\n");
            return 0;
        }
    }

    FroggerConfig cfg;
    config_crossing(&cfg);
    if (config_path) {
        if (!config_load_json(&cfg, config_path)) {
            fprintf(stderr, "Failed to load config from %s, using defaults\n", config_path);
        }
    }
    cfg.seed = (int)seed;

    if (weights_path) {
        if (neuro_weights_load(&neuro_weights, weights_path)) {
            has_neuro_weights = 1;
        } else {
            fprintf(stderr, "Failed to load weights from %s\n", weights_path);
        }
    }

    if (compare) {
        evaluator_compare(&cfg, episodes, seed);
        return 0;
    }

    printf("\n=== Headless Run ===\n");
    const char* type_names[] = {"random", "scripted", "heuristic", "greedy", "neuro"};
    printf("Agent: %s | Episodes: %d | Seed: %llu\n\n",
           type_names[agent_type], episodes, (unsigned long long)seed);

    Metrics* metrics = (Metrics*)malloc(sizeof(Metrics));
    metrics_init(metrics);

    if (agent_type == AGENT_NEURO && has_neuro_weights) {
        Agent agent;
        agent_init(&agent, AGENT_NEURO);
        agent.impl = &neuro_weights;

        for (int ep = 0; ep < episodes; ep++) {
            FroggerEnv env;
            env_init(&env, &cfg);
            env_reset(&env, seed + (uint64_t)ep);
            agent_reset(&agent, seed + (uint64_t)ep);
            runner_run_single(&env, &agent, cfg.max_steps, metrics);
        }
    } else {
        RunConfig rc;
        rc.config = cfg;
        rc.agent_type = agent_type;
        rc.seed = seed;
        rc.episodes = episodes;
        rc.record_replay = (replay_path != NULL);

        runner_run(&rc, metrics);
    }
    metrics_print(metrics);
    metrics_print_terminal_breakdown(metrics);

    if (csv_path) {
        if (metrics_export_csv(metrics, csv_path)) {
            printf("\nCSV exported to %s\n", csv_path);
        } else {
            fprintf(stderr, "Failed to export CSV to %s\n", csv_path);
        }
    }
    if (json_path) {
        if (metrics_export_json(metrics, json_path)) {
            printf("JSON exported to %s\n", json_path);
        } else {
            fprintf(stderr, "Failed to export JSON to %s\n", json_path);
        }
    }
    free(metrics);

    if (replay_path) {
        Replay replay;
        replay_init(&replay, &cfg, seed);
        FroggerEnv env;
        env_init(&env, &cfg);
        env_reset(&env, seed);
        Agent agent;
        agent_init(&agent, agent_type);
        if (agent_type == AGENT_NEURO && has_neuro_weights) {
            agent.impl = &neuro_weights;
        }
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
