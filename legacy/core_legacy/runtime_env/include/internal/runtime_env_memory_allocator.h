// src/core/runtime_env/include/internal/runtime_env_memory_allocator.h

#ifndef LEXLEO_RUNTIME_ENV_MEMORY_ALLOCATOR_H
#define LEXLEO_RUNTIME_ENV_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
    #include "memory_allocator.h"
    #define RUNTIME_ENV_MALLOC(size) get_current_malloc()(size)
    #define RUNTIME_ENV_FREE(ptr)    get_current_free()(ptr)
#else
    #include <stdlib.h>
    #define RUNTIME_ENV_MALLOC(size) malloc(size)
    #define RUNTIME_ENV_FREE(ptr)    free(ptr)
#endif

#endif //LEXLEO_RUNTIME_ENV_MEMORY_ALLOCATOR_H