#ifndef FROGGER_RENDERER_H
#define FROGGER_RENDERER_H

#include "raylib.h"
#include "env/env.h"
#include "env/frogger_observation.h"

#define TILE_SIZE 32

void renderer_init(int screen_w, int screen_h);
void renderer_draw_arena(const DebugSnapshot* snap, int ox, int oy, int tile_size);
void renderer_draw_local_obs(const Observation* obs, int ox, int oy, int w, int tile_size);
void renderer_draw_danger_map(const DebugSnapshot* snap, int ox, int oy, int w, int h);
void renderer_draw_reward_graph(const float* rewards, int count, int ox, int oy, int w, int h, float y_max);
void renderer_draw_action_dist(const int* counts, int total, int ox, int oy, int w, int h);
void renderer_draw_status_panel(const DebugSnapshot* snap, int ox, int oy, int w, int h);
void renderer_draw_event_log(const char** events, int count, int ox, int oy, int w, int h);
void renderer_draw_decision_trace(const char* text, int ox, int oy, int w, int h);
void renderer_draw_controls(int ox, int oy, int w, int h, int paused, int speed);

#endif /* FROGGER_RENDERER_H */
