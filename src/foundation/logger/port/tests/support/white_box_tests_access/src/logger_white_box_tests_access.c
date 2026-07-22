/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_white_box_tests_access.c
 * @ingroup logger_white_box_tests_access_group
 * @brief White-box test access helper implementation for the logger module.
 *
 * @details
 * This file implements the test-only helpers used to inject and observe
 * selected internal owned-resource fields of logger handles.
 */

#include "logger/tests/logger_white_box_tests_access.h"

#include "internal/logger_handle.h"

#include "policy/lexleo_assert.h"

void logger_inject_backend(logger_t *logger, void *backend)
{
	LEXLEO_ASSERT(logger);
	logger->backend = backend;
}

void *logger_get_backend(const logger_t *logger)
{
	LEXLEO_ASSERT(logger);
	return logger->backend;
}
