#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "raymath.h"
#include "Math.h"
#include "Primitive.h"

BoxPrimitive* make_box() {
    BoxPrimitive* b = malloc(sizeof *b);
    b->base.type = PRIMITIVE_BOX;
    b->base.bounds = (BoundingBox) {
        (Vector3) { 0.0f, -1.0f, 0.0f },
        (Vector3) { 1.0f, 0.0f, 1.0f }
    };

    b->base.material = LoadMaterialDefault();
    b->base.material.maps[0].color = WHITE;

    prim_recompute_mesh(&b->base, true);

    return b;
}

RayCollision prim_ray_collide(Primitive* prim, Ray ray) {
    return GetRayCollisionBox(ray, prim->bounds);
}

void prim_recompute_mesh(Primitive* prim, bool new) {
    assert(prim->type == PRIMITIVE_BOX);

    // TODO: Maybe track this in a MeshState flag or something with EMPTY, DIRTY, CLEAN, etc.
    if (!new) {
        for (int i=0; i<6; i++) {
            UnloadMesh(prim->meshes[i]);
        }
    }

    Vector3 size = vec3_sub(prim->bounds.max, prim->bounds.min);
    float vertices[] = {
        0, 0, size.z,
        size.x, 0, size.z,
        size.x, size.y, size.z,
        0, size.y, size.z,

        0, 0, 0,
        0, size.y, 0,
        size.x, size.y, 0,
        size.x, 0, 0,

        0, size.y, 0,
        0, size.y, size.z,
        size.x, size.y, size.z,
        size.x, size.y, 0,

        0, 0, 0,
        size.x, 0, 0,
        size.x, 0, size.z,
        0, 0, size.z,

        size.x, 0, 0,
        size.x, size.y, 0,
        size.x, size.y, size.z,
        size.x, 0, size.z,

        0, 0, 0,
        0, 0, size.z,
        0, size.y, size.z,
        0, size.y, 0
    };

    float texcoords[] = {
        0.0f, 0.0f,
        size.x, 0.0f,
        size.x, size.y,
        0.0f, size.y,

        size.x, 0.0f,
        size.x, size.y,
        0.0f, size.y,
        0.0f, 0.0f,

        0.0f, size.z,
        0.0f, 0.0f,
        size.x, 0.0f,
        size.x, size.z,

        size.x, size.z,
        0.0f, size.z,
        0.0f, 0.0f,
        size.x, 0.0f,

        size.z, 0.0f,
        size.z, size.y,
        0.0f, size.y,
        0.0f, 0.0f,

        0.0f, 0.0f,
        size.z, 0.0f,
        size.z, size.y,
        0.0f, size.y
    };

    float normals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,

        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f
    };

    for (int i=0; i<6; i++) {
        Mesh mesh = { 0 };

        mesh.vertices = (float*)malloc(4*3*sizeof(float));
        memcpy(mesh.vertices, vertices + 4*3*i, 4*3*sizeof(float));

        mesh.texcoords = (float*)RL_MALLOC(4*2*sizeof(float));
        memcpy(mesh.texcoords, texcoords + 4*2*i, 4*2*sizeof(float));

        mesh.normals = (float *)RL_MALLOC(24*3*sizeof(float));
        memcpy(mesh.normals, normals, 24*3*sizeof(float));

        mesh.indices = (unsigned short *)RL_MALLOC(6*sizeof(unsigned short));
        mesh.indices[0] = 0;
        mesh.indices[1] = 1;
        mesh.indices[2] = 2;
        mesh.indices[3] = 0;
        mesh.indices[4] = 2;
        mesh.indices[5] = 3;

        mesh.vertexCount = 4;
        mesh.triangleCount = 2;

        // We do a full reupload for now instead of updating the buffer, cuz we can't resize it
        // Future optimization idea would to be checking the size and maybe updating
        prim->meshes[i] = mesh;
        UploadMesh(&prim->meshes[i], false);
    }
}

void prim_resize(Primitive* prim, Vector3 normal, float magnitude) {
    // Let's play a game called no judgement

    // change max if resizing on positive axis, otherwise min
    float* xAxis = normal.x > 0.0f ? &prim->bounds.max.x : &prim->bounds.min.x;
    float* xOffAxis = normal.x > 0.0f ? &prim->bounds.min.x : &prim->bounds.max.x;

    float* yAxis = normal.y > 0.0f ? &prim->bounds.max.y : &prim->bounds.min.y;
    float* yOffAxis = normal.y > 0.0f ? &prim->bounds.min.y : &prim->bounds.max.y;

    float* zAxis = normal.z > 0.0f ? &prim->bounds.max.z : &prim->bounds.min.z;
    float* zOffAxis = normal.z > 0.0f ? &prim->bounds.min.z : &prim->bounds.max.z;

    Vector3 diff = Vector3Scale(normal, magnitude);
    Vector3 result = { *xAxis + diff.x, *yAxis + diff.y, *zAxis + diff.z };

    *(xAxis) += (normal.x * (result.x - *xOffAxis)) > 0.0f ? diff.x : 0.0f;
    *(yAxis) += (normal.y * (result.y - *yOffAxis)) > 0.0f ? diff.y : 0.0f;
    *(zAxis) += (normal.z * (result.z - *zOffAxis)) > 0.0f ? diff.z : 0.0f;

    prim_recompute_mesh(prim, false);
}

Matrix prim_get_transform(Primitive* prim) {
    Matrix mat = MatrixIdentity();

    mat = MatrixMultiply(
        mat,
        MatrixTranslate(
            prim->bounds.min.x,
            prim->bounds.min.y,
            prim->bounds.min.z
        )
    );

    return mat;
}
