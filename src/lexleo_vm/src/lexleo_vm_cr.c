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
#include "internal/lexleo_vm_owned_resources_type.h"

#include "lexleo_vm/cr/lexleo_vm_cr_api.h"

#include "stream/cr/stream_cr_api.h"

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_api.h"
#include "stdio_stream/cr/stdio_stream_cr_api.h"
#include "fs_stream/cr/fs_stream_cr_api.h"

#include "policy/lexleo_assert.h"

lexleo_vm_env_t lexleo_vm_default_env(
	const osal_mem_ops_t *mem_ops,
	const osal_stdio_ops_t *stdio_ops,
	const osal_file_ops_t *file_ops,
	const osal_str_ops_t *str_ops,
	const osal_time_ops_t *time_ops,
	stream_t *in,
	stream_t *out,
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
		.logger = logger
	};
}

lexleo_vm_cfg_t lexleo_vm_default_cfg(void)
{
	return (lexleo_vm_cfg_t){
		.reserved = 0
	};
}

lexleo_vm_status_t lexleo_vm_create(
	lexleo_vm_t **out,
	const lexleo_vm_env_t *env
) {
	LEXLEO_ASSERT(
		   out
		&& env
		&& env->mem_ops
		&& env->mem_ops->calloc
		&& env->mem_ops->free
	);

	lexleo_vm_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return LEXLEO_VM_STATUS_OOM;
	}

	tmp->mem_ops = env->mem_ops;
	tmp->stdio_ops = env->stdio_ops;
	tmp->file_ops = env->file_ops;
	tmp->str_ops = env->str_ops;
	tmp->time_ops = env->time_ops;
	tmp->in = env->in;
	tmp->out = env->out;
	tmp->logger = env->logger;

	*out = tmp;
	return LEXLEO_VM_STATUS_OK;
}

void lexleo_vm_destroy(lexleo_vm_t **vm)
{
	if (!vm || !*vm) {
		return;
	}

	LEXLEO_ASSERT(
		   (*vm)->mem_ops
		&& (*vm)->mem_ops->free
	);

	(*vm)->mem_ops->free(*vm);
	*vm = NULL;
}

lexleo_vm_status_t lexleo_vm_complete_default_init(
	lexleo_vm_t *vm,
	const lexleo_vm_cfg_t *cfg
) {
	LEXLEO_ASSERT(
		   vm
		&& cfg
	);

	return LEXLEO_VM_STATUS_OK;
}

lexleo_vm_status_t lexleo_vm_create_init(
	lexleo_vm_t **out,
	const lexleo_vm_env_t *env,
	const lexleo_vm_cfg_t *cfg
) {
	LEXLEO_ASSERT(out && env);

	lexleo_vm_t *tmp = NULL;
	lexleo_vm_status_t ret = lexleo_vm_create(&tmp, env);
	if (ret != LEXLEO_VM_STATUS_OK) {
		return ret;
	}

	ret = lexleo_vm_complete_default_init(tmp, cfg);
	if (ret != LEXLEO_VM_STATUS_OK) {
		lexleo_vm_destroy(&tmp);
		return ret;
	}

	*out = tmp;
	return ret;
}
