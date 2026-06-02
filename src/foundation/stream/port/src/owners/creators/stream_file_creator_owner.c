/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_file_creator_owner.c
 * @ingroup stream_internal_group
 * @brief File stream creator operation implementation.
 *
 * @details
 * This file implements the owner-facing operation used to create a
 * file-backed `stream_t` through a `stream_file_creator_t`.
 */

#include "stream/owners/creators/stream_file_creator.h"

#include "internal/stream_factory_create_stream.h"

#include "internal/creators/stream_file_creator_handle.h"

#include "policy/lexleo_assert.h"

stream_status_t stream_file_creator_create(
	const stream_file_creator_t *creator,
	const char *path,
	const char *mode,
	stream_t **out
) {
	LEXLEO_ASSERT(creator);

	stream_file_creator_args_t args = {
		.path = path,
		.mode = mode
	};

	return
		stream_factory_create_stream(
			creator->base.factory,
			creator->base.key,
			&args,
			out
		);
}
