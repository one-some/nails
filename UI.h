#pragma once

#include "raylib.h"

#include "Vec.h"
#include "SizeConstraint.h"

typedef enum {
    UI_CONTAINER,
    UI_COLOR_RECT,
} UIType;

typedef struct {
    UIType type;
    Size size;
    Vec2 position;
    Vec children;
} UIComponent;

typedef struct {
    UIComponent base;
    Color color;
} UIColorRect;

void ui_render(UIComponent* component, UIComponent* parent, Vec2 global_position);
