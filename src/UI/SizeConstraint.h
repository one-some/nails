#pragma once

#include <stdint.h>

#include "UI/Vec2.h"

#define SIZE(x, y) (Size) { x, y }
#define PX(x) (SizeConstraint) { .type = SIZE_ABSOLUTE, .value = x }
#define GROW(x) (SizeConstraint) { .type = SIZE_FLEX_GROW, .value = x }


typedef enum {
    SIZE_ABSOLUTE,
    SIZE_FLEX_GROW,
} SizeType;

typedef struct {
    SizeType type;
    int32_t value;
} SizeConstraint;

typedef struct {
    SizeConstraint x;
    SizeConstraint y;
} Size;

Size size_absolute(int32_t x, int32_t y);

int32_t get_render_size_axis(SizeConstraint size_axis, SizeConstraint* parent_size_axis);
Vec2 get_render_size(Size size, Size* parent_size);
