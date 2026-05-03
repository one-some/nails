#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "UI/UI.h"

Font ui_font;

inline int32_t* stack_vec_axis(Vec2* vec, Axis axis) {
    return axis == AXIS_X ? &vec->x : &vec->y;
}

inline SizeConstraint* stack_size_axis(Size* size, Axis axis) {
    return axis == AXIS_X ? &size->x : &size->y;
}

void ui_stack_layout(UIStack* stack) {
    Axis a_on = stack->axis;
    Axis a_off = stack->axis == AXIS_Y ? AXIS_X : AXIS_Y;

    // IM SO Lazy! I ADMIT IT!
    UIComponent* component = (UIComponent*)stack;

    int32_t play_room = *stack_vec_axis(&component->render_size, a_on);
    int32_t size_off = *stack_vec_axis(&component->render_size, a_off);

    int32_t position = *stack_vec_axis(&component->render_position, a_on);
    int32_t global_pos_off = *stack_vec_axis(&component->render_position, a_off);

    int32_t total_flex_pie = 0;

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        SizeConstraint* size = stack_size_axis(&child->size, a_on);

        play_room -= stack->gap;

        switch (size->type) {
            case SIZE_ABSOLUTE:
                play_room -= size->value;
                break;
            case SIZE_FLEX_GROW:
                total_flex_pie += size->value;
                break;
        }
    }

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];

        int32_t* pos_on = stack_vec_axis(&child->render_position, a_on);
        int32_t* pos_off = stack_vec_axis(&child->render_position, a_off);
        int32_t* size_on = stack_vec_axis(&child->render_size, a_on);
        int32_t* size_off = stack_vec_axis(&child->render_size, a_off);

        SizeConstraint* size_constraint_on = stack_size_axis(&child->size, a_on);

        *pos_off = global_pos_off;
        *pos_on = position;

        *size_off = *stack_vec_axis(&component->render_size, a_off);

        if (size_constraint_on->type == SIZE_FLEX_GROW) {
            *size_on = (play_room / total_flex_pie) * size_constraint_on->value;
        }

        position += *size_on + stack->gap;
    }
}

void ui_grid_layout(UIGrid* grid) {
    UIComponent* component = (UIComponent*)grid;

    int32_t item_width = (component->render_size.x / grid->columns) - grid->gap_px;
    int32_t item_height = item_width * 1.5;

    // Keep it centered even if perfect sizing is impossible
    int32_t offset = component->render_size.x - ((item_width + grid->gap_px) * grid->columns);
    
    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        child->render_size = (Vec2) { item_width, item_height };

        Vec2 pos = {
            offset + ((i % grid->columns) * (item_width + grid->gap_px)),
            (i / grid->columns) * (item_height + grid->gap_px)
        };

        child->render_position = vec2_add(component->render_position, pos);
    }
}

void ui_layout(UIComponent* component, UIComponent* parent) {
    assert(component);

    if (component->size.x.type == SIZE_ABSOLUTE)
        component->render_size.x = component->size.x.value;
    if (component->size.y.type == SIZE_ABSOLUTE)
        component->render_size.y = component->size.y.value;

    if (component->size.x.type == SIZE_OTHER_AXIS)
        component->render_size.x = component->render_size.y;
    if (component->size.y.type == SIZE_OTHER_AXIS)
        component->render_size.y = component->render_size.x;

    if (parent && parent->type != UI_STACK && parent->type != UI_GRID) {
        Vec2 inner_size = parent->render_size;
        if (parent->type == UI_FRAME) {
            inner_size.x -= ((UIFrame*)parent)->padding_px * 2;
            inner_size.y -= ((UIFrame*)parent)->padding_px * 2;
        }

        if (component->size.x.type == SIZE_FLEX_GROW)
            component->render_size.x = inner_size.x;
        if (component->size.y.type == SIZE_FLEX_GROW)
            component->render_size.y = inner_size.y;
    }

    if (component->type == UI_STACK) {
        ui_stack_layout((UIStack*)component);
    } else if (component->type == UI_GRID) {
        ui_grid_layout((UIGrid*)component);
    } else if (component->type == UI_FRAME) {
        UIFrame* frame = (UIFrame*)component;
        component->render_size.x -= frame->margin_px * 2;
        component->render_size.y -= frame->margin_px * 2;

        component->render_position.x += frame->margin_px;
        component->render_position.y += frame->margin_px;

        for (int i=0; i<component->children.length; i++) {
            UIComponent* child = component->children.data[i];
            child->render_position.x = component->render_position.x + frame->padding_px;
            child->render_position.y = component->render_position.y + frame->padding_px;
        }
    } else {
        for (int i=0; i<component->children.length; i++) {
            UIComponent* child = component->children.data[i];
            child->render_position = component->render_position;
        }
    }

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        ui_layout(child, component);
    }
}

