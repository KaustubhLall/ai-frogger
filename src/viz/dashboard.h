#ifndef FROGGER_DASHBOARD_H
#define FROGGER_DASHBOARD_H

#include "env/types.h"
#include <string.h>

#define MAX_DASH_EVENTS 64
#define MAX_DASH_REWARDS 512
#define MAX_EVENT_LEN 128

typedef struct {
    char events[MAX_DASH_EVENTS][MAX_EVENT_LEN];
    int event_count;
    float reward_history[MAX_DASH_REWARDS];
    int reward_count;
    int action_counts[ACTION_COUNT];
    int total_actions;
} DashboardState;

void dashboard_init(DashboardState* ds);
void dashboard_add_event(DashboardState* ds, const char* event);
void dashboard_add_reward(DashboardState* ds, float reward);
void dashboard_add_action(DashboardState* ds, Action action);

#endif /* FROGGER_DASHBOARD_H */
