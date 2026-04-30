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
    printf("Resize %f (%f, %f, %f)\n", magnitude, normal.x, normal.y, normal.z);
    // change max if resizing on positive axis, otherwise min
    float* xAxis = normal.x > 0.0f ? &prim->bounds.max.x : &prim->bounds.min.x;
    float* yAxis = normal.y > 0.0f ? &prim->bounds.max.y : &prim->bounds.min.y;
    float* zAxis = normal.z > 0.0f ? &prim->bounds.max.z : &prim->bounds.min.z;

    *(xAxis) += normal.x * magnitude;
    *(yAxis) += normal.y * magnitude;
    *(zAxis) += normal.z * magnitude;
}
