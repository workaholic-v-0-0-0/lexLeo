/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/dynamic_buffer_stream/src/internal/
 * dynamic_buffer_stream_backend.h
 *
 * Internal dynamic buffer stream backend structure.
 *
 * This header defines the concrete backend state used by the
 * dynamic_buffer-based stream implementation.
 *
 * Not part of the public API.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_BACKEND_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_BACKEND_H

#include "internal/dynamic_buffer_stream_state.h"
#include "osal_mem_ops.h"

typedef struct dynamic_buffer_stream_t {
	dynamic_buffer_stream_state_t state;
	const struct osal_mem_ops_t *mem;
} dynamic_buffer_stream_t;

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_BACKEND_H
