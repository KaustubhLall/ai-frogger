#ifndef FROGGER_AGENT_H
#define FROGGER_AGENT_H

#include "env/env.h"
#include "env/frogger_observation.h"

typedef enum {
    AGENT_RANDOM = 0,
    AGENT_SCRIPTED,
    AGENT_HEURISTIC,
    AGENT_GREEDY,
    AGENT_NEURO
} AgentType;

typedef struct Agent {
    AgentType type;
    RNG rng;
    char decision_text[256];
    void* impl;
} Agent;

void agent_init(Agent* agent, AgentType type);
void agent_reset(Agent* agent, uint64_t seed);
Action agent_act(Agent* agent, const Observation* obs, const DebugSnapshot* snap);
AgentType agent_parse_type(const char* str);

#endif /* FROGGER_AGENT_H */
