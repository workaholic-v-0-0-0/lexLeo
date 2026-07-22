/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_owner.c
 * @ingroup logger_internal_group
 * @brief Owner-facing logger operation implementation.
 */

#include "logger/owners/logger_owners_api.h"

#include "internal/logger_handle.h"

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_assert.h"

logger_status_t logger_destroy(logger_t **l)
{
	if (!l || !*l) {
		return LOGGER_STATUS_OK;
	}

	logger_t *tmp = *l;

	LEXLEO_ASSERT(
		   tmp->vtbl->destroy
		&& tmp->mem
		&& tmp->mem->free
	);

	logger_status_t st = LOGGER_STATUS_OK;

	if (tmp->backend) {
		st = tmp->vtbl->destroy(tmp->backend);
	}

	tmp->mem->free(tmp);
	*l = NULL;

	return st;
}
