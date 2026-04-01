/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_ctor.h
 * @ingroup dynamic_buffer_stream_internal_group
 * @brief Private factory constructor callback for the
 * `dynamic_buffer_stream` adapter.
 *
 * @details
 * This constructor is stored in the adapter descriptor created by
 * `dynamic_buffer_stream_create_desc()` and later invoked by the `stream`
 * port factory when creating a dynamic-buffer-backed stream instance.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_H

#include "stream/borrowers/stream_types.h"

/**
 * @brief Factory constructor callback for the `dynamic_buffer_stream` adapter.
 *
 * @details
 * This function implements the `stream_ctor_fn_t` contract expected by the
 * `stream` port factory.
 *
 * The opaque user-data pointer `ud` is expected to reference a
 * `dynamic_buffer_stream_ctor_ud_t` structure containing the configuration
 * and environment required to construct the stream instance.
 *
 * The `args` parameter is the runtime argument blob supplied by the factory
 * when creating a stream instance. This adapter does not require runtime
 * creation arguments, so `args` is expected to be `NULL`.
 *
 * @param[in] ud
 * Opaque user data stored in the adapter descriptor.
 *
 * Expected type:
 * - `const dynamic_buffer_stream_ctor_ud_t *`
 *
 * @param[in] args
 * Runtime argument blob supplied by the factory.
 *
 * Expected value:
 * - `NULL`
 *
 * @param[out] out
 * Receives the created stream handle.
 *
 * @return
 * - `STREAM_STATUS_OK` on success
 * - `STREAM_STATUS_INVALID` if one or more arguments are invalid
 * - `STREAM_STATUS_OOM` on allocation failure
 *
 * @post
 * On success, `*out` receives a newly created stream instance owned by the
 * caller.
 *
 * On failure, `*out` is left unchanged.
 */
stream_status_t dynamic_buffer_stream_ctor(
	const void *ud,
	const void *args,
	stream_t **out);

#endif // LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_H
