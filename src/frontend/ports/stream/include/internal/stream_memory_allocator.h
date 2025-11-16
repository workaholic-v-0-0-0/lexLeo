// src/frontend/ports/stream/include/internal/stream_memory_allocator.h

#ifndef LEXLEO_STREAM_MEMORY_ALLOCATOR_H
#define LEXLEO_STREAM_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
	#include "memory_allocator.h"
	#define STREAM_MALLOC(n)    ( get_current_malloc()((n)) )
	#define STREAM_FREE(p)      ( get_current_free()((p)) )
	#define STREAM_REALLOC(p,n) ( get_current_realloc()((p),(n)) )
#else
	#include <stdlib.h>
	#define STREAM_MALLOC(n)    ( malloc((n)) )
	#define STREAM_FREE(p)      ( free((p)) )
	#define STREAM_REALLOC(p,n) ( realloc((p),(n)) )
#endif

#endif //LEXLEO_STREAM_MEMORY_ALLOCATOR_H