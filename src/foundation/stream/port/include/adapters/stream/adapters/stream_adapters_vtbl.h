/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_adapters_vtbl.h
 * @ingroup stream_adapters_api
 * @brief Adapter dispatch-table types for the `stream` port.
 *
 * @details
 * This header defines the backend operation callback types and the
 * @ref stream_vtbl_t dispatch table used to bind adapter-specific behavior
 * to a public `stream_t` handle.
 */

#ifndef LEXLEO_STREAM_ADAPTERS_VTBL_H
#define LEXLEO_STREAM_ADAPTERS_VTBL_H

#include "stream/borrowers/stream_borrowers_types.h"

#include "policy/lexleo_cstd_types.h"

/**
 * @brief Backend read operation for a stream adapter.
 *
 * @param[in] backend Adapter-owned backend instance bound to the stream.
 * @param[out] buf Destination buffer receiving up to `n` bytes.
 * @param[in] n Maximum number of bytes to read.
 * @param[out] st Optional status output.
 *
 * @return Number of bytes actually read.
 */
typedef size_t (*stream_read_fn_t)(
	void *backend,
	void *buf,
	size_t n,
	stream_status_t *st);

/**
 * @brief Backend write operation for a stream adapter.
 *
 * @param[in] backend Adapter-owned backend instance bound to the stream.
 * @param[in] buf Source buffer containing up to `n` bytes to write.
 * @param[in] n Maximum number of bytes to write.
 * @param[out] st Optional status output.
 *
 * @return Number of bytes actually written.
 */
typedef size_t (*stream_write_fn_t)(
	void *backend,
	const void *buf,
	size_t n,
	stream_status_t *st);

/**
 * @brief Backend flush operation for a stream adapter.
 *
 * @param[in] backend Adapter-owned backend instance bound to the stream.
 *
 * @return Operation status.
 */
typedef stream_status_t (*stream_flush_fn_t)(void *backend);

/**
 * @brief Backend close operation for a stream adapter.
 *
 * @param[in] backend Adapter-owned backend instance bound to the stream.
 *
 * @return Operation status.
 *
 * @details
 * Invoked by @ref stream_destroy() when releasing the stream handle.
 */
typedef stream_status_t (*stream_close_fn_t)(void *backend);

/**
 * @struct stream_vtbl_t
 * @brief Adapter dispatch table bound to a @ref stream_t instance.
 *
 * @details
 * Groups the backend operations used by the `stream` port.
 */
typedef struct stream_vtbl_t {

	/** Backend read operation. Must not be `NULL`. */
	stream_read_fn_t read;

	/** Backend write operation. Must not be `NULL`. */
	stream_write_fn_t write;

	/** Backend flush operation. Must not be `NULL`. */
	stream_flush_fn_t flush;

	/** Backend close operation. Must not be `NULL`. */
	stream_close_fn_t close;

} stream_vtbl_t;

#endif /* LEXLEO_STREAM_ADAPTERS_VTBL_H */
