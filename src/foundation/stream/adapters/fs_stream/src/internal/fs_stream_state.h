/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/fs_stream/src/internal/
 * fs_stream_state.h
 */

#ifndef LEXLEO_FS_STREAM_STATE_H
#define LEXLEO_FS_STREAM_STATE_H

#include <stdbool.h>

typedef struct osal_file_t osal_file_t;

typedef struct fs_stream_state_t {
	osal_file_t *f;
	bool autoclose;
} fs_stream_state_t;

#endif //LEXLEO_FS_STREAM_STATE_H
