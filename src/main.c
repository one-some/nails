#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "rlgl.h"
#include "Math.h"
#include "Primitive.h"
#include "UI/Builder.h"
#include "UI/Event.h"
#include "MaterialPicker.h"

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
static UIComponent* material_frame;
static ViewportCamera camera = { 0 };
static PullTarget pull_target = { 0 };
static bool rotating_camera = false;
static Vector2 grid_pos = { 0 };
static Primitive* box;

static int grid_power = 0;

void viewport_on_tick(Event* event) {
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
        case KEY_M:
            material_frame->visible = !material_frame->visible;
            break;
        default:
            break;
    }
}

void draw_grid(float spacing) {
    int half_slices = 10.0f / spacing;

    rlBegin(RL_LINES);
        rlColor3f(0.75f, 0.75f, 0.75f);

        for (int i = -half_slices; i <= half_slices; i++) {
            rlVertex3f((float)i*spacing + grid_pos.x, 1.0f, (float)-half_slices*spacing + grid_pos.y);
            rlVertex3f((float)i*spacing + grid_pos.x, 1.0f, (float)half_slices*spacing + grid_pos.y);

            rlVertex3f((float)-half_slices*spacing + grid_pos.x, 1.0f, (float)i*spacing + grid_pos.y);
            rlVertex3f((float)half_slices*spacing + grid_pos.x, 1.0f, (float)i*spacing + grid_pos.y);
        }
    rlEnd();
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

    Ray ray = GetScreenToWorldRayEx(
        mouse_pos,
        camera.camera,
        viewport->base.render_size.x,
        viewport->base.render_size.y
    );
    float t = -ray.position.y / ray.direction.y;
    grid_pos.x = (int)(ray.position.x + t * ray.direction.x);
    grid_pos.y = (int)(ray.position.z + t * ray.direction.z);

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


void mat_button_down(MouseButtonEvent* event) {
    material_frame->visible = false;
    Matthewterial* mat = (Matthewterial*)event->base.target->data;

    box->material.maps[0].texture = mat->color.texture;
}

UIComponent* build_root(Vec* materials) {
    UIComponent* root = ui_container(NULL, SIZE(PX(800), PX(500)));
    UIComponent* primary = ui_stack(root, SIZE(GROW(1), GROW(1)), AXIS_Y, 0);

    UIComponent* top = ui_frame(primary, SIZE(GROW(1), PX(20)), RED, 0, 0);
    UIComponent* label = ui_label(top, SIZE(GROW(1), GROW(1)), "nailzz! olympic girl drama", 24);

    UIComponent* middle = ui_stack(primary, SIZE(GROW(1), GROW(1)), AXIS_X, 0);


    viewport = (UIViewport*) ui_viewport(middle, SIZE(GROW(1), GROW(1)));
    viewport->base.event_handlers.on_tick = &viewport_on_tick;
    viewport->base.event_handlers.on_mouse_down = &viewport_on_mouse_down;
    viewport->base.event_handlers.on_mouse_up = &viewport_on_mouse_up;
    viewport->base.event_handlers.on_mouse_move = &viewport_on_mouse_move;
    viewport->base.event_handlers.on_key_down = &viewport_on_key_down;

    UIComponent* right = ui_frame(middle, SIZE(PX(300), GROW(1)), (Color) {0, 0, 0, 200}, 0, 0);
    UIComponent* bottom = ui_frame(primary, SIZE(GROW(1), PX(20)), BLUE, 0, 0);

    /* Material Picker <3 */
    material_frame = ui_frame(
        root,
        SIZE(GROW(1), GROW(1)),
        (Color) { 0x11, 0x11, 0x11, 0xF0 },
        24,
        8
    );
    material_frame->visible = false;

    UIComponent* mat_stack = ui_stack(material_frame, SIZE(GROW(1), GROW(1)), AXIS_Y, 12);
    UIComponent* mat_label = ui_label(mat_stack, SIZE(GROW(1), PX(20)), "Material Library", 24);

    UIComponent* mat_grid_container = ui_frame(
        mat_stack,
        SIZE(GROW(1), GROW(1)),
        BLANK,
        0,
        0
    );
    ((UIFrame*)mat_grid_container)->allow_scrolling = true;
    UIComponent* mat_grid = ui_grid(mat_grid_container, SIZE(GROW(1), GROW(1)), 8, 8);

    for (int i=0; i<materials->length; i++) {
        Matthewterial* mat = materials->data[i];

        UIComponent* a_mat_stack = ui_stack(mat_grid, SIZE(GROW(1), GROW(1)), AXIS_Y, 0);
        ui_image(a_mat_stack, SIZE(GROW(1), OTHER()), &mat->color);
        ui_label(a_mat_stack, SIZE(GROW(1), GROW(1)), mat->name, 24);

        a_mat_stack->data = mat;
        a_mat_stack->event_handlers.on_mouse_down = &mat_button_down;
    }

    return root;
}

void do_events(UIComponent* root) {
    static uint32_t mouse_buttons_down = 0;
    static Vec2 mouse_position = { 0 };
    // This suks
    static bool keys_down[512] = { 0 };

    // Mouse move
    Vec2 mouse_pos = {
        .x = GetMouseX(),
        .y = GetMouseY()
    };

    Event tick_event = (Event) { .type = EVENT_TICK };
    ui_propagate_event(root, (Event*)&tick_event, mouse_pos);

    // Mouse buttons
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
                ui_propagate_event(root, (Event*)&event, mouse_pos);
            }
        } else if (was_down) {
            // MOUSE UP
            MouseButtonEvent event = (MouseButtonEvent) {
                .base = (Event) { .type = EVENT_MOUSE_UP },
                .button = i
            };
            ui_propagate_event(root, (Event*)&event, mouse_pos);
        }
    }
    mouse_buttons_down = new_mouse_buttons_down;

    // Keys
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
        ui_propagate_event(root, (Event*)&event, mouse_pos);
    }

    // Great naming claire
    if (!vec2_eq(mouse_position, mouse_pos)) {
        MouseMoveEvent event = (MouseMoveEvent) {
            .base = (Event) { .type = EVENT_MOUSE_MOVE },
            .position = mouse_position,
            .delta = vec2_sub(mouse_pos, mouse_position)
        };
        mouse_position = mouse_pos;
        ui_propagate_event(root, (Event*)&event, mouse_pos);
    }


    // Mouse wheel
    Vector2 wheel = GetMouseWheelMoveV();

    if (fabs(wheel.x) > 0.001f) {
        MouseWheelEvent event = {
            .base = (Event) { .type = EVENT_MOUSE_WHEEL },
            .delta = wheel.x,
            .axis = AXIS_X
        };
        ui_propagate_event(root, (Event*)&event, mouse_pos);
    }

    if (fabs(wheel.y) > 0.001f) {
        MouseWheelEvent event = {
            .base = (Event) { .type = EVENT_MOUSE_WHEEL },
            .delta = wheel.y,
            .axis = AXIS_Y
        };
        ui_propagate_event(root, (Event*)&event, mouse_pos);
    }

}

