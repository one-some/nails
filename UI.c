#include <assert.h>

#include "UI.h"

void ui_stack_layout(
    UIComponent* component,
) {
    int32_t play_room = AXIS(component->render_size);
    int32_t position = 0;
    int32_t total_flex_pie = 0;

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];

        switch (child->size.y.type) {
            case SIZE_ABSOLUTE:
                play_room -= child->size.y.value;
                break;
            case SIZE_FLEX_GROW:
                total_flex_pie += child->size.y.value;
                break;
        }
    }

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        child->render_position.x = 0;
        child->render_position.y = position;

        child->render_size.x = component->render_size.x;

        if (child->size.y.type == SIZE_FLEX_GROW) {
            child->render_size.y = (play_room / total_flex_pie) * child->size.y.value;
        }

        position += child->render_size.y;
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


    if (
        component->type == UI_VSTACK
        || component->type == UI_HSTACK
    ) {
#define AXIS(value) (component->type(value.x)
#define OFFAXIS(value) (value.y)
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
        case UI_CONTAINER:
            break;
        case UI_COLOR_RECT:
            DrawRectangle(
                component->render_position.x,
                component->render_position.y,
                component->render_size.x,
                component->render_size.y,
                ((UIColorRect*)component)->color
            );
            break;
    }

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        ui_render(child, component);
    }
}

