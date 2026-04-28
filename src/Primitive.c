#include <stdlib.h>
#include <stdio.h>

#include "Primitive.h"

BoxPrimitive* make_box() {
    BoxPrimitive* b = malloc(sizeof *b);
    b->base.type = PRIMITIVE_BOX;
    b->base.bounds = (BoundingBox) {
        (Vector3) { 0.0f, 0.0f, 0.0f },
        (Vector3) { 1.0f, -1.0f, 1.0f }
    };

    return b;
}

RayCollision prim_ray_collide(Primitive* prim, Ray ray) {
    return GetRayCollisionBox(ray, prim->bounds);
}

void prim_resize(Primitive* prim, Vector3 normal, float magnitude) {
    prim->bounds.max.x += normal.x * magnitude;
    prim->bounds.max.y += normal.y * magnitude;
    prim->bounds.max.z += normal.z * magnitude;
}
