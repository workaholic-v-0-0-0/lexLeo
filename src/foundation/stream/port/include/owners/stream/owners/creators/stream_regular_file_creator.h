/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_regular_file_creator.h
 * @ingroup stream_owners_api
 * @brief Owner-facing creator contract for regular-file streams.
 *
 * @details
 * This header exposes the owner-facing creator object used to create
 * regular-file-backed `stream_t` handles.
 */

#ifndef LEXLEO_STREAM_REGULAR_FILE_CREATOR_H
#define LEXLEO_STREAM_REGULAR_FILE_CREATOR_H

#include "stream/common/stream_status_type.h"
#include "stream/common/stream_opaque_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Owner-facing creator object for regular-file streams.
 *
 * @details
 * Represents a reusable object used to create regular-file-backed
 * `stream_t` handles.
 */
typedef struct stream_regular_file_creator_t stream_regular_file_creator_t;

/**
 * @brief Create a regular-file-backed stream.
 *
 * @param[in] creator Creator object.
 * @param[in] path UTF-8 path of the regular file to open.
 * @param[in] mode Portable OSAL file open mode.
 * @param[out] out Receives the created stream handle.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_regular_file_creator_create
 */
stream_status_t stream_regular_file_creator_create(
	const stream_regular_file_creator_t *creator,
	const char *path,
	const char *mode,
	stream_t **out);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_REGULAR_FILE_CREATOR_H */
