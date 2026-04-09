/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger.c
 * @ingroup logger_internal_group
 * @brief Private implementation of the `logger` port.
 */

#include "internal/logger_handle.h"

#include "logger/borrowers/logger.h"
#include "logger/lifecycle/logger_lifecycle.h"
#include "logger/adapters/logger_adapters_api.h"
#include "logger/cr/logger_cr_api.h"

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_assert.h"

logger_env_t logger_default_env(const osal_mem_ops_t *mem_ops)
{
	return (logger_env_t) { .mem = mem_ops };
}

logger_status_t logger_create(
	logger_t **out,
	const logger_vtbl_t *vtbl,
	void *backend,
	const logger_env_t *env )
{
	if (
			   !out
			|| !vtbl
			|| !vtbl->log
			|| !vtbl->destroy
			|| !backend
			|| !env
			|| !env->mem )
		return LOGGER_STATUS_INVALID;

	LEXLEO_ASSERT(env->mem->calloc);

	logger_t *tmp = env->mem->calloc(1, sizeof(*tmp));
	if (!tmp)
		return LOGGER_STATUS_OOM;

	tmp->vtbl = *vtbl;
	tmp->backend = backend;
	tmp->mem = env->mem;

	*out = tmp;
	return LOGGER_STATUS_OK;
}

void logger_destroy(logger_t **l)
{
	if (!l || !*l)
		return;

	logger_t *logger = *l;
	*l = NULL;

	const osal_mem_ops_t *mem = logger->mem;
	void *backend = logger->backend;

	LEXLEO_ASSERT(logger->vtbl.destroy);

	logger->vtbl.destroy(backend);

	LEXLEO_ASSERT(mem && mem->free);

	mem->free(logger);
}

logger_status_t logger_log(logger_t *l, const char *message)
{
	if (!l || !message) return LOGGER_STATUS_INVALID;

	LEXLEO_ASSERT(l->backend);
	LEXLEO_ASSERT(l->vtbl.log);

	return l->vtbl.log(l->backend, message);
}
