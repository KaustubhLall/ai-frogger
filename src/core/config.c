#include "core/config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

static void parse_value(FroggerConfig* cfg, const char* key, const char* val) {
    if (strcmp(key, "width") == 0 || strcmp(key, "grid_width") == 0)
        cfg->grid_width = atoi(val);
    else if (strcmp(key, "height") == 0 || strcmp(key, "grid_height") == 0)
        cfg->grid_height = atoi(val);
    else if (strcmp(key, "max_steps") == 0)
        cfg->max_steps = atoi(val);
    else if (strcmp(key, "seed") == 0)
        cfg->seed = atoi(val);
    else if (strcmp(key, "river_start_row") == 0)
        cfg->river_start_row = atoi(val);
    else if (strcmp(key, "river_end_row") == 0)
        cfg->river_end_row = atoi(val);
    else if (strcmp(key, "road_start_row") == 0)
        cfg->road_start_row = atoi(val);
    else if (strcmp(key, "road_end_row") == 0)
        cfg->road_end_row = atoi(val);
    else if (strcmp(key, "goal_row") == 0)
        cfg->goal_row = atoi(val);
    else if (strcmp(key, "start_row") == 0)
        cfg->start_row = atoi(val);
    else if (strcmp(key, "log_speed_min") == 0)
        cfg->log_speed_min = (float)atof(val);
    else if (strcmp(key, "log_speed_max") == 0)
        cfg->log_speed_max = (float)atof(val);
    else if (strcmp(key, "car_speed_min") == 0)
        cfg->car_speed_min = (float)atof(val);
    else if (strcmp(key, "car_speed_max") == 0)
        cfg->car_speed_max = (float)atof(val);
    else if (strcmp(key, "log_length_min") == 0)
        cfg->log_length_min = atoi(val);
    else if (strcmp(key, "log_length_max") == 0)
        cfg->log_length_max = atoi(val);
    else if (strcmp(key, "log_spacing_min") == 0)
        cfg->log_spacing_min = atoi(val);
    else if (strcmp(key, "log_spacing_max") == 0)
        cfg->log_spacing_max = atoi(val);
    else if (strcmp(key, "car_length_min") == 0)
        cfg->car_length_min = atoi(val);
    else if (strcmp(key, "car_length_max") == 0)
        cfg->car_length_max = atoi(val);
    else if (strcmp(key, "car_spacing_min") == 0)
        cfg->car_spacing_min = atoi(val);
    else if (strcmp(key, "car_spacing_max") == 0)
        cfg->car_spacing_max = atoi(val);
    else if (strcmp(key, "river_lanes") == 0)
        cfg->river_lanes = atoi(val);
    else if (strcmp(key, "road_lanes") == 0)
        cfg->road_lanes = atoi(val);
    else if (strcmp(key, "forward_reward") == 0)
        cfg->forward_reward = (float)atof(val);
    else if (strcmp(key, "goal_reward") == 0)
        cfg->goal_reward = (float)atof(val);
    else if (strcmp(key, "death_penalty") == 0)
        cfg->death_penalty = (float)atof(val);
    else if (strcmp(key, "drowning_penalty") == 0)
        cfg->drowning_penalty = (float)atof(val);
    else if (strcmp(key, "timeout_penalty") == 0)
        cfg->timeout_penalty = (float)atof(val);
    else if (strcmp(key, "backward_penalty") == 0)
        cfg->backward_penalty = (float)atof(val);
    else if (strcmp(key, "stall_penalty") == 0)
        cfg->stall_penalty = (float)atof(val);
    else if (strcmp(key, "log_riding_reward") == 0)
        cfg->log_riding_reward = (float)atof(val);
    else if (strcmp(key, "invalid_action_penalty") == 0)
        cfg->invalid_action_penalty = (float)atof(val);
    else if (strcmp(key, "mode") == 0) {
        if (strcmp(val, "survival") == 0) cfg->mode = MODE_SURVIVAL;
        else cfg->mode = MODE_CROSSING;
    }
}

int config_load_json(FroggerConfig* cfg, const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return 0;

    config_defaults(cfg);

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (*p == ' ' || *p == '\t' || *p == ',') p++;

        if (*p == '{' || *p == '}' || *p == '\n' || *p == '\r' || *p == '\0') continue;

        char* colon = strchr(p, ':');
        if (!colon) continue;

        *colon = '\0';
        char* key = p;
        char* val = colon + 1;

        while (*key == ' ' || *key == '\t' || *key == '"') key++;
        char* kend = key + strlen(key) - 1;
        while (kend > key && (*kend == ' ' || *kend == '\t' || *kend == '"')) kend--;
        *(kend + 1) = '\0';

        while (*val == ' ' || *val == '\t') val++;
        if (*val == '"') {
            val++;
            char* vend = strchr(val, '"');
            if (vend) *vend = '\0';
        } else {
            char* vend = val + strlen(val) - 1;
            while (vend > val && (*vend == ' ' || *vend == '\t' || *vend == '\n' ||
                                  *vend == '\r' || *vend == ',')) vend--;
            *(vend + 1) = '\0';
        }

        parse_value(cfg, key, val);
    }

    fclose(f);
    return 1;
}
