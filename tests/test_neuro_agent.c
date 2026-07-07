#include "env/env.h"
#include "agents/agent.h"
#include "agents/neuro_agent.h"
#include "core/rng.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = 1;
    cfg.max_steps = 50;

    NeuroWeights weights;
    RNG rng;
    rng_init(&rng, 42);
    neuro_weights_init(&weights, &rng);

    FroggerEnv env;
    Agent agent;
    Observation obs;
    DebugSnapshot snap;

    env_init(&env, &cfg);
    agent_init(&agent, AGENT_NEURO);
    agent.impl = &weights;
    agent_reset(&agent, 1);

    int actions_taken = 0;
    for (int i = 0; i < 50; i++) {
        env_observe(&env, &obs);
        env_get_debug_snapshot(&env, &snap);
        Action a = agent_act(&agent, &obs, &snap);
        assert(a >= 0 && a < ACTION_COUNT);
        assert(agent.decision_text[0] != '\0');
        assert(strncmp(agent.decision_text, "neuro:", 6) == 0);
        actions_taken++;
        StepResult r = env_step(&env, a);
        if (r.done) break;
    }
    assert(actions_taken > 0);

    assert(agent_parse_type("neuro") == AGENT_NEURO);

    const char* path = "test_neuro_weights.bin";
    assert(neuro_weights_save(&weights, path) == 1);

    NeuroWeights loaded;
    assert(neuro_weights_load(&loaded, path) == 1);

    for (int a = 0; a < ACTION_COUNT; a++) {
        for (int i = 0; i < NEURO_WEIGHT_SIZE; i++) {
            assert(loaded.weights[a][i] == weights.weights[a][i]);
        }
    }

    printf("test_neuro_agent: ALL PASSED\n");
    return 0;
}
