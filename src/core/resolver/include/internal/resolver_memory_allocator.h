// src/core/resolver/include/internal/resolver_memory_allocator.h

#ifndef LEXLEO_RESOLVER_MEMORY_ALLOCATOR_H
#define LEXLEO_RESOLVER_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
    #include "memory_allocator.h"
    #define RESOLVER_MALLOC(size) get_current_malloc()(size)
    #define RESOLVER_FREE(ptr)    get_current_free()(ptr)
#else
    #include <stdlib.h>
    #define RESOLVER_MALLOC(size) malloc(size)
    #define RESOLVER_FREE(ptr)    free(ptr)
#endif

#endif //LEXLEO_RESOLVER_MEMORY_ALLOCATOR_H
