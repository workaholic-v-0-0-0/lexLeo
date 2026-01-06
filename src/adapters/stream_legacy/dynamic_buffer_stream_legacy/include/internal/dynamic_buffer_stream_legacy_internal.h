// src/adapters/stream_legacy/dynamic_buffer_stream_legacy/include/internal/dynamic_buffer_stream_legacy_internal.h

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_INTERNAL_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_INTERNAL_H

#include "dynamic_buffer_stream_legacy.h"
#include "internal/dynamic_buffer_stream_legacy_memory_allocator.h"

#include <stdbool.h>

#define DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY 256

typedef struct {
	char *buf;
	size_t cap;
	size_t len;
	size_t read_pos;
	bool autoclose;
} dynamic_buffer_stream_legacy_ctx;

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_INTERNAL_H
