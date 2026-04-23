#pragma once

typedef struct {
    size_t index;
    size_t allocated_bytes;
    void* data;
} Vec;

void v_expand_for(Vec* v, size_t n_new);

void v_add(Vec* v, void* value);
