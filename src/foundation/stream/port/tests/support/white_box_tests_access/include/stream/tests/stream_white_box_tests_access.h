/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_white_box_tests_access.h
 * @ingroup stream_white_box_tests_access_group
 * @brief Privileged white-box test access to internal stream operations and
 *        state.
 *
 * @details
 * This header provides test-only access to selected internal stream state and
 * internal stream operations.
 *
 * These declarations are not part of the production API.
 */

#ifndef STREAM_WHITE_BOX_TESTS_ACCESS_H
#define STREAM_WHITE_BOX_TESTS_ACCESS_H

#include "stream/borrowers/stream_borrowers_types.h"
#include "stream/owners/stream_owners_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void stream_inject_backend(stream_t *stream, void *backend);

void *stream_get_backend(const stream_t *stream);

stream_status_t stream_white_box_factory_create_stream(
	const stream_factory_t *f,
	stream_key_t key,
	const void *args,
	stream_t **out);

#ifdef __cplusplus
}
#endif

#endif /* STREAM_WHITE_BOX_TESTS_ACCESS_H */
