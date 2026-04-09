/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_handle.h
 * @ingroup fs_stream_internal_group
 * @brief Private backend handle definition for the `fs_stream` adapter.
 *
 * @details
 * This header exposes the private in-memory backend handle used by the
 * `fs_stream` adapter implementation.
 */

#ifndef LEXLEO_FS_STREAM_HANDLE_H
#define LEXLEO_FS_STREAM_HANDLE_H

#include "internal/fs_stream_state.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/file/osal_file_ops.h"

/**
 * @brief Private backend handle for the `fs_stream` adapter.
 *
 * @details
 * This structure stores:
 * - the backend runtime state,
 * - the memory operations used for backend destruction,
 * - the OSAL file operations used by the adapter.
 */
typedef struct fs_stream_t {
	/** Backend runtime state. */
	fs_stream_state_t state;

	/** Memory operations used to destroy the backend handle. */
	const osal_mem_ops_t *mem_ops;

	/** OSAL file operations used by the backend. */
	const osal_file_ops_t *file_ops;
} fs_stream_t;

#endif // LEXLEO_FS_STREAM_HANDLE_H
