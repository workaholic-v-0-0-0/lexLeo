/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_borrower.c
 * @ingroup lexleo_app_internal_group
 * @brief Borrower-facing runtime implementation for lexleo_app.
 *
 * @details
 * Implements the application runtime entry point.
 * Execution behavior remains to be implemented.
 */

#include "lexleo_app/borrowers/lexleo_app_borrowers_api.h"

#include "internal/lexleo_app_handle.h"

#include "logger/borrowers/logger_borrowers_api.h"
#include "lexleo_vm/borrowers/lexleo_vm_borrowers_api.h"

#include "policy/lexleo_assert.h"

lexleo_app_status_t lexleo_app_run(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);

	logger_status_t logger_status =
		logger_log(
			app->logger,"LexLeo application started."
		);
	if (logger_status != LOGGER_STATUS_OK) {
		return LEXLEO_APP_STATUS_LOGGER_ERROR;
	}

	if (lexleo_vm_run(app->vm) != LEXLEO_VM_STATUS_OK) {
		return LEXLEO_APP_STATUS_VM_ERROR;
	}

	logger_status =
		logger_log(
			app->logger,"LexLeo application shutting down."
		);
	if (logger_status != LOGGER_STATUS_OK) {
		return LEXLEO_APP_STATUS_LOGGER_ERROR;
	}

	return LEXLEO_APP_STATUS_OK;
}
