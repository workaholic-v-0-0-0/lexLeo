/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_state.h
 * @ingroup dynamic_buffer_stream_internal_group
 * @brief Private runtime state for the `dynamic_buffer_stream` backend.
 *
 * @details
 * This header defines the private runtime state stored by the
 * `dynamic_buffer_stream` backend handle.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_STATE_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_STATE_H

#include "policy/lexleo_cstd_types.h"

#define DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY 256

/**
 * @brief Private dynamic buffer state used by the
 * `dynamic_buffer_stream` backend.
 */
typedef struct dynamic_buffer_t {

	/** Buffer storage managed by the backend. */
	char *buf;

	/** Total allocated buffer capacity, in bytes. */
	size_t cap;

	/** Number of bytes currently stored in the buffer. */
	size_t len;

	/** Current read cursor inside the buffer. */
	size_t read_pos;

	/** Whether the buffer storage is released when the backend closes. */
	bool autoclose;

} dynamic_buffer_t;

/**
 * @brief Private runtime state for the `dynamic_buffer_stream` backend.
 */
typedef struct dynamic_buffer_stream_state_t {

	/** Dynamic buffer backing the stream instance. */
	dynamic_buffer_t dbuf;

} dynamic_buffer_stream_state_t;

#endif /* LEXLEO_DYNAMIC_BUFFER_STREAM_STATE_H */
