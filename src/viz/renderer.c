#include "viz/renderer.h"
#include "env/frogger_map.h"
#include <stdio.h>
#include <string.h>

static Color tile_colors[] = {
    {60, 160, 60, 255},    /* GRASS */
    {80, 80, 80, 255},     /* ROAD */
    {40, 100, 180, 255},   /* RIVER */
    {255, 215, 0, 255},    /* GOAL */
    {120, 120, 100, 255}   /* MEDIAN */
};

void renderer_init(int screen_w, int screen_h) {
    (void)screen_w;
    (void)screen_h;
}

void renderer_draw_arena(const DebugSnapshot* snap, int ox, int oy, int tile_size) {
    const FroggerState* s = &snap->state;

    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x++) {
            TileType t = map_get_tile(s, x, y);
            Color c = tile_colors[(int)t];
            DrawRectangle(ox + x * tile_size, oy + y * tile_size, tile_size, tile_size, c);
            DrawRectangleLines(ox + x * tile_size, oy + y * tile_size, tile_size, tile_size,
                               (Color){40, 40, 40, 80});
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
                              tile_size - 4, tile_size - 8, (Color){139, 69, 19, 255});
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
                Color car_color = (car->speed > 0) ? (Color){220, 50, 50, 255} : (Color){50, 50, 220, 255};
                DrawRectangle(ox + px * tile_size + 2, oy + car->y * tile_size + 4,
                              tile_size - 4, tile_size - 8, car_color);
            }
        }
    }

    /* Draw frog */
    if (s->frog.alive) {
        int fx = ox + s->frog.x * tile_size;
        int fy = oy + s->frog.y * tile_size;
        DrawCircle(fx + tile_size / 2, fy + tile_size / 2, tile_size / 2 - 4, (Color){50, 200, 50, 255});
        DrawCircleLines(fx + tile_size / 2, fy + tile_size / 2, tile_size / 2 - 4, (Color){20, 80, 20, 255});
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
    DrawRectangle(ox, oy, w, h, (Color){20, 20, 30, 255});
    DrawRectangleLines(ox, oy, w, h, (Color){100, 100, 120, 255});
    DrawText("Danger Map", ox + 4, oy + 2, 12, WHITE);
    (void)snap;
}

void renderer_draw_reward_graph(const float* rewards, int count, int ox, int oy, int w, int h, float y_max) {
    DrawRectangle(ox, oy, w, h, (Color){20, 20, 30, 255});
    DrawRectangleLines(ox, oy, w, h, (Color){100, 100, 120, 255});
    DrawText("Reward History", ox + 4, oy + 2, 12, WHITE);
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
    DrawRectangle(ox, oy, w, h, (Color){20, 20, 30, 255});
    DrawRectangleLines(ox, oy, w, h, (Color){100, 100, 120, 255});
    DrawText("Action Distribution", ox + 4, oy + 2, 12, WHITE);
    const char* names[] = {"UP", "DOWN", "LEFT", "RIGHT", "WAIT"};
    Color colors[] = {
        {100, 255, 100, 255}, {255, 100, 100, 255}, {255, 255, 100, 255},
        {100, 100, 255, 255}, {200, 200, 200, 255}
    };
    int bar_h = (h - 24) / 5;
    for (int a = 0; a < 5; a++) {
        int bw = total > 0 ? (int)((float)counts[a] / total * (w - 80)) : 0;
        DrawRectangle(ox + 40, oy + 20 + a * bar_h, bw, bar_h - 2, colors[a]);
        DrawText(names[a], ox + 4, oy + 22 + a * bar_h, 10, WHITE);
    }
}

void renderer_draw_status_panel(const DebugSnapshot* snap, int ox, int oy, int w, int h) {
    DrawRectangle(ox, oy, w, h, (Color){20, 20, 30, 255});
    DrawRectangleLines(ox, oy, w, h, (Color){100, 100, 120, 255});
    DrawText("Status", ox + 4, oy + 2, 12, WHITE);
    char buf[128];
    snprintf(buf, sizeof(buf), "Step: %d", snap->state.step);
    DrawText(buf, ox + 8, oy + 20, 12, WHITE);
    snprintf(buf, sizeof(buf), "Frog: (%d,%d)", snap->state.frog.x, snap->state.frog.y);
    DrawText(buf, ox + 8, oy + 36, 12, WHITE);
    snprintf(buf, sizeof(buf), "Alive: %s", snap->state.frog.alive ? "YES" : "NO");
    DrawText(buf, ox + 8, oy + 52, 12, snap->state.frog.alive ? (Color){100, 255, 100, 255} : (Color){255, 80, 80, 255});
    snprintf(buf, sizeof(buf), "Goals: %d", snap->state.goals_reached);
    DrawText(buf, ox + 8, oy + 68, 12, WHITE);
    snprintf(buf, sizeof(buf), "Deaths: %d", snap->state.total_deaths);
    DrawText(buf, ox + 8, oy + 84, 12, WHITE);
    snprintf(buf, sizeof(buf), "Reward: %.2f", snap->cumulative_reward);
    DrawText(buf, ox + 8, oy + 100, 12, (Color){100, 200, 255, 255});
}

void renderer_draw_event_log(const char** events, int count, int ox, int oy, int w, int h) {
    DrawRectangle(ox, oy, w, h, (Color){20, 20, 30, 255});
    DrawRectangleLines(ox, oy, w, h, (Color){100, 100, 120, 255});
    DrawText("Event Log", ox + 4, oy + 2, 12, WHITE);
    int max_lines = (h - 20) / 14;
    int start = count > max_lines ? count - max_lines : 0;
    for (int i = start; i < count; i++) {
        DrawText(events[i], ox + 8, oy + 20 + (i - start) * 14, 10, (Color){180, 180, 200, 255});
    }
}

void renderer_draw_decision_trace(const char* text, int ox, int oy, int w, int h) {
    DrawRectangle(ox, oy, w, h, (Color){20, 20, 30, 255});
    DrawRectangleLines(ox, oy, w, h, (Color){100, 100, 120, 255});
    DrawText("Decision", ox + 4, oy + 2, 12, WHITE);
    if (text && text[0]) {
        DrawText(text, ox + 8, oy + 20, 11, (Color){200, 200, 220, 255});
    }
}

void renderer_draw_controls(int ox, int oy, int w, int h, int paused, int speed) {
    DrawRectangle(ox, oy, w, h, (Color){20, 20, 30, 255});
    DrawRectangleLines(ox, oy, w, h, (Color){100, 100, 120, 255});
    DrawText("Controls", ox + 4, oy + 2, 12, WHITE);
    DrawText("[SPACE] Pause/Resume", ox + 8, oy + 20, 11, (Color){180, 180, 200, 255});
    DrawText("[R] Reset", ox + 8, oy + 34, 11, (Color){180, 180, 200, 255});
    DrawText("[+/-] Speed", ox + 8, oy + 48, 11, (Color){180, 180, 200, 255});
    DrawText("[TAB] Switch Agent", ox + 8, oy + 62, 11, (Color){180, 180, 200, 255});
    DrawText("[1/2/3] View Mode", ox + 8, oy + 76, 11, (Color){180, 180, 200, 255});
    char buf[64];
    snprintf(buf, sizeof(buf), "Speed: %dx  %s", speed, paused ? "PAUSED" : "RUNNING");
    DrawText(buf, ox + 8, oy + 94, 11, paused ? (Color){255, 200, 100, 255} : (Color){100, 255, 100, 255});
}
