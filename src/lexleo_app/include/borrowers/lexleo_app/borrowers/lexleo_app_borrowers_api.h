/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_borrowers_api.h
 * @ingroup lexleo_app_borrowers_api
 * @brief Borrower-facing runtime API for lexleo_app.
 *
 * @details
 * Declares runtime operations for code that borrows a fully initialized
 * LexLeo application handle.
 */

#ifndef LEXLEO_LEXLEO_APP_BORROWERS_API_H
#define LEXLEO_LEXLEO_APP_BORROWERS_API_H

#include "lexleo_app/borrowers/lexleo_app_borrowers_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEXLEO_APP_MAX_LINE_SIZE 4096

/**
 * @brief Run the LexLeo application.
 *
 * @param[in,out] app Fully initialized application handle.
 *
 * @return Execution status.
 *
 * See contract:
 * - @ref specifications_lexleo_app_run
 */
lexleo_app_status_t lexleo_app_run(lexleo_app_t *app);

//<here>
lexleo_app_status_t lexleo_app_read_input_line(
	lexleo_app_t *app,
	char *message,
	size_t capacity);

lexleo_app_status_t lexleo_app_write_output(
	lexleo_app_t *app,
	const char *message);

lexleo_app_status_t lexleo_app_write_error(
	lexleo_app_t *app,
	const char *message);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_APP_BORROWERS_API_H */
