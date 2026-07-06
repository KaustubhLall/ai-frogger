#include "sim/runner.h"
#include <string.h>

void runner_run_single(FroggerEnv* env, Agent* agent, int max_steps, Metrics* metrics) {
    Observation obs;
    DebugSnapshot snap;
    float cumulative = 0.0f;

    for (int step = 0; step < max_steps; step++) {
        env_observe(env, &obs);
        env_get_debug_snapshot(env, &snap);
        Action action = agent_act(agent, &obs, &snap);
        StepResult result = env_step(env, action);
        cumulative += result.reward;
        metrics_update(metrics, result, action, cumulative);

        if (result.done) break;
    }
    metrics_end_episode(metrics, cumulative, env->state.goals_reached, env->state.total_deaths);
}

void runner_run(const RunConfig* rc, Metrics* metrics) {
    FroggerEnv env;
    Agent agent;

    for (int ep = 0; ep < rc->episodes; ep++) {
        env_init(&env, &rc->config);
        env_reset(&env, rc->seed + (uint64_t)ep);
        agent_init(&agent, rc->agent_type);
        agent_reset(&agent, rc->seed + (uint64_t)ep);
        runner_run_single(&env, &agent, rc->config.max_steps, metrics);
    }
}
