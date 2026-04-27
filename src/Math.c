#include "Math.h"

Vector3 cross(Vector3 a, Vector3 b) {
    return (Vector3) {
        .x = (a.y * b.z) - (a.z * b.y),
        .y = (a.z * b.x) - (a.x * b.z),
        .z = (a.x * b.y) - (a.y * b.x)
    };
}

Vector3 normalize(Vector3 vec) {
    float s = abs(vec.x + vec.y + vec.z);
    return (Vector3) {
        .x = vec.x / s,
        .y = vec.y / s,
        .z = vec.z / s
    };
}
