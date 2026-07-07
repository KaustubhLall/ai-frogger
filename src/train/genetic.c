#include "train/genetic.h"
#include "env/env.h"
#include "agents/agent.h"
#include <string.h>
#include <stdio.h>

void ga_init(GATrainer* t, int population_size, int elite_count, float mutation_rate,
             int episodes_per_eval, uint64_t seed) {
    memset(t, 0, sizeof(GATrainer));
    if (population_size > GA_MAX_POPULATION) population_size = GA_MAX_POPULATION;
    if (population_size < 2) population_size = 2;
    t->population_size = population_size;
    t->elite_count = elite_count;
    t->mutation_rate = mutation_rate;
    t->episodes_per_eval = episodes_per_eval;
    t->base_seed = seed;
    t->best_fitness = -1e30f;

    RNG rng;
    rng_init(&rng, seed);
    for (int i = 0; i < t->population_size; i++) {
        neuro_weights_init(&t->population[i].weights, &rng);
        t->population[i].fitness = 0.0f;
    }
}

float ga_evaluate_individual(GATrainer* t, int idx, const FroggerConfig* cfg) {
    GAIndividual* ind = &t->population[idx];
    Agent agent;
    agent_init(&agent, AGENT_NEURO);
    agent.impl = &ind->weights;

    float total_reward = 0.0f;
    int wins = 0, deaths = 0;

    for (int ep = 0; ep < t->episodes_per_eval; ep++) {
        FroggerEnv env;
        env_init(&env, cfg);
        env_reset(&env, t->base_seed + (uint64_t)ep * 7 + (uint64_t)idx * 13);
        agent_reset(&agent, t->base_seed + (uint64_t)ep * 7 + (uint64_t)idx * 13);

        Observation obs;
        DebugSnapshot snap;
        float cumulative = 0.0f;

        for (int step = 0; step < cfg->max_steps; step++) {
            env_observe(&env, &obs);
            env_get_debug_snapshot(&env, &snap);
            Action action = agent_act(&agent, &obs, &snap);
            StepResult result = env_step(&env, action);
            cumulative += result.reward;
            if (result.done) break;
        }

        total_reward += cumulative;
        if (env.state.goals_reached > 0) wins++;
        if (!env.state.frog.alive) deaths++;
    }

    ind->fitness = total_reward / (float)t->episodes_per_eval;
    ind->wins = wins;
    ind->deaths = deaths;
    ind->episodes = t->episodes_per_eval;
    return ind->fitness;
}

static int compare_individuals(const void* a, const void* b) {
    float fa = ((const GAIndividual*)a)->fitness;
    float fb = ((const GAIndividual*)b)->fitness;
    if (fa > fb) return -1;
    if (fa < fb) return 1;
    return 0;
}

void ga_run_generation(GATrainer* t, const FroggerConfig* cfg) {
    for (int i = 0; i < t->population_size; i++) {
        ga_evaluate_individual(t, i, cfg);
    }

    qsort(t->population, t->population_size, sizeof(GAIndividual), compare_individuals);

    if (t->population[0].fitness > t->best_fitness) {
        t->best_fitness = t->population[0].fitness;
        t->best_generation = t->generations;
        memcpy(&t->best_weights, &t->population[0].weights, sizeof(NeuroWeights));
    }

    RNG rng;
    rng_init(&rng, t->base_seed + (uint64_t)t->generations * 9973 + 1);

    int elite = t->elite_count;
    if (elite < 1) elite = 1;
    if (elite > t->population_size) elite = t->population_size;

    for (int i = elite; i < t->population_size; i++) {
        int parent_idx = i % elite;
        neuro_weights_mutate(&t->population[i].weights, &t->population[parent_idx].weights,
                             &rng, t->mutation_rate);
    }

    t->generations++;
}

void ga_train(GATrainer* t, const FroggerConfig* cfg, int generations) {
    for (int g = 0; g < generations; g++) {
        ga_run_generation(t, cfg);
        ga_print_status(t, t->generations - 1);
    }
}

void ga_print_status(const GATrainer* t, int gen) {
    printf("Gen %3d | Best: %8.3f (gen %d) | Cur best: %8.3f | W:%d D:%d\n",
           gen, t->best_fitness, t->best_generation,
           t->population[0].fitness,
           t->population[0].wins, t->population[0].deaths);
}
