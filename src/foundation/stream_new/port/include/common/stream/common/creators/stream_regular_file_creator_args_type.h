/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_regular_file_creator_args_type.h
 * @ingroup stream_common_api
 * @brief Regular-file stream creation argument type.
 *
 * @details
 * Declares the argument type shared by components that create
 * regular-file-backed `stream` instances.
 */

#ifndef LEXLEO_STREAM_REGULAR_FILE_CREATOR_ARGS_TYPE_H
#define LEXLEO_STREAM_REGULAR_FILE_CREATOR_ARGS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Arguments used to create a file-backed stream.
 */
typedef struct stream_regular_file_creator_args_t {

	/** UTF-8 path of the regular file to open. */
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

} stream_regular_file_creator_args_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_REGULAR_FILE_CREATOR_ARGS_TYPE_H */
