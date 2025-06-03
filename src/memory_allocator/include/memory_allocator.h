// src memory_allocator/include/memory_allocator.h

#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <stddef.h>

typedef void *(*allocator_malloc_t)(size_t);
typedef void (*allocator_free_t)(void *);

void set_allocators(allocator_malloc_t malloc_fn, allocator_free_t free_fn);
allocator_malloc_t get_current_malloc(void);
allocator_free_t get_current_free(void);

#endif //MEMORY_ALLOCATOR_H
