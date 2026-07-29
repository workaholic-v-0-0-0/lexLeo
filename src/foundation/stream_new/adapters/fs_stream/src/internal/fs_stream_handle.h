/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_handle.h
 * @ingroup fs_stream_internal_group
 * @brief Private backend handle for the `fs_stream` adapter.
 *
 * @details
 * This header defines the private file-backed backend handle used by the
 * `fs_stream` implementation.
 */

#ifndef LEXLEO_FS_STREAM_HANDLE_H
#define LEXLEO_FS_STREAM_HANDLE_H

#include "internal/fs_stream_handle.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/file/osal_file_ops.h"

/**
 * @brief Private backend handle for the `fs_stream` adapter.
 *
 * @details
 * Groups the runtime state and borrowed operation tables required by the
 * backend.
 */
typedef struct fs_stream_t {

	/** OSAL file operations used by the backend. */
	const osal_file_ops_t *file_ops;

	/** Memory operations used to destroy the backend handle. */
	const osal_mem_ops_t *mem_ops;

	/** OSAL file handle backing the stream. */
	OSAL_FILE *file;

} fs_stream_t;

#endif /* LEXLEO_FS_STREAM_HANDLE_H */
