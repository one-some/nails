#include <assert.h>
#include <stddef.h>

#include "UI/SizeConstraint.h"
#include "UI/Vec2.h"

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

