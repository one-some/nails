#include "Vec.h"

void v_expand_for(Vec* v, size_t n_new) {
    size_t new_count = v->length + n_new;

    size_t required_bytes = new_count * sizeof(void*);

    if (v->allocated_bytes >= required_bytes) return;

    size_t old_allocated_bytes = v->allocated_bytes;

    if (v->allocated_bytes == 0) v->allocated_bytes = 64;
    while (v->allocated_bytes < required_bytes) {
        v->allocated_bytes *= 2;
    }

    void* old_data = v->data;
    v->data = malloc(v->allocated_bytes);

    if (old_data) {
        memcpy(v->data, old_data, old_allocated_bytes);
        free(old_data);
    }
}

void v_add(Vec* v, void* value) {
    v_expand_for(v, 1);
    v->data[v->length++] = value;
}
