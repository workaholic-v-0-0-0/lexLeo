// src/symtab/include/internal/symtab_memory_allocator.h

#ifndef SYMTAB_MEMORY_ALLOCATOR_H
#define SYMTAB_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
    #include "memory_allocator.h"
    #define SYMTAB_MALLOC(size) get_current_malloc()(size)
    #define SYMTAB_FREE(ptr)    get_current_free()(ptr)
#else
    #include <stdlib.h>
    #define SYMTAB_MALLOC(size) malloc(size)
    #define SYMTAB_FREE(ptr)    free(ptr)
#endif

#endif //SYMTAB_MEMORY_ALLOCATOR_H
