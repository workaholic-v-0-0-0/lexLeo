// src/memory_allocator/src/memory_allocator.c

#include "memory_allocator.h"

#include <stdlib.h>

static allocator_malloc_t current_malloc = malloc;
static allocator_free_t current_free = free;
static allocator_realloc_t current_realloc = realloc;

void set_allocators(allocator_malloc_t malloc_fn, allocator_free_t free_fn) {
    current_malloc = malloc_fn ? malloc_fn : malloc;
    current_free = free_fn ? free_fn : free;
}

void set_reallocator(allocator_realloc_t realloc_fn) {
    current_realloc = realloc_fn ? realloc_fn : realloc;
}

allocator_malloc_t get_current_malloc() {
    return current_malloc;
}

allocator_free_t get_current_free() {
    return current_free;
}

allocator_realloc_t get_current_realloc(void) {
    return current_realloc;
}
