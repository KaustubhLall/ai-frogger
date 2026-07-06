#ifndef FROGGER_METRICS_H
#define FROGGER_METRICS_H

#include "env/types.h"

#define MAX_EVENTS 256

typedef struct {
    int episodes;
    int wins;
    int losses;
    int timeouts;
    int deaths;
    int total_steps;
    float total_reward;
    float max_reward;
    int action_counts[ACTION_COUNT];
    int total_actions;
    double start_time;
    double end_time;
} Metrics;

void metrics_init(Metrics* m);
void metrics_update(Metrics* m, StepResult result, Action action, float cumulative);
void metrics_end_episode(Metrics* m, float cumulative, int goals, int deaths);
void metrics_print(const Metrics* m);

#endif /* FROGGER_METRICS_H */
