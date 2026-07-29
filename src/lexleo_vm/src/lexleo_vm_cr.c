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

	tmp->lexleo_vm_owned_resources =
		env->mem_ops->calloc(1, sizeof(*tmp->lexleo_vm_owned_resources));
	if (!tmp->lexleo_vm_owned_resources) {
		env->mem_ops->free(tmp);
		return LEXLEO_VM_STATUS_OOM;
	}

	tmp->mem_ops = env->mem_ops;
	tmp->stdio_ops = env->stdio_ops;
	tmp->file_ops = env->file_ops;
	tmp->str_ops = env->str_ops;
	tmp->time_ops = env->time_ops;
	tmp->in = env->in;
	tmp->out = env->out;
	tmp->err = env->err;
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
		&& (*vm)->lexleo_vm_owned_resources
	);

	stream_destroy_standard_stream_creator(
		&(*vm)->lexleo_vm_owned_resources->stream_standard_stream_creator
	);
	stream_destroy_regular_file_creator(
		&(*vm)->lexleo_vm_owned_resources->stream_regular_file_creator
	);
	stream_destroy_dynamic_buffer_creator(
		&(*vm)->lexleo_vm_owned_resources->stream_dynamic_buffer_creator
	);
	stream_destroy_factory(&(*vm)->lexleo_vm_owned_resources->stream_factory);
	(*vm)->mem_ops->free((*vm)->lexleo_vm_owned_resources);
	(*vm)->mem_ops->free(*vm);
	*vm = NULL;
}

