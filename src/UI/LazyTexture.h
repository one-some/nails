#pragma once

#include "raylib.h"
#include "Vec.h"

typedef enum {
    LOAD_PHASE_UNLOADED,
    LOAD_PHASE_IMAGE,
    LOAD_PHASE_LOADED
} LazyLoadPhase;

typedef struct {
    char* path;
    LazyLoadPhase load_phase;
    Image img;
    Texture2D texture;
} LazyTexture;

extern Vec texture_cache;

void* lazy_texture_load_thread(void* arg);
void lazy_texture_load_online();
LazyTexture* load_lazy_texture(char* path);
