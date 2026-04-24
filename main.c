#include <stdio.h>

#include "UI.h"

static UIViewport* viewport;

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

    UIHStack* middle = malloc(sizeof(UIHStack));
    *middle = (UIHStack) {
        .base = (UIComponent) {
            .type = UI_HSTACK,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 },
                .y = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 }
            }
        },
    };
    v_add(&ui_root->base.children, middle);

    viewport = malloc(sizeof(UIViewport));
    *viewport = (UIViewport) {
        .base = (UIComponent) {
            .type = UI_VIEWPORT,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 },
                .y = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 }
            }
        },
    };
    v_add(&middle->base.children, viewport);




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
    ui_layout(ui_root, NULL, (Vec2) { 0, 0 });

    viewport->render_texture = LoadRenderTexture(
        viewport->render_size.x, 
        viewport->render_size.y,
    );

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
