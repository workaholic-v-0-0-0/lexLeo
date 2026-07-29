/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_standard_stream_creator.h
 * @ingroup stream_owners_api
 * @brief Owner-facing creator contract for standard streams.
 *
 * @details
 * This header exposes the owner-facing creator object used to create
 * standard-stream-backed `stream_t` handles.
 */

#ifndef LEXLEO_STREAM_STANDARD_STREAM_CREATOR_H
#define LEXLEO_STREAM_STANDARD_STREAM_CREATOR_H

#include "stream/common/stream_status_type.h"
#include "stream/common/stream_opaque_type.h"
#include "stream/common/creators/stream_standard_stream_creator_args_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Owner-facing creator object for standard streams.
 *
 * @details
 * Represents a reusable object used to create standard-stream-backed
 * `stream_t` handles.
 */
typedef struct stream_standard_stream_creator_t stream_standard_stream_creator_t;

/**
 * @brief Create a standard-stream-backed stream.
 *
 * @param[in] creator Creator object.
 * @param[in] kind Standard stream kind to create.
 * @param[out] out Receives the created stream handle.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_standard_stream_creator_create
 */
stream_status_t stream_standard_stream_creator_create(
	const stream_standard_stream_creator_t *creator,
	stream_standard_stream_kind_t kind,
	stream_t **out);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_STANDARD_STREAM_CREATOR_H */
