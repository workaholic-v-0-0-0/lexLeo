// src/core/ast/include/internal/ast_memory_allocator.h

#ifndef AST_MEMORY_ALLOCATOR_H
#define AST_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
    #include "memory_allocator.h"
    #define AST_MALLOC(size) get_current_malloc()(size)
    #define AST_FREE(ptr)    get_current_free()(ptr)
    #define AST_REALLOC(ptr,sz) get_current_realloc()(ptr, sz)
#else
    #include <stdlib.h>
    #define AST_MALLOC(size) malloc(size)
    #define AST_FREE(ptr)    free(ptr)
    #define AST_REALLOC(ptr,sz) realloc(ptr, sz)
#endif

#endif //AST_MEMORY_ALLOCATOR_H
