#ifndef FROGGER_GENETIC_H
#define FROGGER_GENETIC_H

#include "core/config.h"
#include "agents/neuro_agent.h"

#define GA_MAX_POPULATION 64

typedef struct {
    NeuroWeights weights;
    float fitness;
    int wins;
    int deaths;
    int episodes;
} GAIndividual;

typedef struct {
    GAIndividual population[GA_MAX_POPULATION];
    int population_size;
    int elite_count;
    float mutation_rate;
    int generations;
    int episodes_per_eval;
    uint64_t base_seed;

    NeuroWeights best_weights;
    float best_fitness;
    int best_generation;
} GATrainer;

void ga_init(GATrainer* t, int population_size, int elite_count, float mutation_rate,
             int episodes_per_eval, uint64_t seed);
float ga_evaluate_individual(GATrainer* t, int idx, const FroggerConfig* cfg);
void ga_run_generation(GATrainer* t, const FroggerConfig* cfg);
void ga_train(GATrainer* t, const FroggerConfig* cfg, int generations);
void ga_print_status(const GATrainer* t, int gen);

#endif /* FROGGER_GENETIC_H */
