/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_handle.h
 * @ingroup stdio_stream_internal_group
 * @brief Private backend handle definition for the `stdio_stream` adapter.
 *
 * @details
 * This header defines the private standard-stream-backed backend handle used
 * by the `stdio_stream` implementation.
 */

#ifndef LEXLEO_STDIO_STREAM_HANDLE_H
#define LEXLEO_STDIO_STREAM_HANDLE_H

#include "osal/stdio/osal_stdio_ops.h"

#include "osal/mem/osal_mem_ops.h"

/**
 * @brief Private backend handle for the `stdio_stream` adapter.
 *
 * @details
 * Groups the external borrowed dependencies and internal borrowed dependency
 * required by the backend.
 */
typedef struct stdio_stream_t {

	/** External borrowed OSAL stdio operations table used by the backend. */
	const osal_stdio_ops_t *stdio_ops;

	/** External borrowed memory operations used to destroy the backend handle. */
	const osal_mem_ops_t *mem_ops;

	/** Internal borrowed standard stream handle wrapped by this backend. */
	OSAL_STDIO *stdio;

} stdio_stream_t;

#endif /* LEXLEO_STDIO_STREAM_HANDLE_H */
