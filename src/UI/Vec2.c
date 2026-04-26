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

