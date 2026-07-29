/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_white_box_tests_access.h
 * @ingroup fs_stream_white_box_tests_access_group
 * @brief Privileged white-box access to internal `fs_stream` state and
 * operations for unit tests.
 *
 * @details
 * Declares test-only helpers used to inspect and modify private `fs_stream_t`
 * state and to access selected internal fs_stream operations without exposing
 * private implementation headers to test translation units.
 */

#ifndef STREAM_WHITE_BOX_TESTS_ACCESS_H
#define STREAM_WHITE_BOX_TESTS_ACCESS_H

#include "fs_stream/cr/fs_stream_cr_types.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/file/osal_file_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void fs_stream_inject_file_ops(
	fs_stream_t *fs_stream,
	const osal_file_ops_t *file_ops
);

const osal_file_ops_t *fs_stream_get_file_ops(
	const fs_stream_t *fs_stream
);

void fs_stream_inject_mem_ops(
	fs_stream_t *fs_stream,
	const osal_mem_ops_t *mem_ops
);

const osal_mem_ops_t *fs_stream_get_mem_ops(
	const fs_stream_t *fs_stream
);

void fs_stream_inject_file(
	fs_stream_t *fs_stream,
	OSAL_FILE *file
);

OSAL_FILE *fs_stream_get_file(fs_stream_t *fs_stream);

#ifdef __cplusplus
}
#endif

#endif /* STREAM_WHITE_BOX_TESTS_ACCESS_H */
