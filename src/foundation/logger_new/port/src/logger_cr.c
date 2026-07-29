/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_cr.c
 * @ingroup logger_internal_group
 * @brief Composition Root implementation for the `logger` port.
 *
 * @details
 * Implements the CR-facing operations used to build default logger
 * environments, create and complete logger handles, and destroy them.
 */

#include "logger/cr/logger_cr_api.h"

#include "internal/logger_handle.h"

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_assert.h"

logger_env_t logger_default_env(
	const logger_vtbl_t *vtbl,
	const osal_mem_ops_t *mem_ops
) {
	return (logger_env_t){ .vtbl = vtbl, .mem_ops = mem_ops };
}

logger_status_t logger_create(
	logger_t **out,
	const logger_env_t *env
) {
	LEXLEO_ASSERT(out && env && env->mem_ops && env->mem_ops->calloc);

	logger_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return LOGGER_STATUS_OOM;
	}

	tmp->vtbl = env->vtbl;
	tmp->mem_ops = env->mem_ops;

	*out = tmp;
	return LOGGER_STATUS_OK;
}

logger_status_t logger_complete_default_init(
	logger_t *logger,
	void *backend
) {
	LEXLEO_ASSERT(
		   logger
		&& (
			   !logger->backend && backend
			|| logger->backend && !backend
		)
	);

	if (!logger->backend) {
		logger->backend = backend;
	}

	return LOGGER_STATUS_OK;
}

logger_status_t logger_destroy(logger_t **logger)
{
	if (!logger || !*logger) {
		return LOGGER_STATUS_OK;
	}

	LEXLEO_ASSERT(
		   (*logger)->mem_ops
		&& (*logger)->mem_ops->free
		&& (*logger)->vtbl
		&& (*logger)->vtbl->destroy
	);

	logger_status_t logger_status = LOGGER_STATUS_OK;
	if ((*logger)->backend) {
		logger_status = (*logger)->vtbl->destroy((*logger)->backend);
	}

	(*logger)->mem_ops->free(*logger);
	*logger = NULL;

	return logger_status;
}
