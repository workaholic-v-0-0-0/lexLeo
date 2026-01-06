// src/adapters/stream/stdio_stream/include/internal/stdio_stream_memory_allocator.h

#ifndef LEXLEO_STDIO_STREAM_MEMORY_ALLOCATOR_H
#define LEXLEO_STDIO_STREAM_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
	#include "memory_allocator.h"
	#define STDIO_STREAM_MALLOC(n)    ( get_current_malloc()((n)) )
	#define STDIO_STREAM_FREE(p)      ( get_current_free()((p)) )
	#define STDIO_STREAM_REALLOC(p,n) ( get_current_realloc()((p),(n)) )
#else
	#include <stdlib.h>
	#define STDIO_STREAM_MALLOC(n)    ( malloc((n)) )
	#define STDIO_STREAM_FREE(p)      ( free((p)) )
	#define STDIO_STREAM_REALLOC(p,n) ( realloc((p),(n)) )
#endif

#endif //LEXLEO_STDIO_STREAM_MEMORY_ALLOCATOR_H
