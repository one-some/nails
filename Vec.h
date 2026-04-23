#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t length;
    size_t allocated_bytes;
    void** data;
} Vec;

void v_expand_for(Vec* v, size_t n_new);
void v_add(Vec* v, void* value);
