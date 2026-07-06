#include "viz/dashboard.h"
#include <stdio.h>

void dashboard_init(DashboardState* ds) {
    memset(ds, 0, sizeof(DashboardState));
}

void dashboard_add_event(DashboardState* ds, const char* event) {
    if (ds->event_count < MAX_DASH_EVENTS) {
        strncpy(ds->events[ds->event_count], event, MAX_EVENT_LEN - 1);
        ds->events[ds->event_count][MAX_EVENT_LEN - 1] = '\0';
        ds->event_count++;
    } else {
        memmove(ds->events, ds->events + 1, (MAX_DASH_EVENTS - 1) * MAX_EVENT_LEN);
        strncpy(ds->events[MAX_DASH_EVENTS - 1], event, MAX_EVENT_LEN - 1);
        ds->events[MAX_DASH_EVENTS - 1][MAX_EVENT_LEN - 1] = '\0';
    }
}

void dashboard_add_reward(DashboardState* ds, float reward) {
    if (ds->reward_count < MAX_DASH_REWARDS) {
        ds->reward_history[ds->reward_count++] = reward;
    } else {
        memmove(ds->reward_history, ds->reward_history + 1, (MAX_DASH_REWARDS - 1) * sizeof(float));
        ds->reward_history[MAX_DASH_REWARDS - 1] = reward;
    }
}

void dashboard_add_action(DashboardState* ds, Action action) {
    if (action >= 0 && action < ACTION_COUNT) {
        ds->action_counts[action]++;
        ds->total_actions++;
    }
}
