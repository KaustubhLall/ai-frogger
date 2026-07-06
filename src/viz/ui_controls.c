#include "viz/ui_controls.h"

void ui_button_init(UIButton* btn, float x, float y, float w, float h, const char* label) {
    btn->rect = (Rectangle){x, y, w, h};
    btn->label = label;
    btn->hovered = 0;
    btn->pressed = 0;
}

int ui_button_update(UIButton* btn) {
    Vector2 mouse = GetMousePosition();
    btn->hovered = CheckCollisionPointRec(mouse, btn->rect);
    btn->pressed = btn->hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    return btn->pressed;
}

void ui_button_draw(const UIButton* btn) {
    Color bg = btn->hovered ? (Color){60, 60, 80, 255} : (Color){30, 30, 40, 255};
    DrawRectangleRec(btn->rect, bg);
    DrawRectangleLinesEx(btn->rect, 1, (Color){100, 100, 120, 255});
    int tw = MeasureText(btn->label, 12);
    DrawText(btn->label, (int)(btn->rect.x + (btn->rect.width - tw) / 2),
             (int)(btn->rect.y + btn->rect.height / 2 - 6), 12, WHITE);
}

void ui_slider_init(UISlider* sl, float x, float y, float w, const char* label, float min_v, float max_v, float value) {
    sl->rect = (Rectangle){x, y, w, 20};
    sl->label = label;
    sl->min_v = min_v;
    sl->max_v = max_v;
    sl->value = value;
    sl->dragging = 0;
}

void ui_slider_update(UISlider* sl) {
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, sl->rect)) {
        sl->dragging = 1;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) sl->dragging = 0;
    if (sl->dragging) {
        float t = (mouse.x - sl->rect.x) / sl->rect.width;
        if (t < 0) t = 0; if (t > 1) t = 1;
        sl->value = sl->min_v + t * (sl->max_v - sl->min_v);
    }
}

void ui_slider_draw(const UISlider* sl) {
    DrawRectangleRec(sl->rect, (Color){30, 30, 40, 255});
    DrawRectangleLinesEx(sl->rect, 1, (Color){100, 100, 120, 255});
    float t = (sl->value - sl->min_v) / (sl->max_v - sl->min_v);
    DrawRectangle((int)sl->rect.x, (int)sl->rect.y, (int)(sl->rect.width * t), (int)sl->rect.height,
                  (Color){60, 120, 200, 255});
    DrawText(sl->label, (int)sl->rect.x, (int)sl->rect.y - 14, 10, (Color){180, 180, 200, 255});
}
