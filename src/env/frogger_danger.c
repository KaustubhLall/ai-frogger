#include "env/frogger_danger.h"
#include "env/frogger_map.h"
#include <string.h>

void danger_compute(DangerMap* dm, const FroggerState* state, const FroggerConfig* cfg) {
    memset(dm, 0, sizeof(DangerMap));

    for (int lane = 0; lane < MAX_LANES; lane++) {
        int lane_y = cfg->road_start_row + lane;
        if (lane_y < 0 || lane_y >= state->height) continue;
        for (int i = 0; i < state->car_count[lane]; i++) {
            const CarState* car = &state->cars[lane][i];
            if (!car->active) continue;
            for (int cx = 0; cx < car->length; cx++) {
                int x = (int)(car->x) + cx;
                if (x >= 0 && x < state->width) {
                    dm->car_danger[lane_y][x] = 1;
                }
            }
        }
    }

    for (int lane = 0; lane < MAX_LANES; lane++) {
        int lane_y = cfg->river_start_row + lane;
        if (lane_y < 0 || lane_y >= state->height) continue;
        if (map_get_tile(state, state->frog.x, lane_y) == TILE_RIVER) {
            int on_log = 0;
            for (int i = 0; i < state->log_count[lane]; i++) {
                const LogState* log = &state->logs[lane][i];
                if (!log->active) continue;
                if ((float)state->frog.x + 0.5f >= log->x &&
                    (float)state->frog.x + 0.5f <= log->x + log->length) {
                    on_log = 1;
                    break;
                }
            }
            dm->water_danger[lane_y] = !on_log;
        }
    }
}

int danger_is_safe(const DangerMap* dm, int x, int y, const FroggerState* state) {
    if (!map_in_bounds(state, x, y)) return 0;
    TileType tile = map_get_tile(state, x, y);
    if (tile == TILE_ROAD) {
        if (x >= 0 && x < state->width && dm->car_danger[y][x]) return 0;
    }
    if (tile == TILE_RIVER) {
        if (dm->water_danger[y]) return 0;
    }
    return 1;
}
