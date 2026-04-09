/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_handle.h
 * @ingroup stdio_stream_internal_group
 * @brief Private backend handle definition for the `stdio_stream` adapter.
 *
 * @details
 * This header exposes the private backend handle used internally by the
 * `stdio_stream` adapter implementation to bind a standard stream, its
 * injected stdio operations, and the memory operations required for
 * destruction.
 */

#ifndef LEXLEO_STDIO_STREAM_HANDLE_H
#define LEXLEO_STDIO_STREAM_HANDLE_H

#include "osal/stdio/osal_stdio_ops.h"

#include "osal/mem/osal_mem_ops.h"

/**
 * @brief Private backend handle for the `stdio_stream` adapter.
 *
 * @details
 * This structure stores:
 * - the borrowed OSAL stdio operations table used by the backend,
 * - the borrowed bound standard stream handle selected at creation time,
 * - the memory operations used for backend destruction.
 */
typedef struct stdio_stream_t {
	/** Borrowed OSAL stdio operations table used by the backend. */
	const osal_stdio_ops_t *stdio_ops;

	/** Borrowed bound standard stream handle wrapped by this backend. */
	OSAL_STDIO *stdio;

	/** Memory operations used to destroy the backend handle. */
	const osal_mem_ops_t *mem_ops;
} stdio_stream_t;

#endif // LEXLEO_STDIO_STREAM_HANDLE_H
