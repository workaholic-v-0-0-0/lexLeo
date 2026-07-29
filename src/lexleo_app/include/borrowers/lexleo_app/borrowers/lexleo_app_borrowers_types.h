/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_borrowers_types.h
 * @ingroup lexleo_app_borrowers_api
 * @brief Borrower-facing types for lexleo_app.
 *
 * @details
 * Declares the opaque application handle and runtime status codes.
 */

#ifndef LEXLEO_LEXLEO_APP_BORROWERS_TYPES_H
#define LEXLEO_LEXLEO_APP_BORROWERS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lexleo_app_t lexleo_app_t;

typedef enum lexleo_app_status_t {
	LEXLEO_APP_STATUS_OK,
	LEXLEO_APP_STATUS_LOGGER_ERROR,
	LEXLEO_APP_STATUS_VM_ERROR
	// TODO
} lexleo_app_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_APP_BORROWERS_TYPES_H */
