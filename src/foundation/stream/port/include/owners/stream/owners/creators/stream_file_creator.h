/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_file_creator.h
 * @ingroup stream_owners_api
 * @brief Owner-facing creator contract for file-backed streams.
 *
 * @details
 * This header exposes a small owner-facing creator object used to build
 * `stream_t` handles for file-oriented workflows.
 */

#ifndef LEXLEO_STREAM_FILE_CREATOR_H
#define LEXLEO_STREAM_FILE_CREATOR_H

#include "stream/borrowers/stream_borrowers_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Owner-facing creator object for file-backed streams.
 */
typedef struct stream_file_creator_t stream_file_creator_t;

/**
 * @brief File stream creation arguments.
 */
typedef struct stream_file_creator_args_t {

	/**
	 * @brief UTF-8 path of the target file.
	 */
	const char *path;

	/**
	 * @brief Portable OSAL file open mode.
	 *
	 * @details
	 * This string is forwarded to `osal_file_ops_t::open()`.
	 * Supported values are defined by the `osal_file` contract,
	 * currently `"rb"`, `"wb"`, and `"ab"`.
	 */
	const char *mode;

} stream_file_creator_args_t;

/**
 * @brief Create a file-backed stream through a creator object.
 *
 * @param[in] creator Creator object to use.
 * @param[in] path UTF-8 path of the target file.
 * @param[in] mode Portable OSAL file open mode.
 * @param[out] out Receives the created stream handle.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_regular_file_creator_create
 */
stream_status_t stream_file_creator_create(
	const stream_file_creator_t *creator,
	const char *path,
	const char *mode,
	stream_t **out);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_FILE_CREATOR_H */
