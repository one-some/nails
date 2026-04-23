#pragma once

#include "raylib.h"

#include "Vec.h"
#include "SizeConstraint.h"

typedef enum {
    UI_CONTAINER,
    UI_VSTACK,
    UI_COLOR_RECT,
} UIType;

typedef struct {
    UIType type;
    Size size;
    Vec2 render_size;
    Vec2 render_position;
    Vec children;
} UIComponent;

typedef struct {
    UIComponent base;
} UIVStack;

typedef struct {
    UIComponent base;
    Color color;
} UIColorRect;

void ui_layout(UIComponent* component, UIComponent* parent, Vec2 global_position);
void ui_render(UIComponent* component, UIComponent* parent);
