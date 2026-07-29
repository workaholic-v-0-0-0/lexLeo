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

#include "lexleo_app/common/lexleo_app_limits.h"
#include "lexleo_app/borrowers/lexleo_app_borrowers_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_APP_BORROWERS_API_H */
