/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_status_type.h
 * @ingroup logger_common_api
 * @brief Status type for the `logger` port.
 *
 * @details
 * Declares the status values returned by `logger` operations.
 */

#ifndef LEXLEO_LOGGER_STATUS_TYPE_H
#define LEXLEO_LOGGER_STATUS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Status returned by `logger` operations.
 */
typedef enum logger_status_t {

	/** Operation completed successfully. */
	LOGGER_STATUS_OK = 0,

	/** An invalid argument was provided. */
	LOGGER_STATUS_INVALID,

	/** An input/output operation failed. */
	LOGGER_STATUS_IO_ERROR,

	/** No backend is attached to the logger handle. */
	LOGGER_STATUS_NO_BACKEND,

	/** A memory allocation failed. */
	LOGGER_STATUS_OOM

} logger_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_STATUS_TYPE_H */
