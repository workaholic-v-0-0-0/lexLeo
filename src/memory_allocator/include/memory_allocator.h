// src/memory_allocator/include/memory_allocator.h

#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <stddef.h>

typedef void *(*allocator_malloc_t)(size_t);
typedef void (*allocator_free_t)(void *);
typedef void *(*allocator_realloc_t)(void *ptr, size_t size);

void set_allocators(allocator_malloc_t malloc_fn, allocator_free_t free_fn);
void set_reallocator(allocator_realloc_t realloc_fn);

allocator_malloc_t get_current_malloc(void);
allocator_free_t get_current_free(void);
allocator_realloc_t get_current_realloc(void);

#endif //MEMORY_ALLOCATOR_H
