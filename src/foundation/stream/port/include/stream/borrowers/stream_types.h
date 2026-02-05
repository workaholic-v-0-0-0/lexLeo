/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_types.h
 * @ingroup stream_borrowers_api
 * @brief Borrower-visible public types for the `stream` port.
 *
 * @details
 * This header exposes the public opaque handle, status codes, and borrower
 * operation table used by the `stream` port.
 */

#ifndef LEXLEO_STREAM_TYPES_H
#define LEXLEO_STREAM_TYPES_H

#include <stddef.h>

/**
 * @brief Opaque public stream handle type.
 *
 * @details
 * A `stream_t` represents a runtime stream instance manipulated through the
 * public borrower and lifecycle APIs.
 */
typedef struct stream_t stream_t;

/**
 * @brief Public status codes used by the `stream` port.
 *
 * @details
 * These values report the outcome of stream operations across borrower,
 * adapter, lifecycle, and factory-related APIs.
 */
typedef enum {
	/** Operation completed successfully. */
	STREAM_STATUS_OK = 0,

	/** End-of-stream was reached. */
	STREAM_STATUS_EOF,

	/** An argument or precondition was invalid. */
	STREAM_STATUS_INVALID,

	/** An I/O operation failed. */
	STREAM_STATUS_IO_ERROR,

	/** No backend is bound to the stream or no backend could be resolved. */
	STREAM_STATUS_NO_BACKEND,

	/** Memory allocation failed. */
	STREAM_STATUS_OOM,

	/** A requested item could not be found. */
	STREAM_STATUS_NOT_FOUND,

	/** A registration attempt failed because the item already exists. */
	STREAM_STATUS_ALREADY_EXISTS,

	/** A bounded container or registry is full. */
	STREAM_STATUS_FULL
} stream_status_t;

/**
 * @brief Borrower-facing operation table for the `stream` port.
 *
 * @details
 * A `stream_ops_t` groups the default borrower-visible runtime operations
 * exposed by the public `stream` port.
 */
typedef struct stream_ops_t {
	/**
	 * @brief Read bytes from a stream.
	 *
	 * @param[in] s
	 * Stream handle to read from.
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
	size_t (*read)(stream_t *s, void *buf, size_t n, stream_status_t *st);

	/**
	 * @brief Write bytes to a stream.
	 *
	 * @param[in] s
	 * Stream handle to write to.
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
	size_t (*write)(stream_t *s, const void *buf, size_t n, stream_status_t *st);

	/**
	 * @brief Flush a stream.
	 *
	 * @param[in] s
	 * Stream handle to flush.
	 *
	 * @return
	 * Operation status.
	 */
	stream_status_t (*flush)(stream_t *s);
} stream_ops_t;

#endif // LEXLEO_STREAM_TYPES_H
