#include "env/env.h"
#include "env/frogger_map.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    FroggerConfig cfg;
    config_crossing(&cfg);
    cfg.seed = 100;

    FroggerEnv env;
    env_init(&env, &cfg);

    assert(env.state.width == cfg.grid_width);
    assert(env.state.height == cfg.grid_height);

    assert(map_get_tile(&env.state, 0, cfg.goal_row) == TILE_GOAL);
    for (int y = cfg.river_start_row; y <= cfg.river_end_row; y++) {
        assert(map_get_tile(&env.state, 0, y) == TILE_RIVER);
    }
    for (int y = cfg.road_start_row; y <= cfg.road_end_row; y++) {
        assert(map_get_tile(&env.state, 0, y) == TILE_ROAD);
    }

    assert(map_in_bounds(&env.state, 0, 0) == 1);
    assert(map_in_bounds(&env.state, -1, 0) == 0);
    assert(map_in_bounds(&env.state, cfg.grid_width, 0) == 0);

    FroggerEnv env2;
    env_init(&env2, &cfg);
    env_reset(&env2, 100);
    env_reset(&env, 100);
    for (int y = 0; y < cfg.grid_height; y++) {
        assert(env.state.tiles[y][0] == env2.state.tiles[y][0]);
    }

    printf("test_map: ALL PASSED\n");
    return 0;
}
