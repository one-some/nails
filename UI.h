#pragma once

#include "raylib.h"

#include "Vec.h"
#include "SizeConstraint.h"

typedef enum {
    UI_CONTAINER,
    UI_VSTACK,
    UI_HSTACK,
    UI_COLOR_RECT,
    UI_VIEWPORT
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
} UIHStack;

typedef struct {
    UIComponent base;
    Color color;
} UIColorRect;

typedef struct {
    UIComponent base;
    RenderTexture2D render_texture;
} UIViewport;


typedef enum {
    AXIS_X,
    AXIS_Y
} AxisSelection;

void ui_layout(UIComponent* component, UIComponent* parent, Vec2 global_position);
void ui_render(UIComponent* component, UIComponent* parent);

int32_t* stack_vec_axis(Vec2* vec, AxisSelection axis);
SizeConstraint* stack_size_axis(Size* size, AxisSelection axis);
