#include "Vec.h"

void v_expand_for(Vec* v, size_t n_new) {
    size_t new_count = v->index + n_new;

    size_t required_bytes = new_count * sizeof(void*);
    if (v->allocated_bytes >= required_bytes) return;

    while (v->allocated_bytes < required_bytes) {
        v->allocated_bytes *= 2;
    }

    if (data) {
        void* old_data = data;
        data = malloc(v->allocated_bytes);
        memcpy(
        free(data);
    }
}

void v_add(Vec* v, void* value) {
    v_expand_for(1);
}
