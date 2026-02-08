/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/fs_stream/src/wiring/
 * fs_stream_ctx.h
 */

#ifndef LEXLEO_FS_STREAM_CTX_H
#define LEXLEO_FS_STREAM_CTX_H

#include "file/osal_file_ops.h"
#include "mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fs_stream_deps_t {
	const osal_mem_ops_t *mem;
} fs_stream_deps_t;

typedef struct fs_stream_ctx_t {
	fs_stream_deps_t deps;
} fs_stream_ctx_t;

fs_stream_ctx_t fs_stream_default_ctx(
	const osal_mem_ops_t *mem_ops );

#ifdef __cplusplus
} // extern "C"
#endif

#endif //LEXLEO_FS_STREAM_CTX_H
