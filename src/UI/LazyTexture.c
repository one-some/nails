#include <stdio.h>
#include <unistd.h>
#include "UI/LazyTexture.h"

Vec texture_cache = { 0 };

void* lazy_texture_load_thread(void* arg) {
    while (true) {
        for (int i=0; i<texture_cache.length; i++) {
            LazyTexture* tex = texture_cache.data[i];
            if (tex->load_phase == LOAD_PHASE_UNLOADED) {
                tex->img = LoadImage(tex->path);
                tex->load_phase = LOAD_PHASE_IMAGE;
            }
        }
        sleep(1);
    }
}

void lazy_texture_load_online() {
    for (int i=0; i<texture_cache.length; i++) {
        LazyTexture* tex = texture_cache.data[i];
        if (tex->load_phase == LOAD_PHASE_IMAGE) {
            tex->texture = LoadTextureFromImage(tex->img);
            UnloadImage(tex->img);
            tex->load_phase = LOAD_PHASE_LOADED;
        }
    }
}

LazyTexture* load_lazy_texture(char* path) {
    LazyTexture* t = malloc(sizeof *t);
    *t = (LazyTexture) {
        .path = strdup(path),
        .load_phase=LOAD_PHASE_UNLOADED
    };

    v_add(&texture_cache, t);
    return t;
}
