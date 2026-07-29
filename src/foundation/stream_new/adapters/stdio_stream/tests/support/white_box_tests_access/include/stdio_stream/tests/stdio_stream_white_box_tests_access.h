/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_white_box_tests_access.h
 * @ingroup stdio_stream_white_box_tests_access_group
 * @brief Privileged white-box access to internal `stdio_stream` state and
 * operations for unit tests.
 *
 * @details
 * Declares test-only helpers used to inspect and modify the private
 * `stdio_stream_t` external borrowed dependencies and internal borrowed
 * dependency, and to access selected internal `stdio_stream` operations
 * without exposing private implementation headers to test translation units.
 */

#ifndef STREAM_WHITE_BOX_TESTS_ACCESS_H
#define STREAM_WHITE_BOX_TESTS_ACCESS_H

#include "stdio_stream/cr/stdio_stream_cr_types.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/stdio/osal_stdio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void stdio_stream_inject_stdio_ops(
	stdio_stream_t *stdio_stream,
	const osal_stdio_ops_t *stdio_ops
);

const osal_stdio_ops_t *stdio_stream_get_stdio_ops(
	const stdio_stream_t *stdio_stream
);

void stdio_stream_inject_mem_ops(
	stdio_stream_t *stdio_stream,
	const osal_mem_ops_t *mem_ops
);

const osal_mem_ops_t *stdio_stream_get_mem_ops(
	const stdio_stream_t *stdio_stream
);

void stdio_stream_inject_stdio(
	stdio_stream_t *stdio_stream,
	OSAL_STDIO *stdio
);

OSAL_STDIO *stdio_stream_get_stdio(stdio_stream_t *stdio_stream);

#ifdef __cplusplus
}
#endif

#endif /* STREAM_WHITE_BOX_TESTS_ACCESS_H */
