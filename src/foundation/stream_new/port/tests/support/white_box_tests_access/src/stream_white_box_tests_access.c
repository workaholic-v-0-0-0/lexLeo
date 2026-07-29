/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_white_box_tests_access.c
 * @ingroup stream_white_box_tests_access_group
 * @brief White-box test access implementation for the `stream` module.
 *
 * @details
 * Implements test-only helpers used to inspect and modify private stream
 * state and to expose selected internal operations through the white-box
 * test API.
 */

#include "stream/tests/stream_white_box_tests_access.h"

#include "internal/stream_internal_api.h"

#include "policy/lexleo_assert.h"

void stream_inject_backend(stream_t *stream, void *backend)
{
	LEXLEO_ASSERT(stream);
	stream->backend = backend;
}

const stream_vtbl_t *stream_get_vtbl(const stream_t *stream)
{
	LEXLEO_ASSERT(stream);
	return stream->vtbl;
}

const osal_mem_ops_t *stream_get_mem(const stream_t *stream)
{
	LEXLEO_ASSERT(stream);
	return stream->mem;
}

void *stream_get_backend(const stream_t *stream)
{
	LEXLEO_ASSERT(stream);
	return stream->backend;
}

static const stream_internal_api_t g_stream_internal_api = {
	.stream_factory_create_stream = stream_factory_create_stream
};

const stream_internal_api_t *stream_get_internal_api(void)
{
	return &g_stream_internal_api;
}