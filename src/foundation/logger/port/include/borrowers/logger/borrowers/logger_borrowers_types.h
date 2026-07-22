/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_borrowers_types.h
 * @ingroup logger_borrowers_api
 * @brief Borrower-facing types for the `logger` port.
 *
 * @details
 * This header declares the opaque @ref logger_t handle and the
 * @ref logger_status_t status type used by the public `logger` API.
 */

#ifndef LEXLEO_LOGGER_BORROWERS_TYPES_H
#define LEXLEO_LOGGER_BORROWERS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct logger_t
 * @brief Opaque logger handle.
 */
typedef struct logger_t logger_t;

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

#endif /* LEXLEO_LOGGER_BORROWERS_TYPES_H */
