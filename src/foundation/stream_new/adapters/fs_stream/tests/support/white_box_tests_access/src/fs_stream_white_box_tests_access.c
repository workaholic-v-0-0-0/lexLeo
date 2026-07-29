/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_white_box_tests_access.c
 * @ingroup fs_stream_white_box_tests_access_group
 * @brief White-box test access implementation for the `fs_stream` module.
 *
 * @details
 * Implements test-only helpers used to inspect and modify private fs_stream
 * state and to expose selected internal operations through the white-box
 * test API.
 */

#include "fs_stream/tests/fs_stream_white_box_tests_access.h"

#include "internal/fs_stream_handle.h"

#include "policy/lexleo_assert.h"

void fs_stream_inject_file_ops(
	fs_stream_t *fs_stream,
	const osal_file_ops_t *file_ops
) {
	LEXLEO_ASSERT(fs_stream);
	fs_stream->file_ops = file_ops;
}

const osal_file_ops_t *fs_stream_get_file_ops(
	const fs_stream_t *fs_stream
) {
	LEXLEO_ASSERT(fs_stream);
	return fs_stream->file_ops;
}

void fs_stream_inject_mem_ops(
	fs_stream_t *fs_stream,
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(fs_stream);
	fs_stream->mem_ops = mem_ops;
}

const osal_mem_ops_t *fs_stream_get_mem_ops(
	const fs_stream_t *fs_stream
) {
	LEXLEO_ASSERT(fs_stream);
	return fs_stream->mem_ops;
}

void fs_stream_inject_file(
	fs_stream_t *fs_stream,
	OSAL_FILE *file
) {
	LEXLEO_ASSERT(fs_stream);
	fs_stream->file = file;
}

OSAL_FILE *fs_stream_get_file(
	fs_stream_t *fs_stream
) {
	LEXLEO_ASSERT(fs_stream);
	return fs_stream->file;
}
