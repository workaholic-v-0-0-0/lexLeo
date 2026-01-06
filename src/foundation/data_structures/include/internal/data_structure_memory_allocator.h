// src/foundation/data_structures/include/internal/data_structure_memory_allocator.h

#ifndef DATA_STRUCTURE_MEMORY_ALLOCATOR_H
#define DATA_STRUCTURE_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
    #include "memory_allocator.h"
    #define DATA_STRUCTURE_MALLOC(size) get_current_malloc()(size)
    #define DATA_STRUCTURE_FREE(ptr)    get_current_free()(ptr)
#else
    #include <stdlib.h>
    #define DATA_STRUCTURE_MALLOC(size) malloc(size)
    #define DATA_STRUCTURE_FREE(ptr)    free(ptr)
#endif

#endif // DATA_STRUCTURE_MEMORY_ALLOCATOR_H
