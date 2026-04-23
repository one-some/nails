#include <stdio.h>
#include "raylib.h"

#include "Vec.h"
#include "SizeConstraint.h"

typedef enum {
    UI_CONTAINER,
    UI_COLOR_RECT,
} UIType;

typedef struct {
    UIType type;
    Size size;
    Vec* children;
} UIComponent;

int main() {
    UIComponent ui_root = (UIComponent) {
        .type = UI_CONTAINER,
        .size = size_absolute(500, 500)
    };

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
            DrawRectangle(0, 0, 100, 100, RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
