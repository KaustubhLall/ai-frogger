#include "env/env.h"
#include "env/frogger_map.h"
#include <assert.h>
#include <stdio.h>

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

    int old_y = env.state.frog.y;
    r = env_step(&env, ACTION_UP);
    assert(env.state.frog.y == old_y - 1 || env.state.frog.y == old_y);

    env_reset(&env, 42);
    old_y = env.state.frog.y;
    r = env_step(&env, ACTION_UP);
    assert(env.state.frog.y == old_y - 1);

    printf("test_frogger_env: ALL PASSED\n");
    return 0;
}
