/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_dynamic_buffer_creator_owner.c
 * @ingroup stream_internal_group
 * @brief Dynamic buffer stream creator operation implementation.
 *
 * @details
 * Implements the owner-facing operation used to create a dynamic-buffer-backed
 * `stream_t` through a `stream_dynamic_buffer_creator_t`.
 */

#include "stream/common/stream_status_type.h"
#include "stream/common/stream_opaque_type.h"
#include "stream/common/stream_factory_status_type.h"

#include "internal/stream_creator_handle.h"
#include "internal/creators/stream_dynamic_buffer_creator_type.h"
#include "internal/stream_internal_api.h"

#include "policy/lexleo_assert.h"

stream_status_t stream_dynamic_buffer_creator_create(
	const stream_dynamic_buffer_creator_t *creator,
	stream_t **out
) {
	LEXLEO_ASSERT(creator && out);

	stream_factory_status_t st =
		stream_factory_create_stream(
			creator->base.factory,
			creator->base.adapter_id,
			NULL,
			out
		);

	return stream_factory_status_to_stream_status(st);
}
