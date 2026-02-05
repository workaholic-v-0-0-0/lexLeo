/* SPDX-License-Identifier: GPL-3.0-or-later
* Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_io_creator.h
 * @ingroup stream_owners_api
 * @brief Owner-style creator contract for standard I/O-oriented streams.
 *
 * @details
 * This header exposes a small owner-facing creator object used to build
 * `stream_t` handles for standard input/output/error workflows.
 */

#ifndef LEXLEO_STREAM_IO_CREATOR_H
#define LEXLEO_STREAM_IO_CREATOR_H

#include "stream/borrowers/stream_types.h"

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
 * @brief Owner-facing creator object for standard I/O-oriented streams.
 *
 * @details
 * A `stream_io_creator_t` packages:
 * - a creation callback,
 * - opaque user data bound to that callback.
 *
 * Such an object is typically prepared by the Composition Root and then
 * injected into owner-side runtime code that needs to create standard
 * input/output/error `stream_t` instances without depending directly on
 * factory wiring details.
 */
typedef struct stream_io_creator_t {
	/**
	 * @brief Create a standard I/O-oriented stream.
	 *
	 * @param[in] ud
	 * Opaque creator-owned context.
	 *
	 * @param[in] kind
	 * Requested standard I/O kind.
	 *
	 * @param[out] out
	 * Receives the created stream handle.
	 *
	 * @return
	 * A `stream_status_t` describing the creation result.
	 */
	stream_status_t (*create)(
		const void *ud,
		stream_io_kind_t kind,
		stream_t **out);

	/**
	 * @brief Opaque context bound to `create`.
	 *
	 * @details
	 * This pointer typically hides Composition Root wiring details such as a
	 * factory reference and the associated adapter key.
	 */
	void *ud;
} stream_io_creator_t;

#endif // LEXLEO_STREAM_IO_CREATOR_H
