#include "env/frogger_reward.h"
#include "env/frogger_map.h"

float reward_compute(RewardBreakdown* rb, const FroggerState* state,
                     Action action, int prev_y, int moved_valid,
                     const FroggerConfig* cfg) {
    rb->forward = 0.0f;
    rb->backward = 0.0f;
    rb->goal = 0.0f;
    rb->death_car = 0.0f;
    rb->death_water = 0.0f;
    rb->timeout = 0.0f;
    rb->stall = 0.0f;
    rb->log_riding = 0.0f;
    rb->invalid_action = 0.0f;

    if (!moved_valid && action != ACTION_WAIT) {
        rb->invalid_action = cfg->invalid_action_penalty;
    }

    if (action == ACTION_WAIT) {
        rb->stall = cfg->stall_penalty;
    }

    int dy = state->frog.y - prev_y;
    if (dy < 0) {
        rb->forward = cfg->forward_reward;
    } else if (dy > 0) {
        rb->backward = cfg->backward_penalty;
    }

    if (state->frog.on_log) {
        rb->log_riding = cfg->log_riding_reward;
    }

    TileType tile = map_get_tile(state, state->frog.x, state->frog.y);
    if (tile == TILE_GOAL) {
        rb->goal = cfg->goal_reward;
    }

    /* Terminal penalties handled by caller via terminal_reason */
    rb->total = rb->forward + rb->backward + rb->goal + rb->death_car +
                rb->death_water + rb->timeout + rb->stall + rb->log_riding +
                rb->invalid_action;

    return rb->total;
}
