#pragma once
#include "UI/UI.h"

UIComponent ui_base_component(Size size);

UIComponent* ui_container(UIComponent* parent, Size size);
UIStack* ui_stack(UIComponent* parent, Size size, Axis axis);
UIColorRect* ui_color_rect(UIComponent* parent, Size size, Color color);
UILabel* ui_label(UIComponent* parent, Size size, const char* text);
UIFrame* ui_frame(UIComponent* parent, Size size, Color color, int32_t margin_px);
