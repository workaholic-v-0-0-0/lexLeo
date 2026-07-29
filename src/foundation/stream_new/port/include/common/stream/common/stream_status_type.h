/* SPDX-License-Identifier: GPL-3.0-or-later
* Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_status_type.h
 * @ingroup stream_common_api
 * @brief Status codes for the `stream` port.
 *
 * @details
 * Declares the status codes shared by the `stream` port APIs and
 * adapter implementations.
 */

#ifndef LEXLEO_STREAM_STATUS_TYPE_H
#define LEXLEO_STREAM_STATUS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stream status type.
 */
typedef enum {

	/** Operation completed successfully. */
	STREAM_STATUS_OK = 0,

	/** End-of-stream was reached. */
	STREAM_STATUS_EOF,

	/** A caller-supplied argument was invalid. */
	STREAM_STATUS_INVALID,

	/** The requested resource could not be found. */
	STREAM_STATUS_NOT_FOUND,

	/** No backend is available for the requested operation. */
	STREAM_STATUS_NO_BACKEND,

	/** An input/output operation failed. */
	STREAM_STATUS_IO_ERROR,

	/** Memory allocation failed. */
	STREAM_STATUS_OOM

} stream_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_STATUS_TYPE_H */
