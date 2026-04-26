/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_handle.h
 * @ingroup logger_default_internal_group
 * @brief Private backend handle definition for the `logger_default` adapter.
 *
 * @details
 * This header exposes the private in-memory backend handle used by the
 * `logger_default` adapter implementation.
 */

#ifndef LEXLEO_LOGGER_DEFAULT_HANDLE_H
#define LEXLEO_LOGGER_DEFAULT_HANDLE_H

#include "stream/borrowers/stream_types.h"

#include "osal/time/osal_time_ops.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

/**
 * @brief Private backend handle for the `logger_default` adapter.
 *
 * @details
 * This structure stores:
 * - the target stream used by the adapter,
 * - the time operations used to obtain timestamps,
 * - the string operations used by the backend,
 * - the adapter runtime behavior flags,
 * - the memory operations used for backend destruction.
 */
typedef struct logger_default_t {
	/** Target stream used by the backend. */
	stream_t *stream;

	/** Time operations used to obtain timestamps for emitted log records. */
	const osal_time_ops_t *time_ops;

	/** String operations used by the backend implementation. */
	const osal_str_ops_t *str_ops;

	/** Whether the backend appends a trailing newline to emitted messages. */
	bool append_newline;

	/** Memory operations used to destroy the backend handle. */
	const osal_mem_ops_t *mem;
} logger_default_t;

#endif // LEXLEO_LOGGER_DEFAULT_HANDLE_H