void ui_render(UIComponent* component, UIComponent* parent) {
    assert(component);

    // FIXME: NEED TO IMPLEMENT A STACK BECAUSE RAYLIB DOESNT SUPPORT NESTED SCISSOR CALLS
    BeginScissorMode(
        component->render_position.x,
        component->render_position.y,
        component->render_size.x,
        component->render_size.y
    );

    switch (component->type) {
        case UI_IMAGE:
            if (((UIImage*)component)->texture.loaded) {
                DrawTexturePro(
                    ((UIImage*)component)->texture.texture,
                    (Rectangle) {
                        0,
                        0,
                        (float)(((UIImage*)component)->texture.texture.width),
                        (float)(((UIImage*)component)->texture.texture.height),
                    },
                    (Rectangle) {
                        component->render_position.x,
                        component->render_position.y,
                        component->render_size.x,
                        component->render_size.y,
                    },
                    (Vector2) { 0, 0 },
                    0.0f,
                    WHITE
                );
            } else {
                DrawRectangle(
                    component->render_position.x,
                    component->render_position.y,
                    component->render_size.x,
                    component->render_size.y,
                    MAGENTA
                );
            }
            break;
        case UI_VIEWPORT:
            DrawTexturePro(
                ((UIViewport*)component)->render_texture.texture,
                (Rectangle) {
                    0,
                    0,
                    (float)(((UIViewport*)component)->render_texture.texture.width),
                    (float)(((UIViewport*)component)->render_texture.texture.height),
                },
                (Rectangle) {
                    component->render_position.x,
                    component->render_position.y,
                    component->render_size.x,
                    component->render_size.y,
                },
                (Vector2) { 0, 0 },
                0.0f,
                WHITE
            );
            break;
        case UI_LABEL:
            DrawTextEx(
                ui_font,
                ((UILabel*)component)->text,
                (Vector2) {
                    component->render_position.x,
                    component->render_position.y,
                },
                ((UILabel*)component)->font_size,
                2,
                WHITE
            );
            break;
        case UI_FRAME:
            DrawRectangle(
                component->render_position.x,
                component->render_position.y,
                component->render_size.x,
                component->render_size.y,
                ((UIFrame*)component)->color
            );
            break;
        default:
            break;
    }

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        ui_render(child, component);
    }

    EndScissorMode();
}


void ui_propagate_event(UIComponent* component, Event* event) {
    assert(component);
    assert(event);

    switch (event->type) {
        case EVENT_TICK:
            if (component->event_handlers.on_tick)
                component->event_handlers.on_tick((TickEvent*)event);
            break;
        case EVENT_MOUSE_DOWN:
            if (component->event_handlers.on_mouse_down)
                component->event_handlers.on_mouse_down((MouseButtonEvent*)event);
            break;
        case EVENT_MOUSE_UP:
            if (component->event_handlers.on_mouse_up)
                component->event_handlers.on_mouse_up((MouseButtonEvent*)event);
            break;
        case EVENT_MOUSE_MOVE:
            if (component->event_handlers.on_mouse_move)
                component->event_handlers.on_mouse_move((MouseMoveEvent*)event);
            break;
        case EVENT_KEY_DOWN:
            if (component->event_handlers.on_key_down)
                component->event_handlers.on_key_down((KeyEvent*)event);
            break;
        case EVENT_KEY_UP:
            if (component->event_handlers.on_key_up)
                component->event_handlers.on_key_up((KeyEvent*)event);
            break;
    }

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        ui_propagate_event(child, event);
   }
}
