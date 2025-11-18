// src/adapters/stream/dynamic_buffer_stream/include/internal/dynamic_buffer_stream_internal.h

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_INTERNAL_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_INTERNAL_H

#include "dynamic_buffer_stream.h"
#include "internal/dynamic_buffer_stream_memory_allocator.h"

#include <stdbool.h>

#define DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY 256

typedef struct {
	char *buf;
	size_t cap;
	size_t len;
	size_t read_pos;
	bool autoclose;
} dynamic_buffer_stream_ctx;

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_INTERNAL_H