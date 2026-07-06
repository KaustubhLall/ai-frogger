#ifndef FROGGER_RUNNER_H
#define FROGGER_RUNNER_H

#include "env/env.h"
#include "agents/agent.h"
#include "core/metrics.h"

typedef struct {
    FroggerConfig config;
    AgentType agent_type;
    uint64_t seed;
    int episodes;
    int record_replay;
} RunConfig;

void runner_run_single(FroggerEnv* env, Agent* agent, int max_steps, Metrics* metrics);
void runner_run(const RunConfig* rc, Metrics* metrics);

#endif /* FROGGER_RUNNER_H */
