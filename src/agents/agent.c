#include "agents/agent.h"
#include <string.h>

void agent_init(Agent* agent, AgentType type) {
    memset(agent, 0, sizeof(Agent));
    agent->type = type;
}

void agent_reset(Agent* agent, uint64_t seed) {
    rng_init(&agent->rng, seed);
    agent->decision_text[0] = '\0';
}

Action agent_act(Agent* agent, const Observation* obs, const DebugSnapshot* snap) {
    switch (agent->type) {
        case AGENT_RANDOM: {
            int valid[ACTION_COUNT];
            int count = 0;
            for (int a = 0; a < ACTION_COUNT; a++) {
                if (obs->valid_actions[a]) valid[count++] = a;
            }
            if (count == 0) return ACTION_WAIT;
            int choice = rng_range(&agent->rng, 0, count);
            strcpy(agent->decision_text, "random");
            return (Action)valid[choice];
        }
        case AGENT_SCRIPTED: {
            /* Move up when safe, avoid cars */
            if (obs->valid_actions[ACTION_UP]) {
                strcpy(agent->decision_text, "scripted: move up");
                return ACTION_UP;
            }
            if (obs->valid_actions[ACTION_RIGHT]) {
                strcpy(agent->decision_text, "scripted: move right");
                return ACTION_RIGHT;
            }
            strcpy(agent->decision_text, "scripted: wait");
            return ACTION_WAIT;
        }
        case AGENT_HEURISTIC: {
            /* Prioritize forward progress, avoid danger */
            if (obs->dist_to_goal < 3 && obs->valid_actions[ACTION_UP]) {
                strcpy(agent->decision_text, "heuristic: rush goal");
                return ACTION_UP;
            }
            if (obs->valid_actions[ACTION_UP] && obs->nearest_car_dist > 2.0f) {
                strcpy(agent->decision_text, "heuristic: move up safely");
                return ACTION_UP;
            }
            if (obs->valid_actions[ACTION_LEFT] && obs->nearest_car_dist > 1.5f) {
                strcpy(agent->decision_text, "heuristic: move left");
                return ACTION_LEFT;
            }
            if (obs->valid_actions[ACTION_RIGHT]) {
                strcpy(agent->decision_text, "heuristic: move right");
                return ACTION_RIGHT;
            }
            strcpy(agent->decision_text, "heuristic: wait");
            return ACTION_WAIT;
        }
        case AGENT_GREEDY: {
            /* Always move toward goal */
            if (obs->valid_actions[ACTION_UP]) {
                strcpy(agent->decision_text, "greedy: move up");
                return ACTION_UP;
            }
            if (obs->valid_actions[ACTION_RIGHT]) {
                strcpy(agent->decision_text, "greedy: move right");
                return ACTION_RIGHT;
            }
            if (obs->valid_actions[ACTION_LEFT]) {
                strcpy(agent->decision_text, "greedy: move left");
                return ACTION_LEFT;
            }
            strcpy(agent->decision_text, "greedy: wait");
            return ACTION_WAIT;
        }
        default:
            return ACTION_WAIT;
    }
}

AgentType agent_parse_type(const char* str) {
    if (strcmp(str, "random") == 0) return AGENT_RANDOM;
    if (strcmp(str, "scripted") == 0) return AGENT_SCRIPTED;
    if (strcmp(str, "heuristic") == 0) return AGENT_HEURISTIC;
    if (strcmp(str, "greedy") == 0) return AGENT_GREEDY;
    return AGENT_RANDOM;
}
