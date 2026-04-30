#include <stdio.h>
#include <assert.h>

#include "Math.h"
#include "Primitive.h"
#include "UI/UI.h"
#include "UI/Event.h"

typedef struct {
    Camera camera;
    Vector3 rotation;
} ViewportCamera;

typedef struct {
    Primitive* target;
    bool pulling;
    Vector3 normal;
    Vector3 point;
    Vector2 start_mouse;
    float pull;
} PullTarget;

static UIViewport* viewport;
static ViewportCamera camera = { 0 };
static PullTarget pull_target = { 0 };
static bool rotating_camera = false;
static Primitive* box;

static int grid_power = 0;

void viewport_on_tick(TickEvent* event) {
    const float speed = 0.2;

    Vector3 forward = (Vector3) {
        sin(camera.rotation.x) * cos(camera.rotation.y),
        sin(camera.rotation.y),
        cos(camera.rotation.x) * cos(camera.rotation.y)
    };

    Vector3 up = { 0, 1, 0 };
    Vector3 left = cross(forward, up);

    // Literally wrote a whole event propagation thing to not
    // use it here cuz it would be uggo #LOL
    Vector3 direction = {
        (IsKeyDown(KEY_D) ? 1.0f : 0.0f) + (IsKeyDown(KEY_A) ? -1.0f : 0.0f),
        (IsKeyDown(KEY_Q) ? 1.0f : 0.0f) + (IsKeyDown(KEY_E) ? -1.0f : 0.0f),
        (IsKeyDown(KEY_W) ? 1.0f : 0.0f) + (IsKeyDown(KEY_S) ? -1.0f : 0.0f)
    };

    camera.camera.position.x += direction.x * speed * left.x;
    camera.camera.position.y += direction.x * speed * left.y;
    camera.camera.position.z += direction.x * speed * left.z;

    camera.camera.position.x += direction.y * speed * up.x;
    camera.camera.position.y += direction.y * speed * up.y;
    camera.camera.position.z += direction.y * speed * up.z;

    camera.camera.position.x += direction.z * speed * forward.x;
    camera.camera.position.y += direction.z * speed * forward.y;
    camera.camera.position.z += direction.z * speed * forward.z;
}

void viewport_on_mouse_down(MouseButtonEvent* event) {
    if (event->button == MOUSE_BUTTON_LEFT) {
        if (pull_target.target)
            pull_target.pulling = true;
    }

    if (event->button == MOUSE_BUTTON_RIGHT)
        rotating_camera = true;
}

void viewport_on_mouse_up(MouseButtonEvent* event) {
    if (event->button == MOUSE_BUTTON_LEFT)
        pull_target.pulling = false;

    if (event->button == MOUSE_BUTTON_RIGHT)
        rotating_camera = false;
}

