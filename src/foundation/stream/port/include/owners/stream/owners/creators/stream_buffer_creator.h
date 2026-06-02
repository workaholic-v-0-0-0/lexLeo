/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_buffer_creator.h
 * @ingroup stream_owners_api
 * @brief Owner-facing creator contract for buffer-backed streams.
 *
 * @details
 * This header exposes a small owner-facing creator object used to build
 * `stream_t` handles for buffer-oriented workflows.
 */

#ifndef LEXLEO_STREAM_BUFFER_CREATOR_H
#define LEXLEO_STREAM_BUFFER_CREATOR_H

#include "stream/borrowers/stream_borrowers_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Owner-facing creator object for buffer-backed streams.
 */
typedef struct stream_buffer_creator_t stream_buffer_creator_t;

/**
 * @brief Create a buffer-backed stream through a creator object.
 *
 * @param[in] creator Creator object to use.
 * @param[out] out Receives the created stream handle.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_buffer_creator_create
 */
stream_status_t stream_buffer_creator_create(
	const stream_buffer_creator_t *creator,
	stream_t **out);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_BUFFER_CREATOR_H */
