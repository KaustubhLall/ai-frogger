#include "train/genetic.h"
#include "core/config.h"
#include "agents/agent.h"
#include "sim/runner.h"
#include "core/metrics.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static float eval_weights(const NeuroWeights* w, const FroggerConfig* cfg, int episodes, uint64_t seed) {
    Agent agent;
    agent_init(&agent, AGENT_NEURO);
    agent.impl = (NeuroWeights*)w;

    Metrics* m = (Metrics*)malloc(sizeof(Metrics));
    metrics_init(m);

    for (int ep = 0; ep < episodes; ep++) {
        FroggerEnv env;
        env_init(&env, cfg);
        env_reset(&env, seed + (uint64_t)ep);
        agent_reset(&agent, seed + (uint64_t)ep);
        runner_run_single(&env, &agent, cfg->max_steps, m);
    }

    float avg = m->episodes > 0 ? m->total_reward / m->episodes : 0.0f;
    free(m);
    return avg;
}

int main(void) {
    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = 42;
    cfg.max_steps = 100;

    GATrainer* trainer = (GATrainer*)malloc(sizeof(GATrainer));
    assert(trainer != NULL);
    ga_init(trainer, 8, 3, 0.1f, 3, 42);

    assert(trainer->population_size == 8);
    assert(trainer->elite_count == 3);
    assert(trainer->mutation_rate == 0.1f);
    assert(trainer->episodes_per_eval == 3);

    /* Evaluate initial random population's best before any training */
    ga_run_generation(trainer, &cfg);
    assert(trainer->generations == 1);
    assert(trainer->population[0].fitness >= trainer->population[1].fitness);

    float initial_best = trainer->best_fitness;
    assert(initial_best > -1e30f);

    /* Train for several more generations */
    for (int g = 0; g < 5; g++) {
        ga_run_generation(trainer, &cfg);
    }
    assert(trainer->generations == 6);

    /* best_fitness should be monotonically non-decreasing across generations */
    assert(trainer->best_fitness >= initial_best);

    /* Held-out evaluation should produce finite, sane rewards */
    float trained_avg = eval_weights(&trainer->best_weights, &cfg, 5, 9999);
    assert(isfinite(trained_avg));
    assert(trained_avg > -10000.0f);
    printf("Trained best avg reward (held-out): %.3f\n", trained_avg);

    /* Verify save/load round-trip preserves trained weights */
    const char* path = "test_ga_weights.bin";
    assert(neuro_weights_save(&trainer->best_weights, path) == 1);
    NeuroWeights loaded;
    assert(neuro_weights_load(&loaded, path) == 1);
    for (int a = 0; a < ACTION_COUNT; a++) {
        for (int i = 0; i < NEURO_WEIGHT_SIZE; i++) {
            assert(loaded.weights[a][i] == trainer->best_weights.weights[a][i]);
        }
    }

    free(trainer);
    printf("test_genetic: ALL PASSED\n");
    return 0;
}
