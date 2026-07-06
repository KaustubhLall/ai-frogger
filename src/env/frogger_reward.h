#ifndef FROGGER_REWARD_H
#define FROGGER_REWARD_H

#include "env/frogger_state.h"
#include "env/types.h"
#include "core/config.h"

typedef struct {
    float forward;
    float backward;
    float goal;
    float death_car;
    float death_water;
    float timeout;
    float stall;
    float log_riding;
    float invalid_action;
    float total;
} RewardBreakdown;

float reward_compute(RewardBreakdown* rb, const FroggerState* state,
                     Action action, int prev_y, int moved_valid,
                     const FroggerConfig* cfg);

#endif /* FROGGER_REWARD_H */
