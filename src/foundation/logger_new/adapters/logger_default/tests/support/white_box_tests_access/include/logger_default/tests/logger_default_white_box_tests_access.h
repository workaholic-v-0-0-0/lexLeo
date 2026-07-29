/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_white_box_tests_access.h
 * @ingroup logger_default_white_box_tests_access_group
 * @brief Privileged white-box access to internal `logger_default` state and
 * operations for unit tests.
 *
 * @details
 * Declares test-only helpers used to inspect private `logger_default_t`
 * state without exposing private implementation headers to test translation
 * units.
 */

#ifndef LEXLEO_LOGGER_DEFAULT_WHITE_BOX_TESTS_ACCESS_H
#define LEXLEO_LOGGER_DEFAULT_WHITE_BOX_TESTS_ACCESS_H

#include "logger_default/cr/logger_default_cr_types.h"

#include "stream/common/stream_opaque_type.h"

#include "osal/time/osal_time_ops.h"
#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

stream_t *logger_default_get_stream(
	const logger_default_t *logger_default);

const osal_time_ops_t *logger_default_get_time_ops(
	const logger_default_t *logger_default
);

const osal_mem_ops_t *logger_default_get_mem_ops(
	const logger_default_t *logger_default
);

bool logger_default_get_append_newline_flag(
	const logger_default_t *logger_default);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_DEFAULT_WHITE_BOX_TESTS_ACCESS_H */
