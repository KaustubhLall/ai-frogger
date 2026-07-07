#include "viz/renderer.h"
#include "env/frogger_map.h"
#include "env/frogger_danger.h"
#include <stdio.h>
#include <string.h>

static Color tile_colors[] = {
    {60, 160, 60, 255},    /* GRASS */
    {80, 80, 80, 255},     /* ROAD */
    {40, 100, 180, 255},   /* RIVER */
    {255, 215, 0, 255},    /* GOAL */
    {120, 120, 100, 255}   /* MEDIAN */
};

static Color panel_bg = {18, 18, 28, 255};
static Color panel_border = {80, 80, 110, 255};
static Color panel_title_color = {180, 180, 220, 255};
static Color text_primary = {220, 220, 235, 255};
static Color text_secondary = {160, 160, 180, 255};

void renderer_init(int screen_w, int screen_h) {
    (void)screen_w;
    (void)screen_h;
}

static void draw_panel(int ox, int oy, int w, int h, const char* title) {
    DrawRectangle(ox, oy, w, h, panel_bg);
    DrawRectangleLines(ox, oy, w, h, panel_border);
    if (title) {
        DrawText(title, ox + 8, oy + 4, 13, panel_title_color);
    }
}

void renderer_draw_arena(const DebugSnapshot* snap, int ox, int oy, int tile_size) {
    const FroggerState* s = &snap->state;

    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x++) {
            TileType t = map_get_tile(s, x, y);
            Color c = tile_colors[(int)t];
            DrawRectangle(ox + x * tile_size, oy + y * tile_size, tile_size, tile_size, c);
            DrawRectangleLines(ox + x * tile_size, oy + y * tile_size, tile_size, tile_size,
                               (Color){40, 40, 40, 60});
        }
    }

    /* Draw logs */
    for (int lane = 0; lane < 6; lane++) {
        for (int i = 0; i < s->log_count[lane]; i++) {
            const LogState* log = &s->logs[lane][i];
            if (!log->active) continue;
            int lx = (int)log->x;
            for (int cx = 0; cx < log->length; cx++) {
                int px = lx + cx;
                if (px < 0 || px >= s->width) continue;
                DrawRectangle(ox + px * tile_size + 2, oy + log->y * tile_size + 4,
                              tile_size - 4, tile_size - 8, (Color){139, 90, 43, 255});
            }
        }
    }

    /* Draw cars */
    for (int lane = 0; lane < 6; lane++) {
        for (int i = 0; i < s->car_count[lane]; i++) {
            const CarState* car = &s->cars[lane][i];
            if (!car->active) continue;
            int cx = (int)car->x;
            for (int j = 0; j < car->length; j++) {
                int px = cx + j;
                if (px < 0 || px >= s->width) continue;
                Color car_color = (car->speed > 0) ? (Color){220, 60, 60, 255} : (Color){60, 60, 220, 255};
                DrawRectangle(ox + px * tile_size + 2, oy + car->y * tile_size + 4,
                              tile_size - 4, tile_size - 8, car_color);
            }
        }
    }

    /* Draw goal lily pads */
    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x++) {
            if (map_get_tile(s, x, y) == TILE_GOAL) {
                DrawCircle(ox + x * tile_size + tile_size / 2,
                           oy + y * tile_size + tile_size / 2,
                           tile_size / 2 - 6, (Color){180, 255, 100, 200});
            }
        }
    }

    /* Draw frog */
    if (s->frog.alive) {
        int fx = ox + s->frog.x * tile_size;
        int fy = oy + s->frog.y * tile_size;
        DrawCircle(fx + tile_size / 2, fy + tile_size / 2, tile_size / 2 - 4, (Color){50, 210, 50, 255});
        DrawCircleLines(fx + tile_size / 2, fy + tile_size / 2, tile_size / 2 - 4, (Color){20, 80, 20, 255});
    }

    /* Draw risk overlay: highlight tiles adjacent to frog that have danger */
    if (s->frog.alive && tile_size >= 16) {
        int fx = s->frog.x;
        int fy = s->frog.y;
        int dirs[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        for (int d = 0; d < 4; d++) {
            int nx = fx + dirs[d][0];
            int ny = fy + dirs[d][1];
            if (nx < 0 || nx >= s->width || ny < 0 || ny >= s->height) continue;
            TileType t = map_get_tile(s, nx, ny);
            if (t == TILE_ROAD) {
                DrawRectangleLines(ox + nx * tile_size, oy + ny * tile_size,
                                   tile_size, tile_size, (Color){255, 100, 100, 180});
            } else if (t == TILE_RIVER) {
                DrawRectangleLines(ox + nx * tile_size, oy + ny * tile_size,
                                   tile_size, tile_size, (Color){100, 150, 255, 180});
            }
        }
    }
}

