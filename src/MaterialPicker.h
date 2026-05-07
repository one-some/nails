#pragma once

#include "raylib.h"
#include "Vec.h"
#include "UI/LazyTexture.h"

typedef struct {
    char* name;
    char* path;
    LazyTexture* color;
} Matthewterial;

void materials_populate_from_disk(Vec* materials, const char* directory);
