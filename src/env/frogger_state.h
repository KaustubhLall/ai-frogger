#ifndef FROGGER_STATE_H
#define FROGGER_STATE_H

#include "core/config.h"

typedef enum {
    TILE_GRASS = 0,
    TILE_ROAD = 1,
    TILE_RIVER = 2,
    TILE_GOAL = 3,
    TILE_MEDIAN = 4
} TileType;

typedef struct {
    float x;
    int y;
    int length;
    float speed;
    int active;
} LogState;

typedef struct {
    float x;
    int y;
    int length;
    float speed;
    int active;
} CarState;

typedef struct {
    int x;
    int y;
    int alive;
    int on_log;
    float log_offset;
    int goals_reached;
    int deaths;
} FrogState;

typedef struct {
    int width;
    int height;
    TileType tiles[MAX_HEIGHT][MAX_WIDTH];

    LogState logs[MAX_LANES][MAX_LOGS_PER_LANE];
    int log_count[MAX_LANES];

    CarState cars[MAX_LANES][MAX_LOGS_PER_LANE];
    int car_count[MAX_LANES];

    FrogState frog;
    int step;
    int goals_reached;
    int total_deaths;
} FroggerState;

#endif /* FROGGER_STATE_H */
