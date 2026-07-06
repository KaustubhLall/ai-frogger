#include "viz/charts.h"

void charts_draw_line(const float* data, int count, int ox, int oy, int w, int h,
                      float min_v, float max_v, Color color) {
    if (count < 2) return;
    float range = max_v - min_v;
    if (range < 0.001f) { max_v = min_v + 1.0f; range = 1.0f; }
    for (int i = 1; i < count; i++) {
        int x0 = ox + (i - 1) * w / (count - 1);
        int x1 = ox + i * w / (count - 1);
        int y0 = oy + h - (int)((data[i-1] - min_v) / range * h);
        int y1 = oy + h - (int)((data[i] - min_v) / range * h);
        DrawLine(x0, y0, x1, y1, color);
    }
}
