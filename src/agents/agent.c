#include "agents/agent.h"
#include "agents/neuro_agent.h"
#include <string.h>

static int action_is_dangerous(const Observation* obs, Action action) {
    switch (action) {
        case ACTION_UP: return obs->danger_up;
        case ACTION_DOWN: return obs->danger_down;
        case ACTION_LEFT: return obs->danger_left;
        case ACTION_RIGHT: return obs->danger_right;
        case ACTION_WAIT: return 0;
        default: return 1;
    }
}

static int action_is_safe(const Observation* obs, Action action) {
    return action >= 0 && action < ACTION_COUNT &&
           obs->valid_actions[action] && !action_is_dangerous(obs, action);
}

static Action first_valid_action(const Observation* obs) {
    for (int a = 0; a < ACTION_COUNT; a++) {
        if (obs->valid_actions[a]) return (Action)a;
    }
    return ACTION_WAIT;
}

void agent_init(Agent* agent, AgentType type) {
    memset(agent, 0, sizeof(Agent));
    agent->type = type;
}

void agent_reset(Agent* agent, uint64_t seed) {
    rng_init(&agent->rng, seed);
    agent->decision_text[0] = '\0';
}

Action agent_act(Agent* agent, const Observation* obs, const DebugSnapshot* snap) {
    (void)snap;

    switch (agent->type) {
        case AGENT_RANDOM: {
            int valid[ACTION_COUNT];
            int count = 0;
            for (int a = 0; a < ACTION_COUNT; a++) {
                if (obs->valid_actions[a]) valid[count++] = a;
            }
            if (count == 0) return ACTION_WAIT;
            int choice = rng_range(&agent->rng, 0, count);
            strcpy(agent->decision_text, "random baseline");
            return (Action)valid[choice];
        }
        case AGENT_SCRIPTED: {
            if (action_is_safe(obs, ACTION_UP)) {
                strcpy(agent->decision_text, "scripted: safe forward");
                return ACTION_UP;
            }
            if (action_is_safe(obs, ACTION_RIGHT)) {
                strcpy(agent->decision_text, "scripted: dodge right");
                return ACTION_RIGHT;
            }
            if (action_is_safe(obs, ACTION_LEFT)) {
                strcpy(agent->decision_text, "scripted: dodge left");
                return ACTION_LEFT;
            }
            strcpy(agent->decision_text, "scripted: hold");
            return obs->valid_actions[ACTION_WAIT] ? ACTION_WAIT : first_valid_action(obs);
        }
        case AGENT_HEURISTIC: {
            if (obs->dist_to_goal < 3 && action_is_safe(obs, ACTION_UP)) {
                strcpy(agent->decision_text, "heuristic: safe goal push");
                return ACTION_UP;
            }
            if (action_is_safe(obs, ACTION_UP)) {
                strcpy(agent->decision_text, "heuristic: safe progress");
                return ACTION_UP;
            }
            if (action_is_safe(obs, ACTION_LEFT) && !action_is_safe(obs, ACTION_RIGHT)) {
                strcpy(agent->decision_text, "heuristic: only left is safe");
                return ACTION_LEFT;
            }
            if (action_is_safe(obs, ACTION_RIGHT)) {
                strcpy(agent->decision_text, "heuristic: sidestep right");
                return ACTION_RIGHT;
            }
            if (action_is_safe(obs, ACTION_LEFT)) {
                strcpy(agent->decision_text, "heuristic: sidestep left");
                return ACTION_LEFT;
            }
            strcpy(agent->decision_text, "heuristic: wait for opening");
            return obs->valid_actions[ACTION_WAIT] ? ACTION_WAIT : first_valid_action(obs);
        }
        case AGENT_GREEDY: {
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
        case AGENT_NEURO: {
            if (agent->impl) {
                return neuro_act((NeuroWeights*)agent->impl, obs,
                                 agent->decision_text, sizeof(agent->decision_text));
            }
            strcpy(agent->decision_text, "neuro: no weights (random fallback)");
            int valid[ACTION_COUNT];
            int count = 0;
            for (int a = 0; a < ACTION_COUNT; a++) {
                if (obs->valid_actions[a]) valid[count++] = a;
            }
            if (count == 0) return ACTION_WAIT;
            int choice = rng_range(&agent->rng, 0, count);
            return (Action)valid[choice];
        }
        default:
            return ACTION_WAIT;
    }
}

AgentType agent_parse_type(const char* str) {
    if (str == NULL) return AGENT_RANDOM;
    if (strcmp(str, "random") == 0) return AGENT_RANDOM;
    if (strcmp(str, "scripted") == 0) return AGENT_SCRIPTED;
    if (strcmp(str, "heuristic") == 0) return AGENT_HEURISTIC;
    if (strcmp(str, "greedy") == 0) return AGENT_GREEDY;
    if (strcmp(str, "neuro") == 0) return AGENT_NEURO;
    return AGENT_RANDOM;
}
