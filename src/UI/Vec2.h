#pragma once

#include <stdint.h>

typedef struct {
    int32_t x;
    int32_t y;
} Vec2;

Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);

bool vec2_eq(Vec2 a, Vec2 b);

bool vec2_in_rec(Vec2 point, Vec2 rect_start, Vec2 rect_size);
