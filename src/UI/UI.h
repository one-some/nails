#pragma once

#include "raylib.h"

#include "UI/Vec.h"
#include "UI/Event.h"
#include "UI/SizeConstraint.h"

typedef enum {
    UI_CONTAINER,
    UI_VSTACK,
    UI_HSTACK,
    UI_COLOR_RECT,
    UI_VIEWPORT
} UIType;

typedef struct {
    void (*on_mouse_down)(MouseButtonEvent* event);
    void (*on_mouse_up)(MouseButtonEvent* event);
    void (*on_mouse_move)(MouseMoveEvent* event);
} EventHandlers;

typedef struct {
    UIType type;
    Size size;
    Vec2 render_size;
    Vec2 render_position;
    Vec children;

    EventHandlers event_handlers;
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
void ui_stack_layout(UIComponent* component, Vec2 global_position);

int32_t* stack_vec_axis(Vec2* vec, AxisSelection axis);
SizeConstraint* stack_size_axis(Size* size, AxisSelection axis);

void ui_propagate_event(UIComponent* component, Event* event);
