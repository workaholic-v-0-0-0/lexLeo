/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_state.h
 * @ingroup fs_stream_internal_group
 * @brief Internal runtime state for the fs_stream backend.
 */

#ifndef LEXLEO_FS_STREAM_STATE_H
#define LEXLEO_FS_STREAM_STATE_H

#include <stdbool.h>

typedef struct osal_file_t osal_file_t;

/**
 * @brief Runtime state for the fs_stream backend.
 *
 * Stores the OSAL file handle and close policy used by the backend during the
 * lifetime of the stream instance.
 */
typedef struct fs_stream_state_t {
	osal_file_t *f;   /**< OSAL file handle backing the stream (may be NULL once closed). */
	bool autoclose;   /**< If true, close the OSAL file when the stream backend is closed. */
} fs_stream_state_t;

#endif //LEXLEO_FS_STREAM_STATE_H
