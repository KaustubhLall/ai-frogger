#include "viz/viz_session.h"
#include <string.h>
#include <stdio.h>

void viz_session_init(VizSession* vs, int max_epochs, uint64_t base_seed) {
    memset(vs, 0, sizeof(VizSession));
    vs->max_epochs = max_epochs;
    vs->base_seed = base_seed;
    vs->active_session = 0;
    vs->paused = 0;
    vs->speed_mult = 1;
    vs->step_once = 0;
    vs->auto_advance_epoch = 1;
    vs->view_mode = VIEW_ARENA;
}

int viz_session_add_agent(VizSession* vs, AgentType type, const char* name,
                          const FroggerConfig* config) {
    if (vs->session_count >= MAX_VIZ_AGENTS) return -1;
    int idx = vs->session_count;
    AgentSession* s = &vs->sessions[idx];

    memset(s, 0, sizeof(AgentSession));
    s->type = type;
    s->config = *config;
    strncpy(s->name, name, sizeof(s->name) - 1);

    agent_init(&s->agent, type);
    env_init(&s->env, &s->config);
    env_reset(&s->env, vs->base_seed);
    agent_reset(&s->agent, vs->base_seed);
    dashboard_init(&s->dashboard);
    s->epoch_count = 0;
    s->episode_done = 0;

    vs->session_count++;
    return idx;
}

void viz_session_reset_epoch(VizSession* vs, int session_idx) {
    if (session_idx < 0 || session_idx >= vs->session_count) return;
    AgentSession* s = &vs->sessions[session_idx];

    uint64_t ep_seed = vs->base_seed + (uint64_t)s->epoch_count;
    env_reset(&s->env, ep_seed);
    agent_reset(&s->agent, ep_seed);
    dashboard_init(&s->dashboard);
    s->current_reward = 0.0f;
    s->current_step = 0;
    s->episode_done = 0;
}

void viz_session_reset_all(VizSession* vs) {
    for (int i = 0; i < vs->session_count; i++) {
        AgentSession* s = &vs->sessions[i];
        s->epoch_count = 0;
        s->total_reward = 0.0f;
        s->total_wins = 0;
        s->total_deaths = 0;
        viz_session_reset_epoch(vs, i);
    }
}

void viz_session_switch_agent(VizSession* vs, int idx) {
    if (idx >= 0 && idx < vs->session_count) vs->active_session = idx;
}

void viz_session_switch_view(VizSession* vs, ViewMode mode) {
    vs->view_mode = mode;
}

AgentSession* viz_session_active(VizSession* vs) {
    if (vs->active_session < 0 || vs->active_session >= vs->session_count) return NULL;
    return &vs->sessions[vs->active_session];
}

static void record_epoch(VizSession* vs, int session_idx) {
    AgentSession* s = &vs->sessions[session_idx];
    if (s->epoch_count >= MAX_VIZ_EPOCHS) return;

    int idx = s->epoch_count;
    s->epoch_rewards[idx] = s->current_reward;
    s->epoch_lengths[idx] = s->current_step;

    int won = (s->env.state.goals_reached > 0) ? 1 : 0;
    int died = !s->env.state.frog.alive;

    s->epoch_wins[idx] = won;
    s->epoch_deaths[idx] = died;

    s->total_reward += s->current_reward;
    s->total_wins += won;
    s->total_deaths += died;
    s->epoch_count++;
    s->epoch_avg_rewards[idx] = s->total_reward / (float)s->epoch_count;
}

void viz_session_step(VizSession* vs) {
    for (int i = 0; i < vs->session_count; i++) {
        AgentSession* s = &vs->sessions[i];
        if (s->episode_done) {
            if (vs->auto_advance_epoch && s->epoch_count < vs->max_epochs) {
                record_epoch(vs, i);
                viz_session_reset_epoch(vs, i);
            }
            continue;
        }

        for (int sp = 0; sp < vs->speed_mult; sp++) {
            if (s->episode_done) break;

            Action action;
            char decision_label[256];

            if (vs->is_replay) {
                if (vs->replay_step_idx >= vs->replay_action_count) {
                    s->episode_done = 1;
                    break;
                }
                action = vs->replay_actions[vs->replay_step_idx];
                snprintf(decision_label, sizeof(decision_label), "replay: step %d", vs->replay_step_idx);
                vs->replay_step_idx++;
            } else {
                Observation obs;
                DebugSnapshot snap;
                env_observe(&s->env, &obs);
                env_get_debug_snapshot(&s->env, &snap);
                action = agent_act(&s->agent, &obs, &snap);
                strncpy(decision_label, s->agent.decision_text, sizeof(decision_label) - 1);
                decision_label[sizeof(decision_label) - 1] = '\0';
            }

            StepResult result = env_step(&s->env, action);

            s->last_action = action;
            strncpy(s->last_decision_text, decision_label, sizeof(s->last_decision_text) - 1);
            s->last_decision_text[sizeof(s->last_decision_text) - 1] = '\0';

            dashboard_add_action(&s->dashboard, action);
            dashboard_add_reward(&s->dashboard, result.reward);
            s->current_reward += result.reward;
            s->current_step++;

            if (result.done) {
                s->episode_done = 1;
                break;
            }
        }
    }
}
