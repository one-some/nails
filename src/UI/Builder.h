#pragma once
#include "UI/UI.h"

UIComponent ui_base_component(Size size);

UIComponent* ui_container(UIComponent* parent, Size size);
UIComponent* ui_stack(UIComponent* parent, Size size, Axis axis, int32_t gap);
UIComponent* ui_color_rect(UIComponent* parent, Size size, Color color);
UIComponent* ui_viewport(UIComponent* parent, Size size);
UIComponent* ui_label(UIComponent* parent, Size size, const char* text, int32_t font_size);
UIComponent* ui_frame(UIComponent* parent, Size size, Color color, int32_t margin_px, int32_t padding_px);
UIComponent* ui_grid(UIComponent* parent, Size size, int32_t columns, int32_t gap_px);
UIComponent* ui_image(UIComponent* parent, Size size, Texture2D texture);
