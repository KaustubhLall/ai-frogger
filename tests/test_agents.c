#include "env/env.h"
#include "agents/agent.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = 1;
    cfg.max_steps = 50;

    FroggerEnv env;
    Agent agent;
    Observation obs;
    DebugSnapshot snap;

    const AgentType types[] = {AGENT_RANDOM, AGENT_SCRIPTED, AGENT_HEURISTIC, AGENT_GREEDY};
    const char* names[] = {"random", "scripted", "heuristic", "greedy"};
    int n = (int)(sizeof(types) / sizeof(types[0]));

    for (int t = 0; t < n; t++) {
        env_init(&env, &cfg);
        agent_init(&agent, types[t]);
        agent_reset(&agent, 1);

        for (int i = 0; i < 50; i++) {
            env_observe(&env, &obs);
            env_get_debug_snapshot(&env, &snap);
            Action a = agent_act(&agent, &obs, &snap);
            StepResult r = env_step(&env, a);
            if (r.done) break;
        }
    }

    assert(agent_parse_type("random") == AGENT_RANDOM);
    assert(agent_parse_type("scripted") == AGENT_SCRIPTED);
    assert(agent_parse_type("heuristic") == AGENT_HEURISTIC);
    assert(agent_parse_type("greedy") == AGENT_GREEDY);

    printf("test_agents: ALL PASSED\n");
    return 0;
}
