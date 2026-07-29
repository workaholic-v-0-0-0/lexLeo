/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_regular_file_creator_owner.c
 * @ingroup stream_internal_group
 * @brief Regular-file stream creator operation implementation.
 *
 * @details
 * Implements the owner-facing operation used to create a regular-file-backed
 * `stream_t` through a `stream_regular_file_creator_t`.
 */

#include "stream/common/stream_status_type.h"
#include "stream/common/stream_opaque_type.h"
#include "stream/common/stream_factory_status_type.h"
#include "stream/common/creators/stream_regular_file_creator_args_type.h"

#include "internal/stream_creator_handle.h"
#include "internal/creators/stream_regular_file_creator_type.h"
#include "internal/stream_internal_api.h"

#include "osal/str/osal_str.h"

#include "policy/lexleo_assert.h"

stream_status_t stream_regular_file_creator_create(
	const stream_regular_file_creator_t *creator,
	const char *path,
	const char *mode,
	stream_t **out
) {
	LEXLEO_ASSERT(
		   creator
		&& mode
		&& (
			   osal_strcmp(mode, "rb") == 0
			|| osal_strcmp(mode, "wb") == 0
			|| osal_strcmp(mode, "ab") == 0
		)
		&& out
	);

	if (!path || *path == '\0' || osal_strlen(path) > 255) {
		return STREAM_STATUS_INVALID;
	}

	stream_regular_file_creator_args_t args = {
		.path = path,
		.mode = mode
	};

	stream_factory_status_t st =
		stream_factory_create_stream(
			creator->base.factory,
			creator->base.adapter_id,
			&args,
			out
		);

	return stream_factory_status_to_stream_status(st);
}
