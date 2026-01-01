// src/lexer/src/lexer_memory_allocator.h

#ifndef LEXLEO_LEXER_MEMORY_ALLOCATOR_H
#define LEXLEO_LEXER_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
	#include "memory_allocator.h"
	#define LEXER_MALLOC(size) get_current_malloc()(size)
	#define LEXER_FREE(ptr)    get_current_free()(ptr)
#else
	#include <stdlib.h>
	#define LEXER_MALLOC(size) malloc(size)
	#define LEXER_FREE(ptr)    free(ptr)
#endif

#endif //LEXLEO_LEXER_MEMORY_ALLOCATOR_H