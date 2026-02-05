/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_ctor.h
 * @ingroup fs_stream_internal_group
 * @brief Private factory constructor callback for the `fs_stream` adapter.
 *
 * @details
 * This constructor is stored in the adapter descriptor created by
 * `fs_stream_create_desc()` and later invoked by the `stream` port factory
 * when creating a file-backed stream instance.
 */

#ifndef LEXLEO_FS_STREAM_CTOR_H
#define LEXLEO_FS_STREAM_CTOR_H

#include "stream/borrowers/stream_types.h"

/**
 * @brief Factory constructor callback for the `fs_stream` adapter.
 *
 * @details
 * This function implements the `stream_ctor_fn_t` contract expected by the
 * `stream` port factory.
 *
 * The opaque user-data pointer `ud` is expected to reference a
 * `fs_stream_ctor_ud_t` structure containing the configuration and
 * environment required to construct the stream instance.
 *
 * The `args` parameter is the runtime argument blob supplied by the factory
 * when creating a stream instance. It is expected to point to a
 * `const fs_stream_args_t *` describing the file to open.
 *
 * @param[in] ud
 * Opaque user data stored in the adapter descriptor.
 *
 * Expected type:
 * - `const fs_stream_ctor_ud_t *`
 *
 * @param[in] args
 * Runtime argument blob supplied by the factory.
 *
 * Expected type:
 * - `const fs_stream_args_t *`
 *
 * @param[out] out
 * Receives the created stream handle.
 *
 * @return
 * - `STREAM_STATUS_OK` on success
 * - `STREAM_STATUS_INVALID` if one or more arguments are invalid
 * - `STREAM_STATUS_OOM` on allocation failure
 * - `STREAM_STATUS_IO_ERROR` if the underlying OSAL file operation fails
 *
 * @post
 * On success, `*out` receives a newly created stream instance owned by the
 * caller.
 *
 * On failure, `*out` is left unchanged.
 */
stream_status_t fs_stream_ctor(
	const void *ud,
	const void *args,
	stream_t **out);

#endif // LEXLEO_FS_STREAM_CTOR_H
