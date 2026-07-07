#ifndef FROGGER_CONFIG_H
#define FROGGER_CONFIG_H

#include <stdint.h>

#define MAX_WIDTH 31
#define MAX_HEIGHT 31
#define MAX_LANES 16
#define MAX_LOGS_PER_LANE 32
#define MAX_LILYPADS 16
#define LOCAL_OBS_SIZE 11
#define MAX_REPLAY_STEPS 20000

typedef enum {
    MODE_CROSSING = 0,
    MODE_SURVIVAL
} GameMode;

typedef struct {
    int grid_width;
    int grid_height;
    GameMode mode;
    int max_steps;
    int seed;

    int river_start_row;
    int river_end_row;
    int road_start_row;
    int road_end_row;

    int goal_row;
    int start_row;

    float log_speed_min;
    float log_speed_max;
    int log_length_min;
    int log_length_max;
    int log_spacing_min;
    int log_spacing_max;
    int river_lanes;

    float car_speed_min;
    float car_speed_max;
    int car_length_min;
    int car_length_max;
    int car_spacing_min;
    int car_spacing_max;
    int road_lanes;

    /* Reward weights */
    float forward_reward;
    float goal_reward;
    float death_penalty;
    float drowning_penalty;
    float timeout_penalty;
    float backward_penalty;
    float stall_penalty;
    float log_riding_reward;
    float invalid_action_penalty;
} FroggerConfig;

void config_defaults(FroggerConfig* cfg);
void config_crossing(FroggerConfig* cfg);
void config_survival(FroggerConfig* cfg);
int config_load_json(FroggerConfig* cfg, const char* path);

#endif /* FROGGER_CONFIG_H */
