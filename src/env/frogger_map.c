#include "env/frogger_map.h"
#include <string.h>

void map_generate(FroggerState* state, const FroggerConfig* cfg) {
    state->width = cfg->grid_width;
    state->height = cfg->grid_height;

    for (int y = 0; y < state->height; y++) {
        for (int x = 0; x < state->width; x++) {
            if (y == cfg->goal_row) {
                state->tiles[y][x] = TILE_GOAL;
            } else if (y >= cfg->river_start_row && y <= cfg->river_end_row) {
                state->tiles[y][x] = TILE_RIVER;
            } else if (y >= cfg->road_start_row && y <= cfg->road_end_row) {
                state->tiles[y][x] = TILE_ROAD;
            } else {
                state->tiles[y][x] = TILE_GRASS;
            }
        }
    }

    /* Median strip between river and road */
    int median_row = cfg->river_end_row + 1;
    if (median_row < state->height && median_row == cfg->road_start_row - 1) {
        for (int x = 0; x < state->width; x++) {
            state->tiles[median_row][x] = TILE_MEDIAN;
        }
    }
}

int map_in_bounds(const FroggerState* state, int x, int y) {
    return x >= 0 && x < state->width && y >= 0 && y < state->height;
}

TileType map_get_tile(const FroggerState* state, int x, int y) {
    if (!map_in_bounds(state, x, y)) return TILE_GRASS;
    return state->tiles[y][x];
}
