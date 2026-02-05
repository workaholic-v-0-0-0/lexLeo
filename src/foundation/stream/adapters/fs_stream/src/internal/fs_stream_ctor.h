/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_ctor.h
 * @ingroup fs_stream_internal_group
 * @brief Internal constructor callback used by the stream factory.
 *
 * @details
 * This constructor is stored in the adapter descriptor created by
 * fs_stream_create_desc() and later invoked by the stream port factory
 * when creating a file-backed stream instance.
 */

#ifndef LEXLEO_FS_STREAM_CTOR_H
#define LEXLEO_FS_STREAM_CTOR_H

#include "stream/borrowers/stream_types.h"

/**
 * @brief Factory constructor callback for the fs_stream adapter.
 *
 * This function implements the @ref stream_ctor_fn_t signature expected by
 * the stream port factory.
 *
 * The opaque user-data pointer (`ud`) is expected to reference a
 * @ref fs_stream_ctor_ud_t structure containing the configuration and
 * environment required to construct the stream instance.
 *
 * The `args` parameter is the runtime argument blob supplied by the factory
 * when creating a stream instance. It is expected to point to a
 * @ref fs_stream_args_t structure describing the file to open.
 *
 * @pre out  != NULL
 * @pre args != NULL
 * @pre ud   != NULL
 *
 * @param[in]  ud
 * Opaque user data stored in the adapter descriptor
 * (expected type: @ref fs_stream_ctor_ud_t).
 *
 * @param[in]  args
 * Runtime argument blob supplied by the factory
 * (expected type: `const fs_stream_args_t *`).
 *
 * @param[out] out
 * Pointer to the output stream instance.
 *
 * @post On success, `*out` receives a newly created stream instance
 *       owned by the caller.
 * @post On failure, `*out` is left unchanged.
 *
 * @retval STREAM_STATUS_OK
 * Stream successfully created.
 *
 * @retval STREAM_STATUS_INVALID
 * One or more arguments are invalid.
 *
 * @retval STREAM_STATUS_OOM
 * Memory allocation failed.
 *
 * @retval STREAM_STATUS_IO_ERROR
 * Underlying OSAL file operation failed.
 */
stream_status_t fs_stream_ctor(
	const void *ud,
	const void *args,
	stream_t **out );

#endif // LEXLEO_FS_STREAM_CTOR_H