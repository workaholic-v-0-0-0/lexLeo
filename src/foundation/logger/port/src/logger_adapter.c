/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_adapter.c
 * @ingroup logger_internal_group
 * @brief Adapter-side logger construction implementation.
 */

#include "logger/adapters/logger_adapters_api.h"

#include "internal/logger_handle.h"

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_assert.h"

logger_env_t logger_default_env(
	const logger_vtbl_t *vtbl,
	const osal_mem_ops_t *mem_ops
) {
	return (logger_env_t) {
		.vtbl = vtbl,
		.mem = mem_ops };
}

logger_status_t logger_create(
	logger_t **out,
	const logger_env_t *env )
{
	LEXLEO_ASSERT(
		   out
		&& env
		&& env->mem
		&& env->mem->calloc
		&& env->vtbl
		&& env->vtbl->log
		&& env->vtbl->destroy
	);

	*out = NULL;

	logger_t *tmp = env->mem->calloc(1, sizeof(*tmp));
	if (!tmp)
		return LOGGER_STATUS_OOM;

	tmp->vtbl = env->vtbl;
	tmp->mem = env->mem;

	tmp->backend = NULL;

	*out = tmp;
	return LOGGER_STATUS_OK;
}

logger_status_t logger_complete_default_init(
	logger_t *logger,
	void *backend
) {
	LEXLEO_ASSERT(logger);

	if (!logger->backend) {
		LEXLEO_ASSERT(backend);
		logger->backend = backend;
	} else {
		LEXLEO_ASSERT(!backend);
	}

	return LOGGER_STATUS_OK;
}
