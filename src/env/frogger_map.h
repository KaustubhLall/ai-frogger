#ifndef FROGGER_MAP_H
#define FROGGER_MAP_H

#include "env/frogger_state.h"
#include "core/config.h"

void map_generate(FroggerState* state, const FroggerConfig* cfg);
int map_in_bounds(const FroggerState* state, int x, int y);
TileType map_get_tile(const FroggerState* state, int x, int y);

#endif /* FROGGER_MAP_H */
