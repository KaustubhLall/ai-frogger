#include "env/env.h"
#include "env/frogger_map.h"
#include "env/frogger_rules.h"
#include <string.h>

static void spawn_logs(FroggerEnv* env) {
    FroggerState* s = &env->state;
    const FroggerConfig* cfg = &env->config;
    for (int lane = 0; lane < cfg->river_lanes; lane++) {
        int y = cfg->river_start_row + lane;
        float speed = rng_float_range(&env->rng, cfg->log_speed_min, cfg->log_speed_max);
        int spacing = rng_range(&env->rng, cfg->log_spacing_min, cfg->log_spacing_max + 1);
        int length = rng_range(&env->rng, cfg->log_length_min, cfg->log_length_max + 1);
        int x = 0;
        if (lane % 2 == 1) speed = -speed;
        s->log_count[lane] = 0;
        while (x < s->width + 5 && s->log_count[lane] < MAX_LOGS_PER_LANE) {
            LogState* log = &s->logs[lane][s->log_count[lane]++];
            log->x = (float)x;
            log->y = y;
            log->length = length;
            log->speed = speed;
            log->active = 1;
            x += length + spacing;
        }
    }
}

static void spawn_cars(FroggerEnv* env) {
    FroggerState* s = &env->state;
    const FroggerConfig* cfg = &env->config;
    for (int lane = 0; lane < cfg->road_lanes; lane++) {
        int y = cfg->road_start_row + lane;
        float speed = rng_float_range(&env->rng, cfg->car_speed_min, cfg->car_speed_max);
        int spacing = rng_range(&env->rng, cfg->car_spacing_min, cfg->car_spacing_max + 1);
        int length = rng_range(&env->rng, cfg->car_length_min, cfg->car_length_max + 1);
        int x = 0;
        if (lane % 2 == 1) speed = -speed;
        s->car_count[lane] = 0;
        while (x < s->width + 5 && s->car_count[lane] < MAX_LOGS_PER_LANE) {
            CarState* car = &s->cars[lane][s->car_count[lane]++];
            car->x = (float)x;
            car->y = y;
            car->length = length;
            car->speed = speed;
            car->active = 1;
            x += length + spacing;
        }
    }
}

static void update_logs(FroggerEnv* env) {
    FroggerState* s = &env->state;
    for (int lane = 0; lane < env->config.river_lanes; lane++) {
        for (int i = 0; i < s->log_count[lane]; i++) {
            LogState* log = &s->logs[lane][i];
            if (!log->active) continue;
            log->x += log->speed * 0.1f;
            if (log->speed > 0 && log->x > s->width + 2) log->x = -log->length - 2.0f;
            else if (log->speed < 0 && log->x + log->length < -2) log->x = (float)(s->width + 2);
        }
    }
}

static void update_cars(FroggerEnv* env) {
    FroggerState* s = &env->state;
    for (int lane = 0; lane < env->config.road_lanes; lane++) {
        for (int i = 0; i < s->car_count[lane]; i++) {
            CarState* car = &s->cars[lane][i];
            if (!car->active) continue;
            car->x += car->speed * 0.1f;
            if (car->speed > 0 && car->x > s->width + 2) car->x = -car->length - 2.0f;
            else if (car->speed < 0 && car->x + car->length < -2) car->x = (float)(s->width + 2);
        }
    }
}

static void update_frog_on_log(FroggerEnv* env) {
    FroggerState* s = &env->state;
    TileType tile = map_get_tile(s, s->frog.x, s->frog.y);
    int was_on_log = s->frog.on_log;
    if (tile != TILE_RIVER) {
        s->frog.on_log = 0;
        s->frog.log_offset = 0.0f;
        return;
    }
    for (int lane = 0; lane < env->config.river_lanes; lane++) {
        int lane_y = env->config.river_start_row + lane;
        if (lane_y != s->frog.y) continue;
        for (int i = 0; i < s->log_count[lane]; i++) {
            LogState* log = &s->logs[lane][i];
            if (!log->active) continue;
            if ((float)s->frog.x + 0.5f >= log->x && (float)s->frog.x + 0.5f <= log->x + (float)log->length) {
                if (!was_on_log) s->frog.log_offset = (float)s->frog.x - log->x;
                s->frog.on_log = 1;
                s->frog.x = (int)(log->x + s->frog.log_offset + 0.5f);
                if (s->frog.x < 0 || s->frog.x >= s->width) s->frog.alive = 0;
                return;
            }
        }
    }
    s->frog.on_log = 0;
    s->frog.log_offset = 0.0f;
}

void env_init(FroggerEnv* env, const FroggerConfig* config) {
    memset(env, 0, sizeof(FroggerEnv));
    env->config = *config;
    rng_init(&env->rng, (uint64_t)config->seed);
    map_generate(&env->state, &env->config);
    env_reset(env, (uint64_t)config->seed);
}

void env_reset(FroggerEnv* env, uint64_t seed) {
    memset(&env->state, 0, sizeof(FroggerState));
    map_generate(&env->state, &env->config);
    rng_init(&env->rng, seed);
    env->state.frog.x = env->config.grid_width / 2;
    env->state.frog.y = env->config.start_row;
    env->state.frog.alive = 1;
    env->state.step = 0;
    env->last_action = ACTION_WAIT;
    spawn_logs(env);
    spawn_cars(env);
    ring_init(&env->action_history);
    env->prev_frog_y = env->state.frog.y;
    env->steps_since_progress = 0;
    danger_compute(&env->danger, &env->state, &env->config);
}

StepResult env_step(FroggerEnv* env, Action action) {
    StepResult result = {0.0f, 0, TERMINAL_NONE};
    FroggerState* s = &env->state;
    if (!s->frog.alive) {
        result.done = 1;
        result.terminal_reason = TERMINAL_OFF_MAP;
        return result;
    }
    int prev_y = s->frog.y;
    int moved_valid = rules_try_move(s, action, &env->config);
    ring_push(&env->action_history, (int)action);
    env->last_action = action;
    update_logs(env);
    update_cars(env);
    update_frog_on_log(env);
    s->step++;
    float reward = reward_compute(&env->last_reward, s, action, prev_y, moved_valid, &env->config);
    TerminalReason reason = rules_check_terminal(s, &env->config);
    if (reason != TERMINAL_NONE) {
        result.done = 1;
        result.terminal_reason = reason;
        if (reason == TERMINAL_GOAL) s->goals_reached++;
        else if (reason == TERMINAL_DEATH_CAR) { env->last_reward.death_car = env->config.death_penalty; s->frog.alive = 0; s->total_deaths++; }
        else if (reason == TERMINAL_DEATH_WATER || reason == TERMINAL_OFF_MAP) { env->last_reward.death_water = env->config.drowning_penalty; s->frog.alive = 0; s->total_deaths++; }
        else if (reason == TERMINAL_TIMEOUT) env->last_reward.timeout = env->config.timeout_penalty;
        env->last_reward.total += env->last_reward.death_car + env->last_reward.death_water + env->last_reward.timeout;
        reward = env->last_reward.total;
    }
    danger_compute(&env->danger, s, &env->config);
    result.reward = reward;
    return result;
}

void env_observe(const FroggerEnv* env, Observation* obs) { obs_compute(&env->state, obs, &env->config); }

void env_get_debug_snapshot(const FroggerEnv* env, DebugSnapshot* out) {
    out->state = env->state;
    out->danger = env->danger;
    out->last_reward = env->last_reward;
    out->last_action = env->last_action;
}
