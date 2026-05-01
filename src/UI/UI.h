#pragma once

#include "raylib.h"

#include "UI/Vec.h"
#include "UI/Event.h"
#include "UI/SizeConstraint.h"

typedef enum {
    AXIS_X,
    AXIS_Y
} Axis;

typedef enum {
    UI_CONTAINER,
    UI_STACK,
    UI_VIEWPORT,
    UI_LABEL,
    UI_FRAME,
    UI_GRID,
    UI_IMAGE,
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
    Vec children;

    Vec2 render_size;
    Vec2 render_position;

    EventHandlers event_handlers;
} UIComponent;

typedef struct {
    UIComponent base;
    Axis axis;
    int32_t gap;
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
    Texture2D texture;
} UIImage;

typedef struct {
    UIComponent base;
    const char* text;
    int32_t font_size;
} UILabel;

typedef struct {
    UIComponent base;
    Color color;
    int32_t margin_px;
    int32_t padding_px;
} UIFrame;

typedef struct {
    UIComponent base;
    int32_t columns;
    int32_t gap_px;
} UIGrid;


extern Font ui_font;

void ui_layout(UIComponent* component, UIComponent* parent);
void ui_render(UIComponent* component, UIComponent* parent);
void ui_stack_layout(UIStack* component);
void ui_grid_layout(UIGrid* grid);

int32_t* stack_vec_axis(Vec2* vec, Axis axis);
SizeConstraint* stack_size_axis(Size* size, Axis axis);

void ui_propagate_event(UIComponent* component, Event* event);
