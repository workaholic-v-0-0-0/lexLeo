/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/adapters/stream/dynamic_buffer_stream/src/internal/
 * dynamic_buffer_stream_state.h
 *
 * Internal dynamic buffer stream backend structure.
 *
 * This header defines the runtime state carried by the
 * dynamic buffer stream.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_STATE_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_STATE_H

#include <stddef.h>
#include <stdbool.h>

#define DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY 256

typedef struct {
	char *buf;
	size_t cap;
	size_t len;
	size_t read_pos;
	bool autoclose;
} dynamic_buffer_t;

typedef struct dynamic_buffer_stream_state_t {
	dynamic_buffer_t dbuf;
} dynamic_buffer_stream_state_t;

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_STATE_H
