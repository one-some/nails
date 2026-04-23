#include <stdio.h>

#include "UI.h"

int main() {
    UIComponent ui_root = (UIComponent) {
        .type = UI_CONTAINER,
        .size = size_absolute(500, 500)
    };

    UIColorRect color_rect = (UIColorRect) {
        .base = (UIComponent) {
            .type = UI_COLOR_RECT,
            .size = size_absolute(200, 200),
            .position = (Vec2) { 1, 2 }
        },
        .color = RED
    };

    v_add(&ui_root.children, &color_rect);

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(
        ui_root.size.x.value,
        ui_root.size.y.value,
        "nailzz"
    );

    // TODO: On events only
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        ui_root.size.x.value = GetRenderWidth();
        ui_root.size.y.value = GetRenderHeight();

        BeginDrawing();

            ClearBackground(RAYWHITE);
            ui_render(&ui_root, NULL, (Vec2) { 0, 0 });

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
