/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_cr.c
 * @ingroup lexleo_vm_internal_group
 * @brief Composition Root implementation for the LexLeo VM module.
 *
 * @details
 * This file implements the Composition Root services used to construct
 * LexLeo VM handles.
 *
 * It creates VM handles from configuration values and borrowed runtime
 * environments, completes their default initialization by creating internal
 * owned runtime resources, and destroys VM handles.
 */

#include "internal/lexleo_vm_handle.h"

#include "lexleo_vm/cr/lexleo_vm_cr_api.h"

#include "stream/cr/stream_cr_api.h"

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_api.h"
#include "stdio_stream/cr/stdio_stream_cr_api.h"
#include "fs_stream/cr/fs_stream_cr_api.h"

#include "policy/lexleo_assert.h"

#define LEXLEO_VM_FILE_CREATOR_DEFAULT_KEY "fs"
#define LEXLEO_VM_STDIO_CREATOR_DEFAULT_KEY "stdio"
#define LEXLEO_VM_BUFFER_CREATOR_DEFAULT_KEY "dbs"

lexleo_vm_env_t lexleo_vm_default_env(
	const osal_mem_ops_t *mem_ops,
	const osal_stdio_ops_t *stdio_ops,
	const osal_file_ops_t *file_ops,
	const osal_str_ops_t *str_ops,
	const osal_time_ops_t *time_ops,
	stream_t *in,
	stream_t *out,
	stream_t *err,
	logger_t *logger
) {
	return (lexleo_vm_env_t){
		.mem_ops = mem_ops,
		.stdio_ops = stdio_ops,
		.file_ops = file_ops,
		.str_ops = str_ops,
		.time_ops = time_ops,
		.in = in,
		.out = out,
		.err = err,
		.logger = logger
	};
}

lexleo_vm_cfg_t lexleo_vm_default_cfg(void)
{
	return (lexleo_vm_cfg_t){
		.reserved = 0
	};
}