void renderer_draw_local_obs(const Observation* obs, int ox, int oy, int w, int tile_size) {
    DrawRectangle(ox, oy, w, LOCAL_OBS_SIZE * tile_size + 20, (Color){20, 20, 30, 255});
    DrawRectangleLines(ox, oy, w, LOCAL_OBS_SIZE * tile_size + 20, (Color){100, 100, 120, 255});
    DrawText("Local Observation", ox + 4, oy + 2, 12, WHITE);

    int grid_oy = oy + 18;
    for (int y = 0; y < LOCAL_OBS_SIZE; y++) {
        for (int x = 0; x < LOCAL_OBS_SIZE; x++) {
            int t = obs->local_tiles[y][x];
            Color c = (t < 0) ? (Color){15, 15, 15, 255} : tile_colors[t % 5];
            DrawRectangle(ox + 4 + x * tile_size, grid_oy + y * tile_size, tile_size, tile_size, c);
            if (obs->local_cars[y][x] != 0.0f) {
                DrawRectangle(ox + 4 + x * tile_size, grid_oy + y * tile_size, tile_size, tile_size,
                              (Color){200, 50, 50, 200});
            }
            if (obs->local_logs[y][x] != 0.0f) {
                DrawRectangle(ox + 4 + x * tile_size, grid_oy + y * tile_size, tile_size, tile_size,
                              (Color){139, 69, 19, 200});
            }
        }
    }
    /* Center marker */
    int cx = ox + 4 + LOCAL_OBS_HALF * tile_size;
    int cy = grid_oy + LOCAL_OBS_HALF * tile_size;
    DrawCircle(cx + tile_size / 2, cy + tile_size / 2, 4, (Color){50, 255, 50, 255});
}

void renderer_draw_danger_map(const DebugSnapshot* snap, int ox, int oy, int w, int h) {
    draw_panel(ox, oy, w, h, "Danger Map");
    const DangerMap* dm = &snap->danger;
    const FroggerState* s = &snap->state;

    int map_w = s->width * 12;
    int map_h = s->height * 12;
    int grid_ox = ox + (w - map_w) / 2;
    int grid_oy = oy + 22;

    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x++) {
            int danger = dm->car_danger[y][x];
            if (danger == 0 && dm->water_danger[y]) {
                TileType t = map_get_tile(s, x, y);
                if (t == TILE_RIVER) danger = 2;
            }
            Color c;
            if (danger == 0) {
                c = (Color){30, 30, 40, 255};
            } else if (danger <= 1) {
                c = (Color){60, 120, 60, 255};
            } else if (danger <= 2) {
                c = (Color){120, 120, 40, 255};
            } else if (danger <= 3) {
                c = (Color){180, 80, 40, 255};
            } else {
                c = (Color){220, 40, 40, 255};
            }
            DrawRectangle(grid_ox + x * 12, grid_oy + y * 12, 11, 11, c);
        }
    }

    /* Draw frog position on danger map */
    if (s->frog.alive) {
        DrawCircle(grid_ox + s->frog.x * 12 + 6, grid_oy + s->frog.y * 12 + 6, 4,
                   (Color){50, 255, 50, 255});
    }
}

void renderer_draw_reward_graph(const float* rewards, int count, int ox, int oy, int w, int h, float y_max) {
    draw_panel(ox, oy, w, h, "Reward History");
    if (count < 2) return;
    float min_v = 0.0f, max_v = y_max;
    for (int i = 0; i < count; i++) {
        if (rewards[i] < min_v) min_v = rewards[i];
        if (rewards[i] > max_v) max_v = rewards[i];
    }
    if (max_v - min_v < 0.001f) max_v = min_v + 1.0f;
    for (int i = 1; i < count; i++) {
        int x0 = ox + 4 + (i - 1) * (w - 8) / (count - 1);
        int x1 = ox + 4 + i * (w - 8) / (count - 1);
        int y0 = oy + h - 4 - (int)((rewards[i-1] - min_v) / (max_v - min_v) * (h - 20));
        int y1 = oy + h - 4 - (int)((rewards[i] - min_v) / (max_v - min_v) * (h - 20));
        DrawLine(x0, y0, x1, y1, (Color){100, 255, 100, 255});
    }
}

