// src/core/runtime_session/include/runtime_session_memory_allocator.h

#ifndef LEXLEO_RUNTIME_SESSION_MEMORY_ALLOCATOR_H
#define LEXLEO_RUNTIME_SESSION_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
	#include "memory_allocator.h"
	#define RUNTIME_SESSION_MALLOC(n)    ( get_current_malloc()((n)) )
	#define RUNTIME_SESSION_FREE(p)      ( get_current_free()((p)) )
	#define RUNTIME_SESSION_REALLOC(p,n) ( get_current_realloc()((p),(n)) )
#else
	#include <stdlib.h>
	#define RUNTIME_SESSION_MALLOC(n)    ( malloc((n)) )
	#define RUNTIME_SESSION_FREE(p)      ( free((p)) )
	#define RUNTIME_SESSION_REALLOC(p,n) ( realloc((p),(n)) )
#endif

#endif //LEXLEO_RUNTIME_SESSION_MEMORY_ALLOCATOR_H