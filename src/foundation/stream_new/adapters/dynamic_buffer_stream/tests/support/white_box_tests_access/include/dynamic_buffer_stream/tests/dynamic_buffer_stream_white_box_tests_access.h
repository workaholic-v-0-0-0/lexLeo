/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_white_box_tests_access.h
 * @ingroup dynamic_buffer_stream_white_box_tests_access_group
 * @brief Privileged white-box access to internal `dynamic_buffer_stream` state
 * and operations for unit tests.
 *
* @details
 * Declares test-only helpers used to inspect `dynamic_buffer_stream_t`
 * external borrowed dependencies and inspect or inject its internal state
 * without requiring test translation units to include private implementation
 * headers directly.
 */

#ifndef DYNAMIC_BUFFER_STREAM_WHITE_BOX_TESTS_ACCESS_H
#define DYNAMIC_BUFFER_STREAM_WHITE_BOX_TESTS_ACCESS_H

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_types.h"

#include "internal/dynamic_buffer_stream_state.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

const osal_mem_ops_t *dynamic_buffer_stream_get_mem_ops(
	const dynamic_buffer_stream_t *dynamic_buffer_stream
);

void dynamic_buffer_stream_inject_state(
	dynamic_buffer_stream_t *dynamic_buffer_stream,
	dynamic_buffer_stream_state_t state
);

dynamic_buffer_stream_state_t dynamic_buffer_stream_get_state(
	const dynamic_buffer_stream_t *dynamic_buffer_stream);

#ifdef __cplusplus
}
#endif

#endif /* DYNAMIC_BUFFER_STREAM_WHITE_BOX_TESTS_ACCESS_H */
