/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_white_box_tests_access.c
 * @ingroup logger_default_white_box_tests_access_group
 * @brief White-box test access implementation for the `logger_default` module.
 *
 * @details
 * Implements test-only helpers used to inspect private `logger_default_t`
 * state without exposing private implementation headers to test translation
 * units.
 */

#include "logger_default/tests/logger_default_white_box_tests_access.h"

#include "internal/logger_default_handle.h"

#include "policy/lexleo_assert.h"

stream_t *logger_default_get_stream(
	const logger_default_t *logger_default
) {
	LEXLEO_ASSERT(logger_default);
	return logger_default->stream;
}

const osal_time_ops_t *logger_default_get_time_ops(
	const logger_default_t *logger_default
) {
	LEXLEO_ASSERT(logger_default);
	return logger_default->time_ops;
}

const osal_mem_ops_t *logger_default_get_mem_ops(
	const logger_default_t *logger_default
) {
	LEXLEO_ASSERT(logger_default);
	return logger_default->mem_ops;
}

bool logger_default_get_append_newline_flag(
	const logger_default_t *logger_default
) {
	LEXLEO_ASSERT(logger_default);
	return logger_default->append_newline;
}
