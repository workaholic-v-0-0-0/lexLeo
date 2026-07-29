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

#include "stream/borrowers/stream_borrowers_types.h"

#include "osal/time/osal_time_ops.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

/**
 * @brief Private backend handle for the `logger_default` adapter.
 *
 * @details
 * This structure stores:
 * - the time operations used to obtain timestamps,
 * - the memory operations used for backend destruction.
 * - the adapter runtime behavior flags,
 * - the target stream used by the adapter,
 */
typedef struct logger_default_t {

	/** Borrowed time operations used to obtain timestamps for emitted log
	 * records. */
	const osal_time_ops_t *time_ops;

	/** Borrowed memory operations used to destroy the backend handle. */
	const osal_mem_ops_t *mem_ops;

	/** Whether the backend appends a trailing newline to emitted messages. */
	bool append_newline;

	/** Borrowed stream used by the backend. */
	stream_t *stream;

} logger_default_t;

#endif /* LEXLEO_LOGGER_DEFAULT_HANDLE_H */