void viewport_on_key_down(KeyEvent* event) {
    switch (event->key) {
        case KEY_ONE:
            // yes these both suck
            grid_power = fmax(grid_power - 1, -4);
            break;
        case KEY_TWO:
            grid_power = fmin(grid_power + 1, 4);
            break;
        default:
            break;
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
    Vector2 mouse_pos = (Vector2) {
        event->position.x - viewport->base.render_position.x,
        viewport->base.render_size.y - (event->position.y - viewport->base.render_position.y)
    };

    if (pull_target.pulling) {
        Vector2 hit = GetWorldToScreenEx(
            pull_target.point,
            camera.camera,
            viewport->base.render_size.x,
            viewport->base.render_size.y
        );

        Vector2 extended = GetWorldToScreenEx(
            (Vector3) {
                pull_target.point.x + pull_target.normal.x,
                pull_target.point.y + pull_target.normal.y,
                pull_target.point.z + pull_target.normal.z
            },
            camera.camera,
            viewport->base.render_size.x,
            viewport->base.render_size.y
        );

        Vector2 direction = { extended.x - hit.x, extended.y - hit.y };
        float pixelRatio = sqrtf(direction.x * direction.x + direction.y * direction.y);
        direction.x /= pixelRatio;
        direction.y /= pixelRatio;

        Vector2 mouse_delta = {
            mouse_pos.x - pull_target.start_mouse.x,
            mouse_pos.y - pull_target.start_mouse.y
        };

        float px = (mouse_delta.x * direction.x) + (mouse_delta.y * direction.y);

        // TODO: Adjustable grid
        const float grid_size = pow(2.0f, grid_power);
        float units = (int)(px / pixelRatio / grid_size) * grid_size;
        float oomph = units - pull_target.pull;

        if (fabs(oomph) > 0.001f) {
            prim_resize(box, pull_target.normal, oomph);
        }
        
        pull_target.pull = units;
    } else {
        Ray ray = GetScreenToWorldRayEx(
            mouse_pos,
            camera.camera,
            viewport->base.render_size.x,
            viewport->base.render_size.y
        );
        RayCollision collision = prim_ray_collide(box, ray);

        pull_target.target = collision.hit ? box : NULL;
        pull_target.pull = 0.0f;

        if (collision.hit) {
            pull_target.normal = collision.normal;
            pull_target.point = collision.point;
            pull_target.start_mouse = mouse_pos;
        }
    }

    if (rotating_camera) {
        camera.rotation.x -= (float)event->delta.x * 0.008;
        camera.rotation.y += (float)event->delta.y * 0.005;
    }
}

UIComponent* build_root() {
    UIComponent* ui_root = malloc(sizeof *ui_root);
    *ui_root = (UIComponent) {
        .type = UI_CONTAINER,
        .size = size_absolute(800, 500)
    };

    UIVStack* primary = malloc(sizeof *primary);
    *primary = (UIVStack) {
        .base = (UIComponent) {
            .type = UI_VSTACK,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 },
                .y = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 }
            }
        }
    };
    v_add(&ui_root->children, primary);

    UIFrame* material_picker = malloc(sizeof *material_picker);
    *material_picker = (UIFrame) {
        .base = (UIComponent) {
            .type = UI_FRAME,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 },
                .y = (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = 1 }
            }
        },
        .color = (Color) { 0x11, 0x11, 0x11, 0xF0 },
        .margin_px = 12
    };
    v_add(&ui_root->children, material_picker);

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
    v_add(&primary->base.children, top);

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
    v_add(&primary->base.children, middle);

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

    UIColorRect* right = malloc(sizeof(UIColorRect));
    *right = (UIColorRect) {
        .base = (UIComponent) {
            .type = UI_COLOR_RECT,
            .size = (Size) {
                .x = (SizeConstraint) { .type = SIZE_ABSOLUTE,  .value = 300 },
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

    box = (Primitive*)make_box();

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
            BeginMode3D(camera.camera);
                float grid_size = pow(2.0, grid_power);
                DrawGrid(15.0f / grid_size, grid_size);

                Vector3 size = vec3_sub(box->bounds.max, box->bounds.min);
                Vector3 pos = vec3_add(
                    box->bounds.min,
                    (Vector3) { size.x / 2.0f, size.y / 2.0f, size.z / 2.0f }
                );

                DrawMesh(
                    box->mesh,
                    box->material,
                    prim_get_transform(box)
                );

                //DrawCubeV(pos, size, WHITE);
                DrawCubeWiresV(pos, size, (Color) { 0, 0, 0, 0xAA });


                if (pull_target.target) {
                    Vector3 ghost_pos = {
                        pos.x + (0.5 * size.x * pull_target.normal.x),
                        pos.y + (0.5 * size.y * pull_target.normal.y),
                        pos.z + (0.5 * size.z * pull_target.normal.z)
                    };

                    Vector3 ghost_size = {
                        size.x * (1.0f - fabs(pull_target.normal.x)),
                        size.y * (1.0f - fabs(pull_target.normal.y)),
                        size.z * (1.0f - fabs(pull_target.normal.z))
                    };

                    DrawCubeV(
                        ghost_pos,
                        ghost_size,
                        (Color) { 0xFF, 0x00, 0x00, 0xAA }
                    );
                }

            EndMode3D();
        EndTextureMode();

        // Window
        BeginDrawing();
            ClearBackground(RAYWHITE);
            ui_render(ui_root, NULL);
        EndDrawing();
    }

    UnloadFont(ui_font);

    CloseWindow();
    return 0;
}
