/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/fs_stream/include/fs_stream/cr/
 * fs_stream_types.h
 */

#ifndef LEXLEO_FS_STREAM_TYPES_H
#define LEXLEO_FS_STREAM_TYPES_H

#include "stream/adapters/stream_env.h"
#include "osal/file/osal_file_ops.h"
#include "osal/file/osal_file_env.h"
#include "policy/lexleo_cstd_types.h"

typedef struct fs_stream_cfg_t {
	// empty for now
} fs_stream_cfg_t;

typedef struct fs_stream_env_t {
	osal_file_env_t file_env;
	const osal_file_ops_t *file_ops;
	stream_env_t port_env;
} fs_stream_env_t;

typedef struct fs_stream_args_t {
	const char *path; // UTF-8
	uint32_t flags;
	bool autoclose;
} fs_stream_args_t;

#endif //LEXLEO_FS_STREAM_TYPES_H
