#include <assert.h>
#include <stddef.h>

#include "SizeConstraint.h"
#include "Vec2.h"

inline Size size_absolute(int32_t x, int32_t y) {
    return (Size) {
        .x = (SizeConstraint) {
            .type = SIZE_ABSOLUTE,
            .value = x
        },
        .y = (SizeConstraint) {
            .type = SIZE_ABSOLUTE,
            .value = y
        }
    };
}

int32_t get_render_size_axis(SizeConstraint size_axis, SizeConstraint* parent_size_axis) {
    if (size_axis.type == SIZE_ABSOLUTE) {
        return size_axis.value;
    }

    assert(false);
}

Vec2 get_render_size(Size size, Size* parent_size) {
    return (Vec2) {
        .x = get_render_size_axis(size.x, parent_size ? &parent_size->x : NULL),
        .y = get_render_size_axis(size.y, parent_size ? &parent_size->y : NULL)
    };
}

