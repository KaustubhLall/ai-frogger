#ifndef FROGGER_UI_CONTROLS_H
#define FROGGER_UI_CONTROLS_H

#include "raylib.h"

typedef struct {
    Rectangle rect;
    const char* label;
    int hovered;
    int pressed;
} UIButton;

typedef struct {
    Rectangle rect;
    const char* label;
    float value;
    float min_v;
    float max_v;
    int dragging;
} UISlider;

void ui_button_init(UIButton* btn, float x, float y, float w, float h, const char* label);
int ui_button_update(UIButton* btn);
void ui_button_draw(const UIButton* btn);

void ui_slider_init(UISlider* sl, float x, float y, float w, const char* label, float min_v, float max_v, float value);
void ui_slider_update(UISlider* sl);
void ui_slider_draw(const UISlider* sl);

#endif /* FROGGER_UI_CONTROLS_H */
