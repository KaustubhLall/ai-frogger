#include "core/config.h"

void config_defaults(FroggerConfig* cfg) {
    cfg->grid_width = 13;
    cfg->grid_height = 14;
    cfg->mode = MODE_CROSSING;
    cfg->max_steps = 500;
    cfg->seed = 1337;

    cfg->start_row = 13;
    cfg->goal_row = 0;
    cfg->river_start_row = 1;
    cfg->river_end_row = 6;
    cfg->road_start_row = 7;
    cfg->road_end_row = 12;

    cfg->log_speed_min = 0.5f;
    cfg->log_speed_max = 2.0f;
    cfg->log_length_min = 2;
    cfg->log_length_max = 5;
    cfg->log_spacing_min = 2;
    cfg->log_spacing_max = 5;
    cfg->river_lanes = 6;

    cfg->car_speed_min = 0.5f;
    cfg->car_speed_max = 2.5f;
    cfg->car_length_min = 1;
    cfg->car_length_max = 3;
    cfg->car_spacing_min = 3;
    cfg->car_spacing_max = 6;
    cfg->road_lanes = 6;

    cfg->forward_reward = 0.1f;
    cfg->goal_reward = 5.0f;
    cfg->death_penalty = -1.0f;
    cfg->drowning_penalty = -0.8f;
    cfg->timeout_penalty = -0.1f;
    cfg->backward_penalty = -0.05f;
    cfg->stall_penalty = -0.01f;
    cfg->log_riding_reward = 0.02f;
    cfg->invalid_action_penalty = -0.05f;
}

void config_crossing(FroggerConfig* cfg) {
    config_defaults(cfg);
    cfg->mode = MODE_CROSSING;
}

void config_survival(FroggerConfig* cfg) {
    config_defaults(cfg);
    cfg->mode = MODE_SURVIVAL;
    cfg->max_steps = 1000;
}
