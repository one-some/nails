#include <assert.h>

#include "UI.h"

void ui_render(
    UIComponent* component,
    UIComponent* parent,
    Vec2 global_position
) {
    assert(component);

    global_position = vec2_add(global_position, component->position);
    Vec2 size = get_render_size(component->size, parent ? &parent->size : NULL);

    switch (component->type) {
        case UI_CONTAINER:
            break;
        case UI_COLOR_RECT:
            DrawRectangle(
                global_position.x,
                global_position.y,
                size.x,
                size.y,
                ((UIColorRect*)component)->color
            );
            break;
    }

    for (int i=0; i<component->children.length; i++) {
        UIComponent* child = component->children.data[i];
        ui_render(child, component, global_position);
    }
}

