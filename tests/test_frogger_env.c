#include "env/env.h"
#include "env/frogger_map.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = 42;

    FroggerEnv env;
    env_init(&env, &cfg);

    assert(env.state.frog.alive == 1);
    assert(env.state.frog.x == cfg.grid_width / 2);
    assert(env.state.frog.y == cfg.start_row);

    StepResult r = env_step(&env, ACTION_WAIT);
    assert(r.done == 0);
    assert(env.state.step == 1);

    DebugSnapshot snap;
    env_get_debug_snapshot(&env, &snap);
    assert(snap.last_action == ACTION_WAIT);

    int old_y = env.state.frog.y;
    r = env_step(&env, ACTION_UP);
    assert(env.state.frog.y == old_y - 1 || env.state.frog.y == old_y);
    env_get_debug_snapshot(&env, &snap);
    assert(snap.last_action == ACTION_UP);

    env_reset(&env, 42);
    old_y = env.state.frog.y;
    r = env_step(&env, ACTION_UP);
    assert(env.state.frog.y == old_y - 1);

    env_reset(&env, 42);
    memset(env.state.car_count, 0, sizeof(env.state.car_count));
    memset(env.state.log_count, 0, sizeof(env.state.log_count));
    env.state.frog.x = 5;
    env.state.frog.y = cfg.river_start_row;
    env.state.frog.alive = 1;
    env.state.logs[0][0].x = 4.0f;
    env.state.logs[0][0].y = cfg.river_start_row;
    env.state.logs[0][0].length = 4;
    env.state.logs[0][0].speed = 1.0f;
    env.state.logs[0][0].active = 1;
    env.state.log_count[0] = 1;

    r = env_step(&env, ACTION_WAIT);
    assert(r.done == 0);
    assert(env.state.frog.on_log == 1);
    assert(env.state.frog.x == 5);

    printf("test_frogger_env: ALL PASSED\n");
    return 0;
}
