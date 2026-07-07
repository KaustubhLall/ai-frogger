#ifndef FROGGER_METRICS_H
#define FROGGER_METRICS_H

#include "env/types.h"

#define MAX_EVENTS 256
#define MAX_EPISODE_RECORDS 1000

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

    int terminal_counts[6];
    float episode_rewards[MAX_EPISODE_RECORDS];
    int episode_lengths[MAX_EPISODE_RECORDS];
    int episode_count;
} Metrics;

void metrics_init(Metrics* m);
void metrics_update(Metrics* m, StepResult result, Action action, float cumulative);
void metrics_end_episode(Metrics* m, float cumulative, int goals, int deaths);
void metrics_end_episode_full(Metrics* m, float cumulative, int goals, int deaths,
                              TerminalReason reason, int steps);
void metrics_print(const Metrics* m);
void metrics_print_terminal_breakdown(const Metrics* m);
int metrics_export_csv(const Metrics* m, const char* path);
int metrics_export_json(const Metrics* m, const char* path);

#endif /* FROGGER_METRICS_H */
