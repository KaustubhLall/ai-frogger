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

void metrics_end_episode_full(Metrics* m, float cumulative, int goals, int deaths,
                              TerminalReason reason, int steps) {
    metrics_end_episode(m, cumulative, goals, deaths);
    if (reason >= 0 && reason < 6) m->terminal_counts[reason]++;
    if (reason == TERMINAL_TIMEOUT) m->timeouts++;
    if (m->episode_count < MAX_EPISODE_RECORDS) {
        m->episode_rewards[m->episode_count] = cumulative;
        m->episode_lengths[m->episode_count] = steps;
        m->episode_count++;
    }
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
    printf("Max reward:        %.2f\n", m->max_reward);
    printf("Wins:              %d\n", m->wins);
    printf("Losses:            %d\n", m->losses);
    printf("Deaths:            %d\n", m->deaths);
    printf("Timeouts:          %d\n", m->timeouts);
    printf("Win rate:          %.1f%%\n", m->episodes > 0 ? 100.0f * m->wins / m->episodes : 0.0f);
    printf("Action distribution:\n");
    static const char* names[] = {"UP", "DOWN", "LEFT", "RIGHT", "WAIT"};
    for (int a = 0; a < ACTION_COUNT; a++) {
        printf("  %-6s: %d (%.1f%%)\n", names[a], m->action_counts[a],
               m->total_actions > 0 ? 100.0f * m->action_counts[a] / m->total_actions : 0.0f);
    }
    printf("Episodes/sec:      %.1f\n", m->episodes / elapsed);
    printf("Steps/sec:         %.1f\n", m->total_steps / elapsed);
    printf("Avg episode len:   %.1f\n", m->episodes > 0 ? (float)m->total_steps / m->episodes : 0.0f);
}

void metrics_print_terminal_breakdown(const Metrics* m) {
    static const char* reasons[] = {"NONE", "GOAL", "DEATH_CAR", "DEATH_WATER", "TIMEOUT", "OFF_MAP"};
    printf("\n=== Terminal Reason Breakdown ===\n");
    for (int i = 0; i < 6; i++) {
        printf("  %-14s: %d (%.1f%%)\n", reasons[i], m->terminal_counts[i],
               m->episodes > 0 ? 100.0f * m->terminal_counts[i] / m->episodes : 0.0f);
    }
}

int metrics_export_csv(const Metrics* m, const char* path) {
    FILE* f = fopen(path, "w");
    if (!f) return 0;

    fprintf(f, "episode,reward,length\n");
    for (int i = 0; i < m->episode_count; i++) {
        fprintf(f, "%d,%.4f,%d\n", i, m->episode_rewards[i], m->episode_lengths[i]);
    }

    fclose(f);
    return 1;
}

int metrics_export_json(const Metrics* m, const char* path) {
    FILE* f = fopen(path, "w");
    if (!f) return 0;

    static const char* reasons[] = {"none", "goal", "death_car", "death_water", "timeout", "off_map"};
    static const char* action_names[] = {"up", "down", "left", "right", "wait"};

    fprintf(f, "{\n");
    fprintf(f, "  \"episodes\": %d,\n", m->episodes);
    fprintf(f, "  \"total_steps\": %d,\n", m->total_steps);
    fprintf(f, "  \"total_reward\": %.4f,\n", m->total_reward);
    fprintf(f, "  \"avg_reward\": %.4f,\n", m->episodes > 0 ? m->total_reward / m->episodes : 0.0f);
    fprintf(f, "  \"max_reward\": %.4f,\n", m->max_reward);
    fprintf(f, "  \"wins\": %d,\n", m->wins);
    fprintf(f, "  \"losses\": %d,\n", m->losses);
    fprintf(f, "  \"deaths\": %d,\n", m->deaths);
    fprintf(f, "  \"win_rate\": %.4f,\n", m->episodes > 0 ? (float)m->wins / m->episodes : 0.0f);
    fprintf(f, "  \"death_rate\": %.4f,\n", m->episodes > 0 ? (float)m->deaths / m->episodes : 0.0f);
    fprintf(f, "  \"avg_episode_length\": %.1f,\n", m->episodes > 0 ? (float)m->total_steps / m->episodes : 0.0f);
    fprintf(f, "  \"terminal_reasons\": {\n");
    for (int i = 0; i < 6; i++) {
        fprintf(f, "    \"%s\": %d%s\n", reasons[i], m->terminal_counts[i], i < 5 ? "," : "");
    }
    fprintf(f, "  },\n");
    fprintf(f, "  \"action_distribution\": {\n");
    for (int a = 0; a < ACTION_COUNT; a++) {
        fprintf(f, "    \"%s\": %d%s\n", action_names[a], m->action_counts[a], a < ACTION_COUNT - 1 ? "," : "");
    }
    fprintf(f, "  }\n");
    fprintf(f, "}\n");

    fclose(f);
    return 1;
}
