/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_state.h
 * @ingroup fs_stream_internal_group
 * @brief Private runtime state definition for the `fs_stream` backend.
 *
 * @details
 * This header exposes the private runtime state stored inside the
 * `fs_stream` backend handle.
 */

#ifndef LEXLEO_FS_STREAM_STATE_H
#define LEXLEO_FS_STREAM_STATE_H

#include <stdbool.h>

typedef struct osal_file_t osal_file_t;

/**
 * @brief Private runtime state for the `fs_stream` backend.
 *
 * @details
 * This structure stores the OSAL file handle and close policy used by the
 * backend during the lifetime of the stream instance.
 */
typedef struct fs_stream_state_t {
	/** OSAL file handle backing the stream. May be `NULL` once closed. */
	osal_file_t *f;

	/** Whether the OSAL file must be closed when the backend is closed. */
	bool autoclose;
} fs_stream_state_t;

#endif // LEXLEO_FS_STREAM_STATE_H
