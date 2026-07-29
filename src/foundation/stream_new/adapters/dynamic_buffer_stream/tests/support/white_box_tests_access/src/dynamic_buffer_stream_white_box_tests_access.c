/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_white_box_tests_access.c
 * @ingroup dynamic_buffer_stream_white_box_tests_access_group
 * @brief White-box test access implementation for the
 * `dynamic_buffer_stream` module.
 *
 * @details
 * Implements test-only helpers used to inspect
 * `dynamic_buffer_stream_t` external borrowed dependencies and inspect or
 * inject its internal state.
 */

#include "dynamic_buffer_stream/tests/dynamic_buffer_stream_white_box_tests_access.h"

#include "internal/dynamic_buffer_stream_handle.h"

#include "policy/lexleo_assert.h"

const osal_mem_ops_t *dynamic_buffer_stream_get_mem_ops(
	const dynamic_buffer_stream_t *dynamic_buffer_stream
) {
	LEXLEO_ASSERT(dynamic_buffer_stream);
	return dynamic_buffer_stream->mem_ops;
}

void dynamic_buffer_stream_inject_state(
	dynamic_buffer_stream_t *dynamic_buffer_stream,
	dynamic_buffer_stream_state_t state
) {
	LEXLEO_ASSERT(dynamic_buffer_stream);
	dynamic_buffer_stream->state = state;
}

dynamic_buffer_stream_state_t dynamic_buffer_stream_get_state(
	const dynamic_buffer_stream_t *dynamic_buffer_stream
) {
	LEXLEO_ASSERT(dynamic_buffer_stream);
	return dynamic_buffer_stream->state;
}
