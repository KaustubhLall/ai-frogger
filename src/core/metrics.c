#include "core/metrics.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void metrics_init(Metrics* m) {
    memset(m, 0, sizeof(Metrics));
    m->start_time = (double)clock() / CLOCKS_PER_SEC;
}

void metrics_update(Metrics* m, StepResult result, Action action, float cumulative) {
    m->total_steps++;
    m->total_actions++;
    if (action >= 0 && action < ACTION_COUNT) m->action_counts[action]++;
    if (cumulative > m->max_reward) m->max_reward = cumulative;
    if (result.done && result.terminal_reason == TERMINAL_TIMEOUT) m->timeouts++;
}

void metrics_end_episode(Metrics* m, float cumulative, int goals, int deaths) {
    m->episodes++;
    m->total_reward += cumulative;
    if (goals > 0) m->wins++;
    else m->losses++;
    if (deaths > 0) m->deaths++;
}

void metrics_print(const Metrics* m) {
    double end_time = (double)clock() / CLOCKS_PER_SEC;
    double elapsed = end_time - m->start_time;
    if (elapsed < 0.001) elapsed = 0.001;

    printf("=== Metrics ===\n");
    printf("Episodes:          %d\n", m->episodes);
    printf("Total steps:       %d\n", m->total_steps);
    printf("Total reward:      %.2f\n", m->total_reward);
    printf("Avg reward/ep:     %.4f\n", m->episodes > 0 ? m->total_reward / m->episodes : 0.0f);
    printf("Wins:              %d\n", m->wins);
    printf("Losses:            %d\n", m->losses);
    printf("Deaths:            %d\n", m->deaths);
    printf("Timeouts:          %d\n", m->timeouts);
    printf("Win rate:          %.1f%%\n", m->episodes > 0 ? 100.0f * m->wins / m->episodes : 0.0f);
    printf("Action distribution:\n");
    const char* names[] = {"UP", "DOWN", "LEFT", "RIGHT", "WAIT"};
    for (int a = 0; a < ACTION_COUNT; a++) {
        printf("  %-6s: %d (%.1f%%)\n", names[a], m->action_counts[a],
               m->total_actions > 0 ? 100.0f * m->action_counts[a] / m->total_actions : 0.0f);
    }
    printf("Episodes/sec:      %.1f\n", m->episodes / elapsed);
    printf("Steps/sec:         %.1f\n", m->total_steps / elapsed);
    printf("Avg episode len:   %.1f\n", m->episodes > 0 ? (float)m->total_steps / m->episodes : 0.0f);
}
