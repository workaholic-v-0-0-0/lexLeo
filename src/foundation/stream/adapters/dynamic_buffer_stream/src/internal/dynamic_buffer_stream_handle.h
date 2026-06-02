/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_handle.h
 * @ingroup dynamic_buffer_stream_internal_group
 * @brief Private backend handle for the `dynamic_buffer_stream` adapter.
 *
 * @details
 * This header defines the private in-memory backend handle used by the
 * `dynamic_buffer_stream` implementation.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_HANDLE_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_HANDLE_H

#include "internal/dynamic_buffer_stream_state.h"

#include "osal/mem/osal_mem_ops.h"

/**
 * @brief Private backend handle for the `dynamic_buffer_stream` adapter.
 *
 * @details
 * Groups the runtime state and borrowed operation tables required by the
 * backend.
 */
typedef struct dynamic_buffer_stream_t {

	/** Backend runtime state. */
	dynamic_buffer_stream_state_t state;

	/** Borrowed memory operations used by the backend. */
	const osal_mem_ops_t *mem;

} dynamic_buffer_stream_t;

#endif /* LEXLEO_DYNAMIC_BUFFER_STREAM_HANDLE_H */
