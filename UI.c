#include <assert.h>

#include <stdio.h>
#include "UI.h"

inline int32_t* stack_vec_axis(Vec2* vec, AxisSelection axis) {
    return axis == AXIS_X ? &vec->x : &vec->y;
}

inline SizeConstraint* stack_size_axis(Size* size, AxisSelection axis) {
    return axis == AXIS_X ? &size->x : &size->y;
}

void ui_stack_layout(UIComponent* component, Vec2 global_position) {
    AxisSelection a_on = component->type == UI_VSTACK ? AXIS_Y : AXIS_X;
    AxisSelection a_off = component->type == UI_VSTACK ? AXIS_X : AXIS_Y;

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

void ui_layout(
    UIComponent* component,
    UIComponent* parent,
    Vec2 global_position
) {
    assert(component);

    if (component->size.x.type == SIZE_ABSOLUTE)
        component->render_size.x = component->size.x.value;
    if (component->size.y.type == SIZE_ABSOLUTE)
        component->render_size.y = component->size.y.value;

    global_position = vec2_add(global_position, component->render_position);

    if (
        component->type == UI_VSTACK
        || component->type == UI_HSTACK
    ) {
        ui_stack_layout(component, global_position);
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

    switch (component->type) {
        case UI_COLOR_RECT:
            DrawRectangle(
                component->render_position.x,
                component->render_position.y,
                component->render_size.x,
                component->render_size.y,
                ((UIColorRect*)component)->color
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
        default:
            break;
    }

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        ui_render(child, component);
    }
}

