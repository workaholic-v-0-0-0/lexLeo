/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/fs_stream/src/internal/
 * fs_stream_handle.h
 */

#ifndef LEXLEO_FS_STREAM_HANDLE_H
#define LEXLEO_FS_STREAM_HANDLE_H

#include "fs_stream_state.h"
#include "mem/osal_mem_ops.h"

typedef struct fs_stream_t {
	fs_stream_state_t state;
	const osal_mem_ops_t *mem;
	const osal_file_ops_t *file;
} fs_stream_t;

stream_status_t fs_stream_create_stream(
	stream_t **out,
	const fs_stream_ctx_t *ctx );

#endif //LEXLEO_FS_STREAM_HANDLE_H
