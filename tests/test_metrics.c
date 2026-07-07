#include "core/metrics.h"
#include "env/types.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    Metrics* m = (Metrics*)malloc(sizeof(Metrics));
    assert(m != NULL);
    metrics_init(m);

    for (int ep = 0; ep < 10; ep++) {
        StepResult result = {0.5f, 0, TERMINAL_NONE};
        for (int s = 0; s < 20; s++) {
            metrics_update(m, result, ACTION_UP, (float)(s + 1) * 0.5f);
        }
        TerminalReason reason = (ep % 3 == 0) ? TERMINAL_GOAL :
                                (ep % 3 == 1) ? TERMINAL_DEATH_CAR : TERMINAL_TIMEOUT;
        metrics_end_episode_full(m, 10.0f + ep, ep % 3 == 0 ? 1 : 0,
                                 ep % 3 == 1 ? 1 : 0, reason, 20);
    }

    assert(m->episodes == 10);
    assert(m->episode_count == 10);
    assert(m->terminal_counts[TERMINAL_GOAL] == 4);
    assert(m->terminal_counts[TERMINAL_DEATH_CAR] == 3);
    assert(m->terminal_counts[TERMINAL_TIMEOUT] == 3);

    metrics_print(m);
    metrics_print_terminal_breakdown(m);

    const char* json_path = "test_metrics.json";
    int ok = metrics_export_json(m, json_path);
    assert(ok == 1);

    FILE* f = fopen(json_path, "r");
    assert(f != NULL);
    char line[256];
    assert(fgets(line, sizeof(line), f) != NULL);
    assert(strstr(line, "{") != NULL);
    fclose(f);

    const char* csv_path = "test_metrics.csv";
    ok = metrics_export_csv(m, csv_path);
    assert(ok == 1);

    f = fopen(csv_path, "r");
    assert(f != NULL);
    int line_count = 0;
    while (fgets(line, sizeof(line), f)) line_count++;
    fclose(f);
    assert(line_count == 11);

    free(m);
    printf("test_metrics: ALL PASSED\n");
    return 0;
}
