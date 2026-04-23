#include "Vec2.h"

inline Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2) {a.x + b.x, a.y + b.y };
}
