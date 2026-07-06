#ifndef FROGGER_CHARTS_H
#define FROGGER_CHARTS_H

#include "raylib.h"

void charts_draw_line(const float* data, int count, int ox, int oy, int w, int h,
                      float min_v, float max_v, Color color);

#endif /* FROGGER_CHARTS_H */
