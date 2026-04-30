#pragma once

#include "raylib.h"

#include "UI/Vec.h"
#include "UI/Event.h"
#include "UI/SizeConstraint.h"

typedef enum {
    UI_CONTAINER,
    UI_STACK,
    UI_COLOR_RECT,
    UI_VIEWPORT,
    UI_LABEL,
    UI_FRAME,
} UIType;

typedef struct {
    void (*on_tick)(TickEvent* event);

    void (*on_mouse_down)(MouseButtonEvent* event);
    void (*on_mouse_up)(MouseButtonEvent* event);

    void (*on_mouse_move)(MouseMoveEvent* event);

    void (*on_key_up)(KeyEvent* event);
    void (*on_key_down)(KeyEvent* event);
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
    Axis axis;
} UIStack;

typedef struct {
    UIComponent base;
    Color color;
} UIColorRect;

typedef struct {
    UIComponent base;
    RenderTexture2D render_texture;
} UIViewport;

typedef struct {
    UIComponent base;
    const char* text;
} UILabel;

typedef struct {
    UIComponent base;
    Color color;
    int32_t margin_px;
} UIFrame;


typedef enum {
    AXIS_X,
    AXIS_Y
} Axis;

extern Font ui_font;

void ui_layout(UIComponent* component, UIComponent* parent, Vec2 global_position);
void ui_render(UIComponent* component, UIComponent* parent);
void ui_stack_layout(UIComponent* component, Vec2 global_position);

int32_t* stack_vec_axis(Vec2* vec, Axis axis);
SizeConstraint* stack_size_axis(Size* size, Axis axis);

void ui_propagate_event(UIComponent* component, Event* event);
