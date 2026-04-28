#include <stdio.h>
#include <assert.h>

#include "Math.h"
#include "UI/UI.h"
#include "UI/Event.h"

typedef struct {
    Camera camera;
    Vector3 rotation;
} ViewportCamera;

static UIViewport* viewport;
static ViewportCamera camera = { 0 };
static Vector2 control_direction = { 0 };
static bool dragging = false;

void viewport_on_tick(TickEvent* event) {
    const float speed = 0.2;

    Vector3 forward = (Vector3) {
        sin(camera.rotation.x) * cos(camera.rotation.y),
        sin(camera.rotation.y),
        cos(camera.rotation.x) * cos(camera.rotation.y)
    };

    Vector3 left = cross(forward, (Vector3) { 0, 1, 0 });

    camera.camera.position.x += control_direction.y * speed * forward.x;
    camera.camera.position.y += control_direction.y * speed * forward.y;
    camera.camera.position.z += control_direction.y * speed * forward.z;

    camera.camera.position.x += control_direction.x * speed * left.x;
    camera.camera.position.y += control_direction.x * speed * left.y;
    camera.camera.position.z += control_direction.x * speed * left.z;
}

void viewport_on_mouse_down(MouseButtonEvent* event) {
    if (event->button == MOUSE_BUTTON_RIGHT) {
        dragging = true;
        //DisableCursor();
    }
}

void viewport_on_mouse_up(MouseButtonEvent* event) {
    if (event->button == MOUSE_BUTTON_RIGHT) {
        dragging = false;
        //EnableCursor();
    }
}


void look_at(Vector3 pos) {
    Vector3 diff = normalize((Vector3) {
        .x = pos.x - camera.camera.position.x,
        .y = pos.y - camera.camera.position.y,
        .z = pos.z - camera.camera.position.z
    });

    camera.rotation.x = atan2(diff.z, diff.x);
    camera.rotation.y = cos(diff.y);
}

void viewport_on_mouse_move(MouseMoveEvent* event) {
    if (!dragging) return;
    camera.rotation.x -= (float)event->delta.x * 0.008;
    camera.rotation.y += (float)event->delta.y * 0.005;
}

void viewport_on_key_down(KeyEvent* event) {
    switch (event->key) {
        case KEY_W:
            control_direction.y = 1.0f;
            break;
        case KEY_S:
            control_direction.y = -1.0f;
            break;
        case KEY_A:
            control_direction.x = -1.0f;
            break;
        case KEY_D:
            control_direction.x = 1.0f;
            break;
        default:
            break;
    }
}

void viewport_on_key_up(KeyEvent* event) {
    switch (event->key) {
        case KEY_W:
        case KEY_S:
            control_direction.y = 0.0f;
            break;
        case KEY_A:
        case KEY_D:
            control_direction.x = 0.0f;
            break;
        default:
            break;
    }
}

