/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_standard_stream_creator_owner.c
 * @ingroup stream_internal_group
 * @brief Standard stream creator operation implementation.
 *
 * @details
 * Implements the owner-facing operation used to create a standard-stream-backed
 * `stream_t` through a `stream_standard_stream_creator_t`.
 */

#include "stream/common/stream_status_type.h"
#include "stream/common/stream_opaque_type.h"
#include "stream/common/stream_factory_status_type.h"
#include "stream/common/creators/stream_standard_stream_creator_args_type.h"

#include "internal/stream_creator_handle.h"
#include "internal/creators/stream_standard_stream_creator_type.h"
#include "internal/stream_internal_api.h"

#include "policy/lexleo_assert.h"

stream_status_t stream_standard_stream_creator_create(
	const stream_standard_stream_creator_t *creator,
	stream_standard_stream_kind_t kind,
	stream_t **out
) {
	LEXLEO_ASSERT(
		   creator
		&& (
			   kind == STREAM_STANDARD_STREAM_KIND_STDIN
			|| kind == STREAM_STANDARD_STREAM_KIND_STDOUT
			|| kind == STREAM_STANDARD_STREAM_KIND_STDERR
		)
		&& out
	);

	stream_standard_stream_creator_args_t args = { .kind = kind };

	stream_factory_status_t st =
		stream_factory_create_stream(
			creator->base.factory,
			creator->base.adapter_id,
			&args,
			out
		);

	return stream_factory_status_to_stream_status(st);
}
