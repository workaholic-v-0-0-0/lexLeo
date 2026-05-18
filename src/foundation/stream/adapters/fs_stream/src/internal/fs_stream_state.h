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

#include "osal/file/osal_file_types.h"

/**
 * @brief Private runtime state for the `fs_stream` backend.
 */
typedef struct fs_stream_state_t {
	/** OSAL file handle backing the stream. Must be `NULL` once closed. */
	OSAL_FILE *file;
} fs_stream_state_t;

#endif /* LEXLEO_FS_STREAM_STATE_H */
