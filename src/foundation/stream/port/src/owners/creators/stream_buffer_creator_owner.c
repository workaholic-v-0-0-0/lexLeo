/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_buffer_creator_owner.c
 * @ingroup stream_internal_group
 * @brief Buffer stream creator operation implementation.
 *
 * @details
 * This file implements the owner-facing operation used to create a
 * buffer-backed `stream_t` through a `stream_buffer_creator_t`.
 */

#include "stream/owners/creators/stream_buffer_creator.h"

#include "internal/stream_factory_create_stream.h"

#include "internal/creators/stream_buffer_creator_handle.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_assert.h"

stream_status_t stream_buffer_creator_create(
	const stream_buffer_creator_t *creator,
	stream_t **out
) {
	LEXLEO_ASSERT(creator);

	return
		stream_factory_create_stream(
			creator->base.factory,
			creator->base.key,
			NULL,
			out
		);
}
