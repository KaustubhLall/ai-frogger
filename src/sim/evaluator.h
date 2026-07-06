#ifndef FROGGER_EVALUATOR_H
#define FROGGER_EVALUATOR_H

#include "core/config.h"
#include "agents/agent.h"

typedef struct {
    float avg_reward;
    float win_rate;
    float death_rate;
    float avg_length;
    int episodes;
} EvalResult;

EvalResult evaluator_run(const FroggerConfig* cfg, AgentType type, int episodes, uint64_t seed);
void evaluator_compare(const FroggerConfig* cfg, int episodes, uint64_t seed);

#endif /* FROGGER_EVALUATOR_H */