lexleo_vm_status_t lexleo_vm_complete_default_init(
	lexleo_vm_t *vm,
	const lexleo_vm_cfg_t *cfg
) {
	LEXLEO_ASSERT(
		   vm
		&& vm->lexleo_vm_owned_resources
		&& cfg
	);

	if (vm->lexleo_vm_owned_resources->stream_factory) {
		LEXLEO_ASSERT(
			   vm->lexleo_vm_owned_resources->stream_standard_stream_creator
			&& vm->lexleo_vm_owned_resources->stream_regular_file_creator
			&& vm->lexleo_vm_owned_resources->stream_dynamic_buffer_creator
		);
		return LEXLEO_VM_STATUS_OK;
	}

	LEXLEO_ASSERT(
		   !vm->lexleo_vm_owned_resources->stream_standard_stream_creator
		&& !vm->lexleo_vm_owned_resources->stream_regular_file_creator
		&& !vm->lexleo_vm_owned_resources->stream_dynamic_buffer_creator
	);

	stream_factory_status_t stream_factory_status = STREAM_FACTORY_STATUS_OK;
	stdio_stream_status_t stdio_stream_status = STDIO_STREAM_STATUS_OK;
	fs_stream_status_t fs_stream_status = FS_STREAM_STATUS_OK;
	dynamic_buffer_stream_status_t dynamic_buffer_stream_status = DYNAMIC_BUFFER_STREAM_STATUS_OK;

	stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
	stream_factory_status =
		stream_create_factory(
			&vm->lexleo_vm_owned_resources->stream_factory,
			&stream_factory_cfg,
			vm->mem_ops
		);
	if (stream_factory_status != STREAM_FACTORY_STATUS_OK) {
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	stdio_stream_cfg_t stdio_stream_cfg = stdio_stream_default_cfg();
	stdio_stream_env_t stdio_stream_env =
		stdio_stream_default_env(
			vm->stdio_ops,
			vm->mem_ops
		);
	stream_adapter_provider_t *stream_adapter_provider_standard_stream = NULL;
	stdio_stream_status =
		stdio_stream_create_adapter_provider(
			&stream_adapter_provider_standard_stream,
			&stdio_stream_cfg,
			&stdio_stream_env
		);
	if (stdio_stream_status != STDIO_STREAM_STATUS_OK) {
		stream_destroy_factory(&vm->lexleo_vm_owned_resources->stream_factory);
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	stream_factory_status =
		stream_factory_add_adapter(
			vm->lexleo_vm_owned_resources->stream_factory,
			LEXLEO_VM_STDIO_CREATOR_DEFAULT_KEY,
			stream_adapter_provider_standard_stream
		);
	if (stream_factory_status != STREAM_FACTORY_STATUS_OK) {
		stream_destroy_adapter_provider(stream_adapter_provider_standard_stream);
		stream_destroy_factory(&vm->lexleo_vm_owned_resources->stream_factory);
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	stream_factory_status =
		stream_create_standard_stream_creator(
			&vm->lexleo_vm_owned_resources->stream_standard_stream_creator,
			vm->lexleo_vm_owned_resources->stream_factory,
			LEXLEO_VM_STDIO_CREATOR_DEFAULT_KEY,
			vm->mem_ops
		);
	if (stream_factory_status != STREAM_FACTORY_STATUS_OK) {
		stream_destroy_factory(&vm->lexleo_vm_owned_resources->stream_factory);
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	fs_stream_env_t fs_stream_env =
		fs_stream_default_env(
			vm->file_ops,
			vm->mem_ops
		);
	stream_adapter_provider_t *stream_adapter_provider_regular_file = NULL;
	fs_stream_status =
		fs_stream_create_adapter_provider(
			&stream_adapter_provider_regular_file,
			&fs_stream_cfg,
			&fs_stream_env
		);
	if (fs_stream_status != FS_STREAM_STATUS_OK) {
		stream_destroy_standard_stream_creator(
			&vm->lexleo_vm_owned_resources->stream_standard_stream_creator
		);
		stream_destroy_factory(&vm->lexleo_vm_owned_resources->stream_factory);
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	stream_factory_status =
		stream_factory_add_adapter(
			vm->lexleo_vm_owned_resources->stream_factory,
			LEXLEO_VM_FILE_CREATOR_DEFAULT_KEY,
			stream_adapter_provider_regular_file
		);
	if (stream_factory_status != STREAM_FACTORY_STATUS_OK) {
		stream_destroy_standard_stream_creator(
			&vm->lexleo_vm_owned_resources->stream_standard_stream_creator
		);
		stream_destroy_adapter_provider(stream_adapter_provider_regular_file);
		stream_destroy_factory(&vm->lexleo_vm_owned_resources->stream_factory);
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	stream_factory_status =
		stream_create_regular_file_creator(
			&vm->lexleo_vm_owned_resources->stream_regular_file_creator,
			vm->lexleo_vm_owned_resources->stream_factory,
			LEXLEO_VM_FILE_CREATOR_DEFAULT_KEY,
			vm->mem_ops
		);
	if (stream_factory_status != STREAM_FACTORY_STATUS_OK) {
		stream_destroy_standard_stream_creator(
			&vm->lexleo_vm_owned_resources->stream_standard_stream_creator
		);
		stream_destroy_factory(&vm->lexleo_vm_owned_resources->stream_factory);
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	dynamic_buffer_stream_cfg_t dynamic_buffer_stream_cfg =
		dynamic_buffer_stream_default_cfg();
	dynamic_buffer_stream_env_t dynamic_buffer_stream_env =
		dynamic_buffer_stream_default_env(
			vm->mem_ops
		);
	stream_adapter_provider_t *stream_adapter_provider_dynamic_buffer = NULL;
	dynamic_buffer_stream_status =
		dynamic_buffer_stream_create_adapter_provider(
			&stream_adapter_provider_dynamic_buffer,
			&dynamic_buffer_stream_cfg,
			&dynamic_buffer_stream_env
		);
	if (dynamic_buffer_stream_status != DYNAMIC_BUFFER_STREAM_STATUS_OK) {
		stream_destroy_standard_stream_creator(
			&vm->lexleo_vm_owned_resources->stream_standard_stream_creator
		);
		stream_destroy_regular_file_creator(
			&vm->lexleo_vm_owned_resources->stream_regular_file_creator
		);
		stream_destroy_factory(&vm->lexleo_vm_owned_resources->stream_factory);
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	stream_factory_status =
		stream_factory_add_adapter(
			vm->lexleo_vm_owned_resources->stream_factory,
			LEXLEO_VM_BUFFER_CREATOR_DEFAULT_KEY,
			stream_adapter_provider_dynamic_buffer
		);
	if (stream_factory_status != STREAM_FACTORY_STATUS_OK) {
		stream_destroy_standard_stream_creator(
			&vm->lexleo_vm_owned_resources->stream_standard_stream_creator
		);
		stream_destroy_regular_file_creator(
			&vm->lexleo_vm_owned_resources->stream_regular_file_creator
		);
		stream_destroy_adapter_provider(stream_adapter_provider_dynamic_buffer);
		stream_destroy_factory(&vm->lexleo_vm_owned_resources->stream_factory);
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	stream_factory_status =
		stream_create_dynamic_buffer_creator(
			&vm->lexleo_vm_owned_resources->stream_dynamic_buffer_creator,
			vm->lexleo_vm_owned_resources->stream_factory,
			LEXLEO_VM_BUFFER_CREATOR_DEFAULT_KEY,
			vm->mem_ops
		);
	if (stream_factory_status != STREAM_FACTORY_STATUS_OK) {
		stream_destroy_standard_stream_creator(
			&vm->lexleo_vm_owned_resources->stream_standard_stream_creator
		);
		stream_destroy_regular_file_creator(
			&vm->lexleo_vm_owned_resources->stream_regular_file_creator
		);
		stream_destroy_factory(&vm->lexleo_vm_owned_resources->stream_factory);
		return LEXLEO_VM_STATUS_INIT_FAIL;
	}

	return LEXLEO_VM_STATUS_OK;
}
