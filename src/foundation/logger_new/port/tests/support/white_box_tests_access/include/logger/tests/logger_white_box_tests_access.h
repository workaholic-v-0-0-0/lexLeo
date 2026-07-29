/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_white_box_tests_access.h
 * @ingroup logger_white_box_tests_access_group
 * @brief Privileged white-box access to internal `logger` state and
 * operations for unit tests.
 *
 * @details
 * Declares test-only helpers used to inspect and modify private `logger_t`
 * state and to access selected internal logger operations without exposing
 * private implementation headers to test translation units.
 */

#ifndef LOGGER_WHITE_BOX_TESTS_ACCESS_H
#define LOGGER_WHITE_BOX_TESTS_ACCESS_H

#include "logger/common/logger_opaque_type.h"
#include "logger/common/logger_vtbl_type.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inject an adapter backend into a logger handle.
 *
 * @param[in,out] logger Logger handle to modify.
 * @param[in,out] backend Adapter backend to inject.
 */
void logger_inject_backend(
	logger_t *logger,
	void *backend
);

/**
 * @brief Return the dispatch table bound to a logger handle.
 *
 * @param[in] logger Logger handle to inspect.
 *
 * @return Bound logger adapter dispatch table.
 */
const logger_vtbl_t *logger_get_vtbl(
	const logger_t *logger
);

/**
 * @brief Return the memory operations bound to a logger handle.
 *
 * @param[in] logger Logger handle to inspect.
 *
 * @return Bound memory operations.
 */
const osal_mem_ops_t *logger_get_mem_ops(
	const logger_t *logger
);

/**
 * @brief Return the backend bound to a logger handle.
 *
 * @param[in] logger Logger handle to inspect.
 *
 * @return Bound adapter backend.
 */
void *logger_get_backend(
	const logger_t *logger
);

#ifdef __cplusplus
}
#endif

#endif /* LOGGER_WHITE_BOX_TESTS_ACCESS_H */
