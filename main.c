#include <stdio.h>
#include <assert.h>

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

    UIColorRect* right = malloc(sizeof(UIColorRect));
    *right = (UIColorRect) {
        .base = (UIComponent) {
            .type = UI_COLOR_RECT,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_ABSOLUTE,  .value = 200 },
                .y = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 }
            }
        },
        .color = GREEN
    };
    v_add(&middle->base.children, right);

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

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(
        ui_root->size.x.value,
        ui_root->size.y.value,
        "nailzz"
    );

    viewport->render_texture = LoadRenderTexture(
        viewport->base.render_size.x, 
        viewport->base.render_size.y
    );


    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 3.0f, 3.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model model = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));

    // TODO: On events only
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        ui_root->size.x.value = GetRenderWidth();
        ui_root->size.y.value = GetRenderHeight();

        //Viewport
        BeginTextureMode(viewport->render_texture);
            ClearBackground(LIGHTGRAY);
            BeginMode3D(camera);

                DrawModelEx(
                    model,
                    (Vector3) { 0.0f, 0.0f, 0.0f },
                    (Vector3) { 0.5f, 1.0f, 0.0f },
                    0.0f,
                    (Vector3) { 1.0f, 1.0f, 1.0f },
                    WHITE
                );

                DrawGrid(10, 1.0f);

            EndMode3D();
        EndTextureMode();

        // Window
        BeginDrawing();
            ClearBackground(RAYWHITE);
            ui_layout(ui_root, NULL, (Vec2) { 0, 0 });
            ui_render(ui_root, NULL);
        EndDrawing();
    }

    UnloadModel(model);

    CloseWindow();
    return 0;
}
