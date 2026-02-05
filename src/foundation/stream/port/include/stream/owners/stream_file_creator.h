/* SPDX-License-Identifier: GPL-3.0-or-later
* Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_file_creator.h
 * @ingroup stream_owners_api
 * @brief Owner-style creator contract for file-backed streams.
 *
 * @details
 * This header exposes a small owner-facing creator object used to build
 * `stream_t` handles for file-oriented workflows.
 */

#ifndef LEXLEO_STREAM_FILE_CREATOR_H
#define LEXLEO_STREAM_FILE_CREATOR_H

#include "stream/borrowers/stream_types.h"

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Owner-facing creator object for file-backed streams.
 *
 * @details
 * A `stream_file_creator_t` packages:
 * - a creation callback,
 * - opaque user data bound to that callback.
 *
 * Such an object is typically prepared by the Composition Root and then
 * injected into owner-side runtime code that needs to create file-backed
 * `stream_t` instances without depending directly on factory wiring details.
 */
typedef struct stream_file_creator_t {
	/**
	 * @brief Create a file-backed stream.
	 *
	 * @param[in] ud
	 * Opaque creator-owned context.
	 *
	 * @param[in] path
	 * File path used to open or create the target stream backend.
	 *
	 * @param[in] flags
	 * File opening flags forwarded to the underlying file-oriented adapter.
	 *
	 * @param[in] autoclose
	 * Whether the created stream should close the underlying file resource
	 * automatically when the stream is destroyed.
	 *
	 * @param[out] out
	 * Receives the created stream handle.
	 *
	 * @return
	 * A `stream_status_t` describing the creation result.
	 */
	stream_status_t (*create)(
		const void *ud,
		const char *path,
		uint32_t flags,
		bool autoclose,
		stream_t **out);

	/**
	 * @brief Opaque context bound to `create`.
	 *
	 * @details
	 * This pointer typically hides Composition Root wiring details such as a
	 * factory reference and the associated adapter key.
	 */
	void *ud;
} stream_file_creator_t;

#endif // LEXLEO_STREAM_FILE_CREATOR_H
