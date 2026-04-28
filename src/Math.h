#pragma once

#include <math.h>
#include <stdlib.h>

#include "raylib.h"

Vector3 vec3_add(Vector3 a, Vector3 b);
Vector3 vec3_sub(Vector3 a, Vector3 b);

Vector3 cross(Vector3 a, Vector3 b);
Vector3 normalize(Vector3 vec);
