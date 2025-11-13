// src/io/input_provider/include/internal/input_provider_memory_allocator.h

#ifndef LEXLEO_INPUT_PROVIDER_MEMORY_ALLOCATOR_H
#define LEXLEO_INPUT_PROVIDER_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
  #include "memory_allocator.h"
  #define INPUT_PROVIDER_MALLOC(n)    ( get_current_malloc()((n)) )
  #define INPUT_PROVIDER_FREE(p)      ( get_current_free()((p)) )
  #define INPUT_PROVIDER_REALLOC(p,n) ( get_current_realloc()((p),(n)) )
#else
  #include <stdlib.h>
  #define INPUT_PROVIDER_MALLOC(n)    ( malloc((n)) )
  #define INPUT_PROVIDER_FREE(p)      ( free((p)) )
  #define INPUT_PROVIDER_REALLOC(p,n) ( realloc((p),(n)) )
#endif

#endif //LEXLEO_INPUT_PROVIDER_MEMORY_ALLOCATOR_H
