// src/io/input_provider/include/internal/input_provider_internal.h

#ifndef LEXLEO_INPUT_PROVIDER_INTERNAL_H
#define LEXLEO_INPUT_PROVIDER_INTERNAL_H

#include "input_provider.h"
#include "internal/input_provider_memory_allocator.h"
#include "input_provider_ctx.h"

#define INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER 256

typedef struct input_provider_dynamic_buffer {
	char *buf;
	size_t cap;
	size_t len;
} input_provider_dynamic_buffer;

struct input_provider {
	input_provider_mode mode;
	FILE *file;
	input_provider_dynamic_buffer dbuf;

	// lexer_scanner borrowed from lexer ;
	// if mode == INPUT_PROVIDER_MODE_CHUNKS, owns its YY_BUFFER_STATE field ;
	// if mode == INPUT_PROVIDER_MODE_FILE, doesn't use its YY_BUFFER_STATE
	//      field at all and hence doesn't own its YY_BUFFER_STATE field
	yyscan_t lexer_scanner;

	YY_BUFFER_STATE lexer_buffer_state;
};

#endif //LEXLEO_INPUT_PROVIDER_INTERNAL_H
