/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/fs_stream/include/fs_stream/cr/
 * fs_stream_cr_api.h
 */

#ifndef LEXLEO_FS_STREAM_CR_API_H
#define LEXLEO_FS_STREAM_CR_API_H

#include "stream/adapters/stream_adapters_api.h"
#include "stream/adapters/stream_install.h"
#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_key_type.h"
#include "mem/osal_mem_ops.h"
#include "file/osal_file_ops.h"
#include "file/osal_file_env.h"

#include "policy/lexleo_cstd_types.h"

// no fs_stream_cfg_t
typedef struct fs_stream_cfg_t {
	// empty for now
} fs_stream_cfg_t;

typedef struct fs_stream_env_t {
	osal_file_env_t file_env;
	const osal_file_ops_t *file_ops;
	stream_env_t port_env;
} fs_stream_env_t;

fs_stream_cfg_t fs_stream_default_cfg(void);

fs_stream_env_t fs_stream_default_env(
	const osal_file_env_t *file_env,
	const osal_file_ops_t *file_ops,
	const stream_env_t *port_env );

typedef struct fs_stream_args_t {
	const char *path; // UTF-8
	uint32_t flags;
	bool autoclose;
} fs_stream_args_t;

stream_status_t fs_stream_create_stream(
	stream_t **out,
	const fs_stream_args_t *args,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env );

// Factory callback delegating to create_stream
stream_status_t fs_stream_ctor(
	void *ud,
	const void *args,
	stream_t **out );

stream_status_t fs_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env,
	const osal_mem_ops_t *mem ); // MUST BE THE FACTORY'S

#endif //LEXLEO_FS_STREAM_CR_API_H
