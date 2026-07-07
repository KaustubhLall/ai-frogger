#include "core/config.h"
#include "core/rng.h"
#include "agents/agent.h"
#include "agents/neuro_agent.h"
#include "train/genetic.h"
#include "sim/runner.h"
#include "sim/evaluator.h"
#include "core/metrics.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int population = 20;
    int generations = 50;
    int episodes = 10;
    int elite = 4;
    float mutation_rate = 0.05f;
    uint64_t seed = 1337;
    const char* save_path = NULL;
    const char* load_path = NULL;
    const char* config_path = NULL;
    int eval_only = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--population") == 0 && i + 1 < argc) {
            population = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--generations") == 0 && i + 1 < argc) {
            generations = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--episodes") == 0 && i + 1 < argc) {
            episodes = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--elite") == 0 && i + 1 < argc) {
            elite = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--mutation") == 0 && i + 1 < argc) {
            mutation_rate = (float)atof(argv[++i]);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (uint64_t)strtoull(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--save") == 0 && i + 1 < argc) {
            save_path = argv[++i];
        } else if (strcmp(argv[i], "--load") == 0 && i + 1 < argc) {
            load_path = argv[++i];
        } else if (strcmp(argv[i], "--eval") == 0 && i + 1 < argc) {
            eval_only = 1;
            episodes = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
            config_path = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: frogger_train [options]\n");
            printf("Options:\n");
            printf("  --population <n>  Population size (default 20, max 64)\n");
            printf("  --generations <n> Generations to train (default 50)\n");
            printf("  --episodes <n>    Episodes per evaluation (default 10)\n");
            printf("  --elite <n>       Elite individuals kept (default 4)\n");
            printf("  --mutation <f>    Mutation rate 0-1 (default 0.05)\n");
            printf("  --seed <n>        Random seed (default 1337)\n");
            printf("  --save <file>     Save best weights to file\n");
            printf("  --load <file>     Load weights from file (for eval)\n");
            printf("  --eval <n>        Evaluate loaded weights over n episodes\n");
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

    if (episodes <= 0) {
        fprintf(stderr, "--episodes must be positive\n");
        return 1;
    }
    if (population <= 0 || population > GA_MAX_POPULATION) {
        fprintf(stderr, "--population must be between 1 and %d\n", GA_MAX_POPULATION);
        return 1;
    }
    if (generations <= 0) {
        fprintf(stderr, "--generations must be positive\n");
        return 1;
    }
    if (elite < 0 || elite > population) {
        fprintf(stderr, "--elite must be between 0 and population size\n");
        return 1;
    }
    if (mutation_rate < 0.0f || mutation_rate > 1.0f) {
        fprintf(stderr, "--mutation must be between 0.0 and 1.0\n");
        return 1;
    }

    if (eval_only && load_path) {
        NeuroWeights weights;
        if (!neuro_weights_load(&weights, load_path)) {
            fprintf(stderr, "Failed to load weights from %s\n", load_path);
            return 1;
        }

        printf("\n=== Evaluating Trained Agent ===\n");
        printf("Weights: %s | Episodes: %d | Seed: %llu\n\n",
               load_path, episodes, (unsigned long long)seed);

        Agent agent;
        agent_init(&agent, AGENT_NEURO);
        agent.impl = &weights;

        Metrics* metrics = (Metrics*)malloc(sizeof(Metrics));
        metrics_init(metrics);

        for (int ep = 0; ep < episodes; ep++) {
            FroggerEnv env;
            env_init(&env, &cfg);
            env_reset(&env, seed + (uint64_t)ep);
            agent_reset(&agent, seed + (uint64_t)ep);
            runner_run_single(&env, &agent, cfg.max_steps, metrics);
        }

        metrics_print(metrics);
        free(metrics);

        printf("\n=== Comparison with Baselines ===\n");
        evaluator_compare(&cfg, episodes, seed);

        AgentType neuro_type = AGENT_NEURO;
        (void)neuro_type;
        return 0;
    }

    printf("\n=== Genetic Algorithm Training ===\n");
    printf("Population: %d | Generations: %d | Episodes/eval: %d | Elite: %d | Mutation: %.3f\n",
           population, generations, episodes, elite, mutation_rate);
    printf("Seed: %llu\n\n", (unsigned long long)seed);

    GATrainer* trainer = (GATrainer*)malloc(sizeof(GATrainer));
    if (!trainer) {
        fprintf(stderr, "Failed to allocate GATrainer\n");
        return 1;
    }
    ga_init(trainer, population, elite, mutation_rate, episodes, seed);

    ga_train(trainer, &cfg, generations);

    printf("\n=== Training Complete ===\n");
    printf("Best fitness: %.3f (generation %d)\n", trainer->best_fitness, trainer->best_generation);

    if (save_path) {
        if (neuro_weights_save(&trainer->best_weights, save_path)) {
            printf("Best weights saved to %s\n", save_path);
        } else {
            fprintf(stderr, "Failed to save weights to %s\n", save_path);
        }
    }

    printf("\n=== Evaluating Best Agent ===\n");
    Agent agent;
    agent_init(&agent, AGENT_NEURO);
    agent.impl = &trainer->best_weights;

    Metrics* metrics = (Metrics*)malloc(sizeof(Metrics));
    metrics_init(metrics);

    for (int ep = 0; ep < 100; ep++) {
        FroggerEnv env;
        env_init(&env, &cfg);
        env_reset(&env, seed + (uint64_t)ep);
        agent_reset(&agent, seed + (uint64_t)ep);
        runner_run_single(&env, &agent, cfg.max_steps, metrics);
    }

    printf("Trained agent over 100 episodes:\n");
    metrics_print(metrics);
    free(metrics);

    printf("\n=== Comparison with Baselines ===\n");
    evaluator_compare(&cfg, 100, seed);

    free(trainer);
    return 0;
}
