/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_borrower.c
 * @ingroup logger_internal_group
 * @brief Borrower-facing runtime operation implementation for the `logger` port.
 */

#include "logger/borrowers/logger_borrowers_api.h"

#include "internal/logger_handle.h"

#include "policy/lexleo_assert.h"

logger_status_t logger_log(logger_t *l, const char *message)
{
	if (!l || !message) return LOGGER_STATUS_INVALID;

	LEXLEO_ASSERT(
		   l->backend
		&& l->vtbl
		&& l->vtbl->log
	);

	return l->vtbl->log(l->backend, message);
}