void renderer_draw_action_dist(const int* counts, int total, int ox, int oy, int w, int h) {
    draw_panel(ox, oy, w, h, "Action Distribution");
    const char* names[] = {"UP", "DOWN", "LEFT", "RIGHT", "WAIT"};
    Color colors[] = {
        {100, 255, 100, 255}, {255, 100, 100, 255}, {255, 255, 100, 255},
        {100, 100, 255, 255}, {200, 200, 200, 255}
    };
    int bar_h = (h - 24) / 5;
    for (int a = 0; a < 5; a++) {
        int bw = total > 0 ? (int)((float)counts[a] / total * (w - 80)) : 0;
        DrawRectangle(ox + 40, oy + 20 + a * bar_h, bw, bar_h - 2, colors[a]);
        DrawText(names[a], ox + 4, oy + 22 + a * bar_h, 10, text_primary);
    }
}

void renderer_draw_status_panel(const DebugSnapshot* snap, int ox, int oy, int w, int h) {
    draw_panel(ox, oy, w, h, "Status");
    char buf[128];
    int y = oy + 20;
    int line_h = 16;

    snprintf(buf, sizeof(buf), "Step: %d", snap->state.step);
    DrawText(buf, ox + 8, y, 12, text_primary); y += line_h;

    snprintf(buf, sizeof(buf), "Frog: (%d,%d)", snap->state.frog.x, snap->state.frog.y);
    DrawText(buf, ox + 8, y, 12, text_primary); y += line_h;

    snprintf(buf, sizeof(buf), "Alive: %s", snap->state.frog.alive ? "YES" : "NO");
    DrawText(buf, ox + 8, y, 12, snap->state.frog.alive ? (Color){100, 255, 100, 255} : (Color){255, 80, 80, 255});
    y += line_h;

    snprintf(buf, sizeof(buf), "Goals: %d", snap->state.goals_reached);
    DrawText(buf, ox + 8, y, 12, text_primary); y += line_h;

    snprintf(buf, sizeof(buf), "Deaths: %d", snap->state.total_deaths);
    DrawText(buf, ox + 8, y, 12, text_primary); y += line_h;

    snprintf(buf, sizeof(buf), "Reward: %.2f", snap->cumulative_reward);
    DrawText(buf, ox + 8, y, 12, (Color){100, 200, 255, 255}); y += line_h;

    const char* action_names[] = {"UP", "DOWN", "LEFT", "RIGHT", "WAIT"};
    if (snap->last_action >= 0 && snap->last_action < ACTION_COUNT) {
        snprintf(buf, sizeof(buf), "Last: %s", action_names[snap->last_action]);
        DrawText(buf, ox + 8, y, 12, (Color){255, 200, 100, 255});
    }
}

void renderer_draw_event_log(const char** events, int count, int ox, int oy, int w, int h) {
    draw_panel(ox, oy, w, h, "Event Log");
    int max_lines = (h - 20) / 14;
    int start = count > max_lines ? count - max_lines : 0;
    for (int i = start; i < count; i++) {
        DrawText(events[i], ox + 8, oy + 20 + (i - start) * 14, 10, text_secondary);
    }
}

void renderer_draw_decision_trace(const char* text, int ox, int oy, int w, int h) {
    draw_panel(ox, oy, w, h, "Agent Brain");
    if (text && text[0]) {
        DrawText(text, ox + 8, oy + 20, 11, (Color){200, 200, 220, 255});
    } else {
        DrawText("(waiting...)", ox + 8, oy + 20, 11, text_secondary);
    }
}

void renderer_draw_controls(int ox, int oy, int w, int h, int paused, int speed) {
    draw_panel(ox, oy, w, h, "Controls");
    DrawText("[SPACE] Pause/Resume", ox + 8, oy + 20, 11, text_secondary);
    DrawText("[R] Reset", ox + 8, oy + 34, 11, text_secondary);
    DrawText("[+/-] Speed", ox + 8, oy + 48, 11, text_secondary);
    DrawText("[TAB] Switch Agent", ox + 8, oy + 62, 11, text_secondary);
    DrawText("[1/2/3] View Mode", ox + 8, oy + 76, 11, text_secondary);
    char buf[64];
    snprintf(buf, sizeof(buf), "Speed: %dx  %s", speed, paused ? "PAUSED" : "RUNNING");
    DrawText(buf, ox + 8, oy + 94, 11, paused ? (Color){255, 200, 100, 255} : (Color){100, 255, 100, 255});
}
