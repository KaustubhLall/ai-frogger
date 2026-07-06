#ifndef FROGGER_DANGER_H
#define FROGGER_DANGER_H

#include "env/frogger_state.h"
#include "core/config.h"

typedef struct {
    int car_danger[MAX_HEIGHT][32];  /* 1 if car at position */
    int water_danger[MAX_HEIGHT];     /* 1 if river lane has no log at frog x */
} DangerMap;

void danger_compute(DangerMap* dm, const FroggerState* state, const FroggerConfig* cfg);
int danger_is_safe(const DangerMap* dm, int x, int y, const FroggerState* state);

#endif /* FROGGER_DANGER_H */
