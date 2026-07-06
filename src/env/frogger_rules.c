#include "env/frogger_rules.h"
#include "env/frogger_map.h"
#include "core/math_util.h"

int rules_try_move(FroggerState* state, Action action, const FroggerConfig* cfg) {
    int old_x = state->frog.x;
    int old_y = state->frog.y;
    int new_x = old_x;
    int new_y = old_y;

    switch (action) {
        case ACTION_UP:    new_y = old_y - 1; break;
        case ACTION_DOWN:  new_y = old_y + 1; break;
        case ACTION_LEFT:  new_x = old_x - 1; break;
        case ACTION_RIGHT: new_x = old_x + 1; break;
        case ACTION_WAIT:  break;
        default: return 0;
    }

    if (!map_in_bounds(state, new_x, new_y)) {
        return 0;
    }

    state->frog.x = new_x;
    state->frog.y = new_y;
    state->frog.on_log = 0;
    state->frog.log_offset = 0.0f;
    return 1;
}

int rules_check_collision(const FroggerState* state) {
    TileType tile = map_get_tile(state, state->frog.x, state->frog.y);
    if (tile != TILE_ROAD) return 0;

    int lane = state->frog.y - 7; /* road_start_row = 7 */
    if (lane < 0 || lane >= MAX_LANES) return 0;

    for (int i = 0; i < state->car_count[lane]; i++) {
        const CarState* car = &state->cars[lane][i];
        if (!car->active) continue;
        float car_left = car->x;
        float car_right = car->x + car->length;
        if ((float)state->frog.x + 0.5f >= car_left && (float)state->frog.x + 0.5f <= car_right) {
            return 1;
        }
    }
    return 0;
}

int rules_check_drowning(const FroggerState* state) {
    TileType tile = map_get_tile(state, state->frog.x, state->frog.y);
    if (tile != TILE_RIVER) return 0;
    return !state->frog.on_log;
}

TerminalReason rules_check_terminal(const FroggerState* state, const FroggerConfig* cfg) {
    TileType tile = map_get_tile(state, state->frog.x, state->frog.y);

    if (tile == TILE_GOAL) return TERMINAL_GOAL;
    if (rules_check_collision(state)) return TERMINAL_DEATH_CAR;
    if (rules_check_drowning(state)) return TERMINAL_DEATH_WATER;
    if (state->step >= cfg->max_steps) return TERMINAL_TIMEOUT;
    return TERMINAL_NONE;
}
