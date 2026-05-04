#pragma once

#include "raylib.h"

#include "UI/Axis.h"
#include "UI/Vec2.h"

typedef enum {
    EVENT_TICK,
    EVENT_MOUSE_DOWN,
    EVENT_MOUSE_UP,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_WHEEL,
    EVENT_KEY_DOWN,
    EVENT_KEY_UP,
} EventType;

typedef struct UIComponent UIComponent;

typedef struct {
    EventType type;
    UIComponent* target;
} Event;

typedef struct {
    Event base;
    MouseButton button;
} MouseButtonEvent;

typedef struct {
    Event base;
    Vec2 position;
    Vec2 delta;
} MouseMoveEvent;

typedef struct {
    Event base;
    KeyboardKey key;
} KeyEvent;

typedef struct {
    Event base;
    float delta;
    Axis axis;
} MouseWheelEvent;
