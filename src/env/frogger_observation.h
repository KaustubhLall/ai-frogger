#ifndef FROGGER_OBSERVATION_H
#define FROGGER_OBSERVATION_H

#include "env/frogger_state.h"
#include "env/types.h"
#include "core/config.h"

#define LOCAL_OBS_HALF (LOCAL_OBS_SIZE / 2)
#define OBS_FLAT_SIZE 512

typedef struct {
    int agent_x;
    int agent_y;
    int agent_alive;
    int goals_reached;
    int deaths;
    int step;

    int local_tiles[LOCAL_OBS_SIZE][LOCAL_OBS_SIZE];
    float local_cars[LOCAL_OBS_SIZE][LOCAL_OBS_SIZE];
    float local_logs[LOCAL_OBS_SIZE][LOCAL_OBS_SIZE];

    int valid_actions[ACTION_COUNT];
    int danger_up;
    int danger_down;
    int danger_left;
    int danger_right;

    float nearest_car_dist;
    float nearest_log_dist;
    float dist_to_goal;
} Observation;

void obs_compute(const FroggerState* state, Observation* obs, const FroggerConfig* cfg);
void obs_to_flat(const Observation* obs, float* flat, int* size);

#endif /* FROGGER_OBSERVATION_H */
