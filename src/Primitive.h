#pragma once

#include "raylib.h"

typedef enum {
    PRIMITIVE_BOX
} PrimitiveType;

typedef struct {
    PrimitiveType type;
    BoundingBox bounds;
    Mesh meshes[6];
    Material material;
} Primitive;

typedef struct {
    Primitive base;
} BoxPrimitive;

BoxPrimitive* make_box();
RayCollision prim_ray_collide(Primitive* prim, Ray ray);
void prim_recompute_mesh(Primitive* prim, bool new);
void prim_resize(Primitive* prim, Vector3 normal, float magnitude);
Matrix prim_get_transform(Primitive* prim);
