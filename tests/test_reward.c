#include "env/env.h"
#include "env/frogger_reward.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

int main(void) {
    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = 1;

    FroggerEnv env;
    env_init(&env, &cfg);

    int prev_y = env.state.frog.y;
    float reward = reward_compute(&env.last_reward, &env.state, ACTION_WAIT,
                                   prev_y, 1, &cfg);

    assert(env.last_reward.stall != 0.0f);

    float sum = env.last_reward.forward + env.last_reward.backward +
                env.last_reward.goal + env.last_reward.death_car +
                env.last_reward.death_water + env.last_reward.timeout +
                env.last_reward.stall + env.last_reward.log_riding +
                env.last_reward.invalid_action;
    assert(fabsf(sum - env.last_reward.total) < 0.001f);

    printf("test_reward: ALL PASSED\n");
    return 0;
}
