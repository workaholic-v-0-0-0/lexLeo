// src/adapters/stream/dynamic_buffer_stream/include/internal/dynamic_buffer_stream_memory_allocator.h

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_MEMORY_ALLOCATOR_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
	#include "memory_allocator.h"
	#define DYNAMIC_BUFFER_STREAM_MALLOC(n)    ( get_current_malloc()((n)) )
	#define DYNAMIC_BUFFER_STREAM_FREE(p)      ( get_current_free()((p)) )
	#define DYNAMIC_BUFFER_STREAM_REALLOC(p,n) ( get_current_realloc()((p),(n)) )
#else
	#include <stdlib.h>
	#define DYNAMIC_BUFFER_STREAM_MALLOC(n)    ( malloc((n)) )
	#define DYNAMIC_BUFFER_STREAM_FREE(p)      ( free((p)) )
	#define DYNAMIC_BUFFER_STREAM_REALLOC(p,n) ( realloc((p),(n)) )
#endif

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_MEMORY_ALLOCATOR_H