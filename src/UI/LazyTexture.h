#pragma once

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
