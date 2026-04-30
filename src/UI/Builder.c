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
}

UIStack* ui_stack(UIComponent* parent, Size size, Axis axis) {
    UIComponent* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    *u = ui_component(size, UI_STACK);
    u->axis = axis;
}

UIColorRect* ui_color_rect(UIComponent* parent, Size size, Color color) {
    UIComponent* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    *u = ui_component(size, UI_COLOR_RECT);
    u->color = color;
}

UIColorRect* ui_label(UIComponent* parent, Size size, const char* text) {
    UIComponent* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    *u = ui_component(size, UI_LABEL);
    u->text = text;
}

UIFrame* ui_frame(UIComponent* parent, Size size, Color color, int32_t margin_px) {
    UIComponent* u = malloc(sizeof *u);
    if (parent) v_add(&parent->children, u);

    *u = ui_component(size, UI_FRAME);
    u->color = color;
    u->margin_px = margin_px;
}
