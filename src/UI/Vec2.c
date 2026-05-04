#include "Vec2.h"

inline Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2) {a.x + b.x, a.y + b.y };
}

inline Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return (Vec2) {a.x - b.x, a.y - b.y };
}

inline bool vec2_eq(Vec2 a, Vec2 b) {
    return a.x == b.x && a.y == b.y;
}

inline bool vec2_in_rec(Vec2 point, Vec2 rect_start, Vec2 rect_size) {
    if (point.x < rect_start.x) return false;
    if (point.y < rect_start.y) return false;

    if (point.x - rect_start.x > rect_size.x) return false;
    if (point.y - rect_start.y > rect_size.y) return false;

    return true;
}
