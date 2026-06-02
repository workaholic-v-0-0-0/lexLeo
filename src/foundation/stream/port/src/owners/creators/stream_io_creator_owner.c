/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_io_creator_owner.c
 * @ingroup stream_internal_group
 * @brief I/O stream creator operation implementation.
 *
 * @details
 * This file implements the owner-facing operation used to create a standard
 * I/O-backed `stream_t` through a `stream_io_creator_t`.
 */

#include "stream/owners/creators/stream_io_creator.h"

#include "internal/stream_factory_create_stream.h"

#include "internal/creators/stream_io_creator_handle.h"

#include "policy/lexleo_assert.h"

stream_status_t stream_io_creator_create(
	const stream_io_creator_t *creator,
	stream_io_kind_t kind,
	stream_t **out
) {
	LEXLEO_ASSERT(creator);

	stream_io_creator_args_t io_creator_args = {
		.kind = kind,
	};

	return
		stream_factory_create_stream(
			creator->base.factory,
			creator->base.key,
			&io_creator_args,
			out
		);
}