UIComponent* build_root() {
    UIVStack* ui_root = malloc(sizeof(UIVStack));
    *ui_root = (UIVStack) {
        .base = (UIComponent) {
            .type = UI_VSTACK,
            .size = size_absolute(800, 500)
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

    UILabel* label = malloc(sizeof(UILabel));
    *label = (UILabel) {
        .base = (UIComponent) {
            .type = UI_LABEL,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 },
                .y = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 }
            }
        },
        .text = "nailzz! olympic girl drama",
    };
    v_add(&top->base.children, label);

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
    viewport->base.event_handlers.on_tick = &viewport_on_tick;
    viewport->base.event_handlers.on_mouse_down = &viewport_on_mouse_down;
    viewport->base.event_handlers.on_mouse_up = &viewport_on_mouse_up;
    viewport->base.event_handlers.on_mouse_move = &viewport_on_mouse_move;
    viewport->base.event_handlers.on_key_down = &viewport_on_key_down;
    viewport->base.event_handlers.on_key_up = &viewport_on_key_up;

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

    //UILabel* ll = malloc(sizeof(UILabel));
    //*ll = (UILabel) {
    //    .base = (UIComponent) {
    //        .type = UI_LABEL,
    //        .size = (Size) {
    //            .x = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 },
    //            .y = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 }
    //        }
    //    },
    //    .text = "This old town is filled with sin\nIt'll swallow you in",
    //};
    //v_add(&right->base.children, ll);

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
    ui_font = LoadFontEx("ibm.ttf", 24, NULL, 0);
    SetTextureFilter(ui_font.texture, TEXTURE_FILTER_POINT);

    viewport->render_texture = LoadRenderTexture(
        viewport->base.render_size.x, 
        viewport->base.render_size.y
    );

    camera.camera.position = (Vector3){ 3.0f, -3.0f, 3.0f };
    camera.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.camera.fovy = 70.0f;
    camera.camera.projection = CAMERA_PERSPECTIVE;
    look_at((Vector3) { 0.0f, 0.0f, 0.0f });

    Model model = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));

    uint32_t mouse_buttons_down = 0;
    Vec2 mouse_position = { 0 };
    Vec2 window_size = { 0 };
    // This suks
    bool keys_down[512] = { 0 };

    // TODO: On events only
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        TickEvent tick_event = (TickEvent) {
            .base = (Event) { .type = EVENT_TICK },
        };
        ui_propagate_event(ui_root, (Event*)&tick_event);

        uint32_t new_mouse_buttons_down = 0;
        for (int i=0; i<=MOUSE_BUTTON_BACK; i++) {
            uint32_t mask = 1 << i;
            bool was_down = (mouse_buttons_down & mask) != 0;

            if (IsMouseButtonDown(i)) {
                new_mouse_buttons_down |= mask;

                if (!was_down) {
                    MouseButtonEvent event = (MouseButtonEvent) {
                        .base = (Event) { .type = EVENT_MOUSE_DOWN },
                        .button = i
                    };
                    ui_propagate_event(ui_root, (Event*)&event);
                }
            } else if (was_down) {
                // MOUSE UP
                MouseButtonEvent event = (MouseButtonEvent) {
                    .base = (Event) { .type = EVENT_MOUSE_UP },
                    .button = i
                };
                ui_propagate_event(ui_root, (Event*)&event);
            }
        }
        mouse_buttons_down = new_mouse_buttons_down;

        for (int i=0; i<512; i++) {
            bool pressed_now = IsKeyDown(i);
            bool pressed_then = keys_down[i];

            if (pressed_now == pressed_then)
                continue;

            keys_down[i] = pressed_now;

            KeyEvent event = (KeyEvent) {
                .base = (Event) {
                    .type = pressed_now ? EVENT_KEY_DOWN : EVENT_KEY_UP
                },
                .key = i
            };
            ui_propagate_event(ui_root, (Event*)&event);
        }


        Vec2 new_mouse_position = {
            .x = GetMouseX(),
            .y = GetMouseY()
        };

        if (!vec2_eq(mouse_position, new_mouse_position)) {
            MouseMoveEvent event = (MouseMoveEvent) {
                .base = (Event) { .type = EVENT_MOUSE_MOVE },
                .position = mouse_position,
                .delta = vec2_sub(new_mouse_position, mouse_position)
            };
            mouse_position = new_mouse_position;
            ui_propagate_event(ui_root, (Event*)&event);
        }

        Vec2 new_window_size = {
            .x = GetRenderWidth(),
            .y = GetRenderHeight()
        };

        ui_root->size.x.value = new_window_size.x;
        ui_root->size.y.value = new_window_size.y;
        ui_layout(ui_root, NULL, (Vec2) { 0, 0 });

        if (!vec2_eq(window_size, new_window_size)) {
            UnloadRenderTexture(viewport->render_texture);
            viewport->render_texture = LoadRenderTexture(
                viewport->base.render_size.x, 
                viewport->base.render_size.y
            );

            window_size = new_window_size;
        }


        camera.rotation.y = fmax(fmin((PI / 2.0f) + 0.01, camera.rotation.y), -(PI / 2.0f) - 0.01);
        camera.camera.target = camera.camera.position;
        camera.camera.target.x += sin(camera.rotation.x) * cos(camera.rotation.y);
        camera.camera.target.y += sin(camera.rotation.y);
        camera.camera.target.z += cos(camera.rotation.x) * cos(camera.rotation.y);

        //Viewport
        BeginTextureMode(viewport->render_texture);
            ClearBackground(BLACK);
            BeginMode3D(*(Camera*)&camera);

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
            ui_render(ui_root, NULL);
        EndDrawing();
    }

    UnloadModel(model);
    UnloadFont(ui_font);

    CloseWindow();
    return 0;
}
