#include <stdio.h>

#include "UI.h"

UIComponent* build_root() {
    UIVStack* ui_root = malloc(sizeof(UIVStack));
    *ui_root = (UIVStack) {
        .base = (UIComponent) {
            .type = UI_VSTACK,
            .size = size_absolute(500, 500)
        }
    };

    UIColorRect* top = malloc(sizeof(UIColorRect));
    *top = (UIColorRect) {
        .base = (UIComponent) {
            .type = UI_COLOR_RECT,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 },
                .y = (SizeConstraint) { .type = SIZE_ABSOLUTE,  .value = 20 }
            }
        },
        .color = RED
    };
    v_add(&ui_root->base.children, top);

    UIColorRect* middle = malloc(sizeof(UIColorRect));
    *middle = (UIColorRect) {
        .base = (UIComponent) {
            .type = UI_COLOR_RECT,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 },
                .y = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 }
            }
        },
        .color = BLUE
    };
    v_add(&ui_root->base.children, middle);

    UIColorRect* bottom = malloc(sizeof(UIColorRect));
    *bottom = (UIColorRect) {
        .base = (UIComponent) {
            .type = UI_COLOR_RECT,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 },
                .y = (SizeConstraint) { .type = SIZE_ABSOLUTE,  .value = 20 }
            }
        },
        .color = BLACK
    };
    v_add(&ui_root->base.children, bottom);

    return (UIComponent*)ui_root;
}

int main() {
    UIComponent* ui_root = build_root();

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(
        ui_root->size.x.value,
        ui_root->size.y.value,
        "nailzz"
    );

    // TODO: On events only
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        ui_root->size.x.value = GetRenderWidth();
        ui_root->size.y.value = GetRenderHeight();

        BeginDrawing();

            ClearBackground(RAYWHITE);
            ui_layout(ui_root, NULL, (Vec2) { 0, 0 });
            ui_render(ui_root, NULL);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
