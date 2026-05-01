#include "UI/Builder.h"

UIComponent ui_component(Size size, UIType type) {
    return (UIComponent) {
        .type = type,
        .size = size
    };
}

UIComponent* ui_container(UIComponent* parent, Size size) {
    UIComponent* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    *u = ui_component(size, UI_CONTAINER);
    return u;
}

UIComponent* ui_stack(UIComponent* parent, Size size, Axis axis) {
    UIStack* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    u->base = ui_component(size, UI_STACK);
    u->axis = axis;

    return (UIComponent*)u;
}

UIComponent* ui_viewport(UIComponent* parent, Size size) {
    UIViewport* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    u->base = ui_component(size, UI_VIEWPORT);

    return (UIComponent*)u;
}

UIComponent* ui_label(UIComponent* parent, Size size, const char* text) {
    UILabel* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    u->base = ui_component(size, UI_LABEL);
    u->text = text;

    return (UIComponent*)u;
}

UIComponent* ui_frame(UIComponent* parent, Size size, Color color, int32_t margin_px, int32_t padding_px) {
    UIFrame* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    u->base = ui_component(size, UI_FRAME);
    u->color = color;
    u->margin_px = margin_px;
    u->padding_px = padding_px;

    return (UIComponent*)u;
}

UIComponent* ui_grid(UIComponent* parent, Size size, int32_t columns, int32_t gap_px) {
    UIGrid* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    u->base = ui_component(size, UI_GRID);
    u->columns = columns;
    u->gap_px = gap_px;

    return (UIComponent*)u;
}

UIComponent* ui_image(UIComponent* parent, Size size, Texture2D texture) {
    UIImage* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    u->base = ui_component(size, UI_IMAGE);
    u->texture = texture;

    return (UIComponent*)u;
}
