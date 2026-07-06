#include "env/frogger_observation.h"
#include "env/frogger_map.h"
#include "core/math_util.h"
#include <string.h>
#include <math.h>

void obs_compute(const FroggerState* state, Observation* obs, const FroggerConfig* cfg) {
    memset(obs, 0, sizeof(Observation));
    obs->agent_x = state->frog.x;
    obs->agent_y = state->frog.y;
    obs->agent_alive = state->frog.alive;
    obs->goals_reached = state->goals_reached;
    obs->deaths = state->total_deaths;
    obs->step = state->step;

    for (int dy = 0; dy < LOCAL_OBS_SIZE; dy++) {
        for (int dx = 0; dx < LOCAL_OBS_SIZE; dx++) {
            int wx = state->frog.x - LOCAL_OBS_HALF + dx;
            int wy = state->frog.y - LOCAL_OBS_HALF + dy;
            if (map_in_bounds(state, wx, wy)) {
                obs->local_tiles[dy][dx] = (int)map_get_tile(state, wx, wy);
            } else {
                obs->local_tiles[dy][dx] = -1;
            }
        }
    }

    /* Car positions in local grid */
    for (int lane = 0; lane < MAX_LANES; lane++) {
        int lane_y = cfg->road_start_row + lane;
        if (lane_y < 0 || lane_y >= state->height) continue;
        for (int i = 0; i < state->car_count[lane]; i++) {
            const CarState* car = &state->cars[lane][i];
            if (!car->active) continue;
            for (int cx = 0; cx < car->length; cx++) {
                int car_x = (int)(car->x) + cx;
                int lx = car_x - state->frog.x + LOCAL_OBS_HALF;
                int ly = lane_y - state->frog.y + LOCAL_OBS_HALF;
                if (lx >= 0 && lx < LOCAL_OBS_SIZE && ly >= 0 && ly < LOCAL_OBS_SIZE) {
                    obs->local_cars[ly][lx] = car->speed > 0 ? 1.0f : -1.0f;
                }
            }
        }
    }

    /* Log positions in local grid */
    for (int lane = 0; lane < MAX_LANES; lane++) {
        int lane_y = cfg->river_start_row + lane;
        if (lane_y < 0 || lane_y >= state->height) continue;
        for (int i = 0; i < state->log_count[lane]; i++) {
            const LogState* log = &state->logs[lane][i];
            if (!log->active) continue;
            for (int lx2 = 0; lx2 < log->length; lx2++) {
                int log_x = (int)(log->x) + lx2;
                int ox = log_x - state->frog.x + LOCAL_OBS_HALF;
                int oy = lane_y - state->frog.y + LOCAL_OBS_HALF;
                if (ox >= 0 && ox < LOCAL_OBS_SIZE && oy >= 0 && oy < LOCAL_OBS_SIZE) {
                    obs->local_logs[oy][ox] = log->speed > 0 ? 1.0f : -1.0f;
                }
            }
        }
    }

    /* Valid actions */
    obs->valid_actions[ACTION_WAIT] = 1;
    if (map_in_bounds(state, state->frog.x, state->frog.y - 1))
        obs->valid_actions[ACTION_UP] = 1;
    if (map_in_bounds(state, state->frog.x, state->frog.y + 1))
        obs->valid_actions[ACTION_DOWN] = 1;
    if (map_in_bounds(state, state->frog.x - 1, state->frog.y))
        obs->valid_actions[ACTION_LEFT] = 1;
    if (map_in_bounds(state, state->frog.x + 1, state->frog.y))
        obs->valid_actions[ACTION_RIGHT] = 1;

    /* Distance to goal */
    obs->dist_to_goal = (float)absi(state->frog.y - cfg->goal_row);

    /* Nearest car/log distances */
    obs->nearest_car_dist = 999.0f;
    obs->nearest_log_dist = 999.0f;
    for (int lane = 0; lane < MAX_LANES; lane++) {
        int lane_y = cfg->road_start_row + lane;
        for (int i = 0; i < state->car_count[lane]; i++) {
            if (!state->cars[lane][i].active) continue;
            float d = absf((float)state->cars[lane][i].x - state->frog.x) +
                      (float)absi(lane_y - state->frog.y);
            if (d < obs->nearest_car_dist) obs->nearest_car_dist = d;
        }
        int river_y = cfg->river_start_row + lane;
        for (int i = 0; i < state->log_count[lane]; i++) {
            if (!state->logs[lane][i].active) continue;
            float d = absf((float)state->logs[lane][i].x - state->frog.x) +
                      (float)absi(river_y - state->frog.y);
            if (d < obs->nearest_log_dist) obs->nearest_log_dist = d;
        }
    }
}

void obs_to_flat(const Observation* obs, float* flat, int* size) {
    int idx = 0;
    flat[idx++] = (float)obs->agent_x;
    flat[idx++] = (float)obs->agent_y;
    flat[idx++] = (float)obs->agent_alive;
    flat[idx++] = (float)obs->goals_reached;
    flat[idx++] = (float)obs->deaths;
    flat[idx++] = (float)obs->step;

    for (int y = 0; y < LOCAL_OBS_SIZE; y++)
        for (int x = 0; x < LOCAL_OBS_SIZE; x++)
            flat[idx++] = (float)obs->local_tiles[y][x];

    for (int y = 0; y < LOCAL_OBS_SIZE; y++)
        for (int x = 0; x < LOCAL_OBS_SIZE; x++)
            flat[idx++] = obs->local_cars[y][x];

    for (int y = 0; y < LOCAL_OBS_SIZE; y++)
        for (int x = 0; x < LOCAL_OBS_SIZE; x++)
            flat[idx++] = obs->local_logs[y][x];

    for (int a = 0; a < ACTION_COUNT; a++)
        flat[idx++] = (float)obs->valid_actions[a];

    flat[idx++] = (float)obs->danger_up;
    flat[idx++] = (float)obs->danger_down;
    flat[idx++] = (float)obs->danger_left;
    flat[idx++] = (float)obs->danger_right;
    flat[idx++] = obs->nearest_car_dist;
    flat[idx++] = obs->nearest_log_dist;
    flat[idx++] = obs->dist_to_goal;

    *size = idx;
}
