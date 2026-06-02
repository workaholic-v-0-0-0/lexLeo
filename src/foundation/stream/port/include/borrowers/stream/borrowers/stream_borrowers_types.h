/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_borrowers_types.h
 * @ingroup stream_borrowers_api
 * @brief Borrower-visible types for the `stream` port.
 *
 * @details
 * This header defines the public opaque handle and status codes used by the
 * `stream` port borrower API.
 */

#ifndef LEXLEO_STREAM_BORROWERS_TYPES_H
#define LEXLEO_STREAM_BORROWERS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque stream handle type.
 */
typedef struct stream_t stream_t;

/**
 * @brief Public status codes used by the `stream` port.
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

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_BORROWERS_TYPES_H */
