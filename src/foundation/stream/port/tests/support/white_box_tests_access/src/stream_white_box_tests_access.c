/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_white_box_tests_access.c
 * @ingroup stream_white_box_tests_access_group
 * @brief White-box test access helper implementation for the stream module.
 *
 * @details
 * This file implements the test-only helpers used to inject and observe
 * selected internal owned-resource fields of stream handles.
 */

#include "stream/tests/stream_white_box_tests_access.h"

#include "internal/stream_handle.h"
#include "internal/stream_factory_create_stream.h"

#include "policy/lexleo_assert.h"

void stream_inject_backend(stream_t *stream, void *backend)
{
	LEXLEO_ASSERT(stream);
	stream->backend = backend;
}

void *stream_get_backend(const stream_t *stream)
{
	LEXLEO_ASSERT(stream);
	return stream->backend;
}

stream_status_t stream_white_box_factory_create_stream(
	const stream_factory_t *f,
	stream_key_t key,
	const void *args,
	stream_t **out)
{
	return stream_factory_create_stream(f, key, args, out);
}