int main() {
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(
        800,
        500,
        "nailzz"
    );
    ui_font = LoadFontEx("ibm.ttf", 24, NULL, 0);
    SetTextureFilter(ui_font.texture, TEXTURE_FILTER_POINT);

    Vec materials = { 0 };
    materials_populate_from_disk(&materials, "textures");

    UIComponent* ui_root = build_root(&materials);
    ui_layout(ui_root, NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, materials_lazy_load_thread, &materials);
    pthread_detach(thread);

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

    Vec2 window_size = { 0 };

    // TODO: On events only
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        do_events(ui_root);

        Vec2 new_window_size = {
            .x = GetRenderWidth(),
            .y = GetRenderHeight()
        };

        ui_root->size.x.value = new_window_size.x;
        ui_root->size.y.value = new_window_size.y;
        ui_layout(ui_root, NULL);

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
                draw_grid(grid_size);

                Vector3 size = vec3_sub(box->bounds.max, box->bounds.min);
                Vector3 pos = vec3_add(
                    box->bounds.min,
                    (Vector3) { size.x / 2.0f, size.y / 2.0f, size.z / 2.0f }
                );

                DrawMesh(box->mesh, box->material, prim_get_transform(box));
                DrawCubeWiresV(pos, size, (Color) { 0, 0, 0, 0xAA });

                if (pull_target.target) {
                    Vector3 ghost_pos = {
                        pos.x + (0.505 * size.x * pull_target.normal.x),
                        pos.y + (0.505 * size.y * pull_target.normal.y),
                        pos.z + (0.505 * size.z * pull_target.normal.z)
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
            ClearBackground(MAGENTA);
            ui_render(ui_root, NULL);
        EndDrawing();

        materials_lazy_load_online(&materials);
    }

    UnloadFont(ui_font);

    CloseWindow();
    return 0;
}
