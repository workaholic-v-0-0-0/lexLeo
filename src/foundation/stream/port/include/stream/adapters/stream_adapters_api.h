/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_adapters_api.h
 * @ingroup stream_adapters_api
 * @brief Adapter-side API for constructing and binding @ref stream_t objects.
 *
 * @details
 * This header exposes the adapter-facing contract used to bind a backend
 * implementation to the generic @ref stream_t port.
 *
 * It defines:
 * - backend operation function-pointer types,
 * - the @ref stream_vtbl_t dispatch table,
 * - the adapter-side constructor helper @ref stream_create().
 *
 * Typical usage:
 * - an adapter defines backend operations matching this contract,
 * - fills a @ref stream_vtbl_t,
 * - allocates or prepares its backend state,
 * - calls @ref stream_create() to build the public stream handle.
 */

#ifndef LEXLEO_STREAM_ADAPTERS_API_H
#define LEXLEO_STREAM_ADAPTERS_API_H

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_env.h"

#include <stddef.h>

/**
 * @brief Backend read operation for a stream adapter.
 *
 * @param[in] backend
 * Adapter-owned backend instance bound to the stream.
 *
 * @param[out] buf
 * Destination buffer receiving up to `n` bytes.
 *
 * @param[in] n
 * Maximum number of bytes to read.
 *
 * @param[out] st
 * Optional status output.
 *
 * @return
 * Number of bytes actually read.
 */
typedef size_t (*stream_read_fn_t)(
	void *backend,
	void *buf,
	size_t n,
	stream_status_t *st);

/**
 * @brief Backend write operation for a stream adapter.
 *
 * @param[in] backend
 * Adapter-owned backend instance bound to the stream.
 *
 * @param[in] buf
 * Source buffer containing up to `n` bytes to write.
 *
 * @param[in] n
 * Maximum number of bytes to write.
 *
 * @param[out] st
 * Optional status output.
 *
 * @return
 * Number of bytes actually written.
 */
typedef size_t (*stream_write_fn_t)(
	void *backend,
	const void *buf,
	size_t n,
	stream_status_t *st);

/**
 * @brief Backend flush operation for a stream adapter.
 *
 * @param[in] backend
 * Adapter-owned backend instance bound to the stream.
 *
 * @return
 * Operation status.
 */
typedef stream_status_t (*stream_flush_fn_t)(void *backend);

/**
 * @brief Backend close operation for a stream adapter.
 *
 * @param[in] backend
 * Adapter-owned backend instance bound to the stream.
 *
 * @return
 * Operation status.
 *
 * @details
 * This function is called by @ref stream_destroy() when destroying the
 * stream handle.
 */
typedef stream_status_t (*stream_close_fn_t)(void *backend);

/**
 * @struct stream_vtbl_t
 * @brief Adapter dispatch table bound to a @ref stream_t instance.
 *
 * @details
 * This table defines the concrete backend operations used by the generic
 * stream port for read, write, flush, and close.
 *
 * A valid vtable must provide all operations.
 */
typedef struct stream_vtbl_t {
	/** Backend read operation. Must not be NULL. */
	stream_read_fn_t read;

	/** Backend write operation. Must not be NULL. */
	stream_write_fn_t write;

	/** Backend flush operation. Must not be NULL. */
	stream_flush_fn_t flush;

	/** Backend close operation. Must not be NULL. */
	stream_close_fn_t close;
} stream_vtbl_t;

/**
 * @brief Create a generic stream handle from adapter-provided backend bindings.
 *
 * @details
 * This function is intended to be called by stream adapters to construct a
 * public @ref stream_t from:
 * - a dispatch table describing backend operations,
 * - an adapter-owned backend instance,
 * - an injected environment carrying runtime dependencies.
 *
 * The produced stream handle takes ownership of neither the vtable nor the
 * environment object itself; it only stores the required runtime references.
 * The backend lifetime and close semantics are governed by the adapter
 * contract, notably through `vtbl->close`.
 *
 * @param[out] out
 * Receives the created stream handle.
 * Must not be NULL.
 *
 * @param[in] vtbl
 * Backend dispatch table.
 * Must not be NULL and must provide non-NULL operations.
 *
 * @param[in] backend
 * Adapter-owned backend instance to bind to the stream.
 * May be NULL only if the resulting semantics are explicitly supported by the
 * caller, though normal adapters are expected to provide a valid backend.
 *
 * @param[in] env
 * Stream environment carrying injected runtime dependencies.
 * Must not be NULL.
 *
 * @return
 * - `STREAM_STATUS_OK` on success
 * - `STREAM_STATUS_INVALID` if arguments are invalid
 * - `STREAM_STATUS_OOM` on allocation failure
 *
 * @post
 * On success, `*out` contains a valid stream handle that must later be
 * destroyed via @ref stream_destroy().
 */
stream_status_t stream_create(
	stream_t **out,
	const stream_vtbl_t *vtbl,
	void *backend,
	const stream_env_t *env);

#endif // LEXLEO_STREAM_ADAPTERS_API_H