/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_white_box_tests_access.c
 * @ingroup stdio_stream_white_box_tests_access_group
 * @brief White-box test access implementation for the `stdio_stream` module.
 *
 * @details
 * Implements test-only helpers used to inspect and modify the private
 * `stdio_stream_t` external borrowed dependencies and internal borrowed
 * dependency through the white-box test API.
 */

#include "stdio_stream/tests/stdio_stream_white_box_tests_access.h"

#include "internal/stdio_stream_handle.h"

#include "policy/lexleo_assert.h"

void stdio_stream_inject_stdio_ops(
	stdio_stream_t *stdio_stream,
	const osal_stdio_ops_t *stdio_ops
) {
	LEXLEO_ASSERT(stdio_stream);
	stdio_stream->stdio_ops = stdio_ops;
}

const osal_stdio_ops_t *stdio_stream_get_stdio_ops(
	const stdio_stream_t *stdio_stream
) {
	LEXLEO_ASSERT(stdio_stream);
	return stdio_stream->stdio_ops;
}

void stdio_stream_inject_mem_ops(
	stdio_stream_t *stdio_stream,
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(stdio_stream);
	stdio_stream->mem_ops = mem_ops;
}

const osal_mem_ops_t *stdio_stream_get_mem_ops(
	const stdio_stream_t *stdio_stream
) {
	LEXLEO_ASSERT(stdio_stream);
	return stdio_stream->mem_ops;
}

void stdio_stream_inject_stdio(
	stdio_stream_t *stdio_stream,
	OSAL_STDIO *stdio
) {
	LEXLEO_ASSERT(stdio_stream);
	stdio_stream->stdio = stdio;
}

OSAL_STDIO *stdio_stream_get_stdio(
	stdio_stream_t *stdio_stream
) {
	LEXLEO_ASSERT(stdio_stream);
	return stdio_stream->stdio;
}
