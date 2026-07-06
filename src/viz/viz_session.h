#ifndef VIZ_SESSION_H
#define VIZ_SESSION_H

#include "env/env.h"
#include "env/frogger_observation.h"
#include "agents/agent.h"
#include "viz/dashboard.h"

#define MAX_VIZ_AGENTS 8
#define MAX_VIZ_EPOCHS 2000

typedef enum {
    VIEW_ARENA = 0,
    VIEW_GRAPHS,
    VIEW_COMPARISON
} ViewMode;

typedef struct {
    Agent agent;
    FroggerEnv env;
    FroggerConfig config;
    char name[32];
    AgentType type;

    float epoch_rewards[MAX_VIZ_EPOCHS];
    float epoch_avg_rewards[MAX_VIZ_EPOCHS];
    int epoch_wins[MAX_VIZ_EPOCHS];
    int epoch_deaths[MAX_VIZ_EPOCHS];
    int epoch_lengths[MAX_VIZ_EPOCHS];
    int epoch_count;

    float total_reward;
    int total_wins;
    int total_deaths;

    float current_reward;
    int current_step;
    int episode_done;

    DashboardState dashboard;
} AgentSession;

typedef struct {
    AgentSession sessions[MAX_VIZ_AGENTS];
    int session_count;
    int active_session;
    int max_epochs;
    int paused;
    int speed_mult;
    int step_once;
    int auto_advance_epoch;
    ViewMode view_mode;
    uint64_t base_seed;
} VizSession;

void viz_session_init(VizSession* vs, int max_epochs, uint64_t base_seed);
int viz_session_add_agent(VizSession* vs, AgentType type, const char* name,
                          const FroggerConfig* config);
void viz_session_step(VizSession* vs);
void viz_session_reset_epoch(VizSession* vs, int session_idx);
void viz_session_reset_all(VizSession* vs);
void viz_session_switch_agent(VizSession* vs, int idx);
void viz_session_switch_view(VizSession* vs, ViewMode mode);
AgentSession* viz_session_active(VizSession* vs);

#endif /* VIZ_SESSION_H */
