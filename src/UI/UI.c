#include <assert.h>

#include <stdio.h>
#include "UI/UI.h"

Font ui_font;

inline int32_t* stack_vec_axis(Vec2* vec, Axis axis) {
    return axis == AXIS_X ? &vec->x : &vec->y;
}

inline SizeConstraint* stack_size_axis(Size* size, Axis axis) {
    return axis == AXIS_X ? &size->x : &size->y;
}

void ui_stack_layout(UIStack* stack, Vec2 global_position) {
    Axis a_on = stack->axis;
    Axis a_off = stack->axis == AXIS_Y ? AXIS_X : AXIS_Y;

    // IM SO Lazy! I ADMIT IT!
    UIComponent* component = (UIComponent*)stack;

    int32_t play_room = *stack_vec_axis(&component->render_size, a_on);

    int32_t position = *stack_vec_axis(&global_position, a_on);
    int32_t global_pos_off = *stack_vec_axis(&global_position, a_off);

    int32_t total_flex_pie = 0;

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        SizeConstraint* size = stack_size_axis(&child->size, a_on);

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

        position += *size_on;
    }
}

void ui_grid_layout(UIGrid* grid, Vec2 global_position) {
    UIComponent* component = (UIComponent*)grid;
    int32_t side_length = (component->render_size.x / grid->columns) - grid->gap_px;
    
    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        child->render_size = (Vec2) { side_length, side_length };

        Vec2 pos = {
            (i % grid->columns) * (side_length + grid->gap_px),
            (i / grid->columns) * (side_length + grid->gap_px)
        };
        child->render_position = vec2_add(global_position, pos);
    }
}

void ui_layout(
    UIComponent* component,
    UIComponent* parent,
    Vec2 global_position
) {
    assert(component);

    for (int i=0; i<component->children.length; i++) {
        // Stupid
        UIComponent* child = component->children.data[i];
        child->render_position = (Vec2) { 0, 0 };
    }

    if (component->size.x.type == SIZE_ABSOLUTE)
        component->render_size.x = component->size.x.value;
    if (component->size.y.type == SIZE_ABSOLUTE)
        component->render_size.y = component->size.y.value;

    if (parent && parent->type != UI_STACK && parent->type != UI_GRID) {
        if (component->size.x.type == SIZE_FLEX_GROW)
            component->render_size.x = parent->render_size.x;
        if (component->size.y.type == SIZE_FLEX_GROW)
            component->render_size.y = parent->render_size.y;
    }


    global_position = vec2_add(global_position, component->render_position);

    if (component->type == UI_STACK) {
        ui_stack_layout((UIStack*)component, global_position);
    } else if (component->type == UI_GRID) {
        ui_grid_layout((UIGrid*)component, global_position);
    } else if (component->type == UI_FRAME) {
        UIFrame* frame = (UIFrame*)component;
        component->render_size.x -= (frame->margin_px + frame->padding_px) * 2;
        component->render_size.y -= (frame->margin_px + frame->padding_px) * 2;

        component->render_position.x += frame->margin_px;
        component->render_position.y += frame->margin_px;

        global_position.x += frame->margin_px + frame->padding_px;
        global_position.y += frame->margin_px + frame->padding_px;
    }

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        ui_layout(child, component, global_position);
    }
}

void ui_render(
    UIComponent* component,
    UIComponent* parent
) {
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
            DrawTexturePro(
                ((UIImage*)component)->texture,
                (Rectangle) {
                    0,
                    0,
                    (float)(((UIImage*)component)->texture.width),
                    (float)(((UIImage*)component)->texture.height),
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
                (float)ui_font.baseSize,
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
