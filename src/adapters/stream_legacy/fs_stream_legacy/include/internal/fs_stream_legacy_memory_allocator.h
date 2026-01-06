// src/adapters/stream/fs_stream/include/internal/fs_stream_memory_allocator.h

#ifndef LEXLEO_FS_STREAM_MEMORY_ALLOCATOR_H
#define LEXLEO_FS_STREAM_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
	#include "memory_allocator.h"
	#define FS_STREAM_MALLOC(n)    ( get_current_malloc()((n)) )
	#define FS_STREAM_FREE(p)      ( get_current_free()((p)) )
	#define FS_STREAM_REALLOC(p,n) ( get_current_realloc()((p),(n)) )
#else
	#include <stdlib.h>
	#define FS_STREAM_MALLOC(n)    ( malloc((n)) )
	#define FS_STREAM_FREE(p)      ( free((p)) )
	#define FS_STREAM_REALLOC(p,n) ( realloc((p),(n)) )
#endif

#endif //LEXLEO_FS_STREAM_MEMORY_ALLOCATOR_H