/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_standard_stream_creator_args_type.h
 * @ingroup stream_common_api
 * @brief Standard-stream creation argument type.
 *
 * @details
 * Declares the argument type shared by components that create
 * standard-stream-backed `stream` instances.
 */

#ifndef LEXLEO_STREAM_STANDARD_STREAM_CREATOR_ARGS_TYPE_H
#define LEXLEO_STREAM_STANDARD_STREAM_CREATOR_ARGS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum stream_standard_stream_kind_t
 * @ingroup stream_common_api
 * @brief Kind of standard stream to create.
 */
typedef enum stream_standard_stream_kind_t {

	/** Standard input stream. */
	STREAM_STANDARD_STREAM_KIND_STDIN = 0,

	/** Standard output stream. */
	STREAM_STANDARD_STREAM_KIND_STDOUT,

	/** Standard error stream. */
	STREAM_STANDARD_STREAM_KIND_STDERR

} stream_standard_stream_kind_t;

/**
 * @struct stream_standard_stream_creator_args_t
 * @ingroup stream_common_api
 * @brief Arguments used to create a standard-stream-backed `stream`.
 */
typedef struct stream_standard_stream_creator_args_t {

	/** Standard stream kind to create. */
	stream_standard_stream_kind_t kind;

} stream_standard_stream_creator_args_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_STANDARD_STREAM_CREATOR_ARGS_TYPE_H */
