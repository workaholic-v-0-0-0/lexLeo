/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_white_box_tests_access.c
 * @ingroup logger_white_box_tests_access_group
 * @brief White-box test access implementation for the `logger` module.
 *
 * @details
 * Implements test-only helpers used to inspect and modify private logger
 * state and to expose selected internal operations through the white-box
 * test API.
 */

#include "logger/tests/logger_white_box_tests_access.h"

#include "internal/logger_handle.h"

#include "policy/lexleo_assert.h"

void logger_inject_backend(logger_t *logger, void *backend)
{
	LEXLEO_ASSERT(logger);
	logger->backend = backend;
}

const logger_vtbl_t *logger_get_vtbl(const logger_t *logger)
{
	LEXLEO_ASSERT(logger);
	return logger->vtbl;
}

const osal_mem_ops_t *logger_get_mem_ops(const logger_t *logger)
{
	LEXLEO_ASSERT(logger);
	return logger->mem_ops;
}

void *logger_get_backend(const logger_t *logger)
{
	LEXLEO_ASSERT(logger);
	return logger->backend;
}
