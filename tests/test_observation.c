#include "env/env.h"
#include "env/frogger_observation.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = 1;

    FroggerEnv env;
    env_init(&env, &cfg);

    Observation obs;
    env_observe(&env, &obs);

    assert(obs.agent_x == env.state.frog.x);
    assert(obs.agent_y == env.state.frog.y);
    assert(obs.agent_alive == 1);

    assert(LOCAL_OBS_SIZE == 11);

    int cx = LOCAL_OBS_HALF;
    int cy = LOCAL_OBS_HALF;
    assert(obs.local_tiles[cy][cx] == (int)TILE_GRASS);

    assert(obs.valid_actions[ACTION_WAIT] == 1);

    float flat[OBS_FLAT_SIZE];
    int flat_size;
    obs_to_flat(&obs, flat, &flat_size);
    assert(flat_size > 0);
    assert(flat_size <= OBS_FLAT_SIZE);

    Observation obs2;
    env_observe(&env, &obs2);
    assert(obs.agent_x == obs2.agent_x);
    assert(obs.agent_y == obs2.agent_y);
    assert(memcmp(obs.local_tiles, obs2.local_tiles, sizeof(obs.local_tiles)) == 0);

    float flat2[OBS_FLAT_SIZE];
    int flat2_size;
    obs_to_flat(&obs2, flat2, &flat2_size);
    assert(flat_size == flat2_size);
    for (int i = 0; i < flat_size; i++) {
        assert(flat[i] == flat2[i]);
    }

    printf("test_observation: ALL PASSED (flat_size=%d)\n", flat_size);
    return 0;
}
