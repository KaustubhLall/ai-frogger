#include "env/env.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = 1337;
    cfg.max_steps = 100;

    FroggerEnv env1, env2;
    env_init(&env1, &cfg);
    env_init(&env2, &cfg);

    env_reset(&env1, 999);
    env_reset(&env2, 999);

    assert(env1.state.frog.x == env2.state.frog.x);
    assert(env1.state.frog.y == env2.state.frog.y);

    Action actions[] = {ACTION_UP, ACTION_DOWN, ACTION_WAIT, ACTION_LEFT,
                        ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_WAIT,
                        ACTION_UP, ACTION_UP};
    int n = (int)(sizeof(actions) / sizeof(actions[0]));

    for (int i = 0; i < n; i++) {
        StepResult r1 = env_step(&env1, actions[i]);
        StepResult r2 = env_step(&env2, actions[i]);
        assert(r1.reward == r2.reward);
        assert(r1.done == r2.done);
        assert(r1.terminal_reason == r2.terminal_reason);
    }

    assert(env1.state.step == env2.state.step);
    assert(env1.state.frog.x == env2.state.frog.x);
    assert(env1.state.frog.y == env2.state.frog.y);
    assert(env1.state.frog.alive == env2.state.frog.alive);

    env_reset(&env1, 111);
    env_reset(&env2, 222);
    assert(env1.state.frog.x == env2.state.frog.x);

    printf("test_determinism: ALL PASSED\n");
    return 0;
}
