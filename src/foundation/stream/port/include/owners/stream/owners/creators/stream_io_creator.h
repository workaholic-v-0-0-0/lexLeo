/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_io_creator.h
 * @ingroup stream_owners_api
 * @brief Owner-facing creator contract for standard I/O-oriented streams.
 *
 * @details
 * This header exposes a small owner-facing creator object used to build
 * `stream_t` handles for standard input/output/error workflows.
 */

#ifndef LEXLEO_STREAM_IO_CREATOR_H
#define LEXLEO_STREAM_IO_CREATOR_H

#include "stream/borrowers/stream_borrowers_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Owner-facing creator object for input-output-backed streams.
 */
typedef struct stream_io_creator_t stream_io_creator_t;

/**
 * @brief Kind of standard I/O stream requested from a `stream_io_creator_t`.
 */
typedef enum stream_io_kind_t {

	/** Standard input stream. */
	STREAM_IO_INPUT = 0,

	/** Standard output stream. */
	STREAM_IO_OUTPUT,

	/** Standard error stream. */
	STREAM_IO_ERR

} stream_io_kind_t;

/**
 * @brief Standard I/O stream creation arguments.
 */
typedef struct stream_io_creator_args_t {

	/** Standard I/O stream kind to wrap. */
	stream_io_kind_t kind;

} stream_io_creator_args_t;

/**
 * @brief Create a standard-I/O-backed stream through a creator object.
 *
 * @param[in] creator Creator object to use.
 * @param[in] kind Standard I/O stream kind to wrap.
 * @param[out] out Receives the created stream handle.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_io_creator_create
 */
stream_status_t stream_io_creator_create(
	const stream_io_creator_t *creator,
	stream_io_kind_t kind,
	stream_t **out);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_IO_CREATOR_H */
