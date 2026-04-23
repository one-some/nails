#pragma once

#include <stdint.h>

typedef struct {
    int32_t x;
    int32_t y;
} Vec2;

Vec2 vec2_add(Vec2 a, Vec2 b);
