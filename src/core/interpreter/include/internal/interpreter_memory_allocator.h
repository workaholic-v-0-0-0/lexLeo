// src/interpreter/include/internal/interpreter_memory_allocator.h

#ifndef LEXLEO_INTERPRETER_MEMORY_ALLOCATOR_H
#define LEXLEO_INTERPRETER_MEMORY_ALLOCATOR_H

#ifdef USE_MEMORY_ALLOCATOR
	#include "memory_allocator.h"
	#define INTERPRETER_MALLOC(size) get_current_malloc()(size)
	#define INTERPRETER_FREE(ptr)    get_current_free()(ptr)
#else
	#include <stdlib.h>
	#define INTERPRETER_MALLOC(size) malloc(size)
	#define INTERPRETER_FREE(ptr)    free(ptr)
#endif

#endif //LEXLEO_INTERPRETER_MEMORY_ALLOCATOR_H
