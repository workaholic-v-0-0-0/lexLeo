/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_factory_status_type.h
 * @ingroup stream_common_api
 * @brief Stream factory status type.
 *
 * @details
 * Declares the status codes returned by `stream` factory operations.
 */

#ifndef LEXLEO_STREAM_FACTORY_STATUS_TYPE_H
#define LEXLEO_STREAM_FACTORY_STATUS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stream factory status codes.
 */
typedef enum {

	/** Operation completed successfully. */
	STREAM_FACTORY_STATUS_OK = 0,

	/** End-of-stream was reached. */
	STREAM_FACTORY_STATUS_EOF,

	/** A caller-supplied argument was invalid. */
	STREAM_FACTORY_STATUS_INVALID,

	/** The requested resource could not be found. */
	STREAM_FACTORY_STATUS_NOT_FOUND,

	/** No backend is available for the requested operation. */
	STREAM_FACTORY_STATUS_NO_BACKEND,

	/** An input/output operation failed. */
	STREAM_FACTORY_STATUS_IO_ERROR,

	/** Memory allocation failed. */
	STREAM_FACTORY_STATUS_OOM,

	/** A registration attempt failed because the item already exists. */
	STREAM_FACTORY_STATUS_ALREADY_EXISTS,

	/** A bounded container or registry is full. */
	STREAM_FACTORY_STATUS_FULL

} stream_factory_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_FACTORY_STATUS_TYPE_H */
