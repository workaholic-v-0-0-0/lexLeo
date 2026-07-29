/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_borrower.c
 * @ingroup logger_borrowers_api
 * @brief Borrower-facing logger operations.
 *
 * @details
 * Implements the runtime operations available to modules that borrow a
 * `logger` handle.
 *
 * See also:
 * - @ref specifications_logger_log "logger_log() specifications"
 */

#include "logger/borrowers/logger_borrowers_api.h"

#include "internal/logger_handle.h"

#include "policy/lexleo_assert.h"

logger_status_t logger_log(logger_t *logger, const char *message)
{
	if (!logger || !message) {
		return LOGGER_STATUS_INVALID;
	}

	LEXLEO_ASSERT(logger->vtbl && logger->vtbl->log && logger->backend);

	return logger->vtbl->log(logger->backend, message);
}
