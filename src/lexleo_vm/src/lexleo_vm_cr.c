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

lexleo_vm_cfg_t lexleo_vm_default_cfg(void)
{
	return (lexleo_vm_cfg_t) { .reserved = 0 };
}

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
	LEXLEO_ASSERT(
		   mem_ops
		&& stdio_ops
		&& file_ops
		&& str_ops
		&& time_ops
	);

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

lexleo_vm_status_t lexleo_vm_create(
	lexleo_vm_t **out,
	const lexleo_vm_cfg_t *cfg,
	const lexleo_vm_env_t *env
) {
	(void)cfg;

	LEXLEO_ASSERT(
		   out
		&& cfg
		&& env
		&& env->mem_ops
		&& env->mem_ops->calloc
	);

	*out = NULL;

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
	tmp->err = env->err;
	tmp->logger = env->logger;

	/* ... */

	*out = tmp;

	return LEXLEO_VM_STATUS_OK;
}

void lexleo_vm_destroy(lexleo_vm_t **vm)
{
	if (!vm || !*vm) {
		return;
	}

	LEXLEO_ASSERT((*vm)->mem_ops && (*vm)->mem_ops->free);

	stream_destroy_io_creator(&(*vm)->stream_io_creator);
	stream_destroy_file_creator(&(*vm)->stream_file_creator);
	stream_destroy_buffer_creator(&(*vm)->stream_buffer_creator);
	stream_destroy_factory(&(*vm)->stream_factory);

	(*vm)->mem_ops->free(*vm);
	*vm = NULL;
}

static lexleo_vm_status_t lexleo_vm_init_default_stream_factory(
	lexleo_vm_t *vm
);
static lexleo_vm_status_t lexleo_vm_init_default_stream_io_creator(
	lexleo_vm_t *vm
);
static lexleo_vm_status_t lexleo_vm_init_default_stream_file_creator(
	lexleo_vm_t *vm
);
static lexleo_vm_status_t lexleo_vm_init_default_stream_buffer_creator(
	lexleo_vm_t *vm
);

lexleo_vm_status_t lexleo_vm_complete_default_init(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);

	lexleo_vm_status_t st = LEXLEO_VM_STATUS_OK;

	st = lexleo_vm_init_default_stream_factory(vm);
	if (st == LEXLEO_VM_STATUS_OOM)
	{
		return LEXLEO_VM_STATUS_STREAM_FACTORY_INIT_OOM;
	}
	LEXLEO_ASSERT(st == LEXLEO_VM_STATUS_OK);

	st = lexleo_vm_init_default_stream_io_creator(vm);
	if (st != LEXLEO_VM_STATUS_OK)
	{
		return LEXLEO_VM_STATUS_STREAM_IO_CREATOR_INIT_OOM;
	}
	LEXLEO_ASSERT(st == LEXLEO_VM_STATUS_OK);

	st = lexleo_vm_init_default_stream_file_creator(vm);
	if (st != LEXLEO_VM_STATUS_OK)
	{
		return LEXLEO_VM_STATUS_STREAM_FILE_CREATOR_INIT_OOM;
	}
	LEXLEO_ASSERT(st == LEXLEO_VM_STATUS_OK);

	st = lexleo_vm_init_default_stream_buffer_creator(vm);
	if (st != LEXLEO_VM_STATUS_OK)
	{
		return LEXLEO_VM_STATUS_STREAM_BUFFER_CREATOR_INIT_OOM;
	}
	LEXLEO_ASSERT(st == LEXLEO_VM_STATUS_OK);

	return LEXLEO_VM_STATUS_OK;
}

static lexleo_vm_status_t lexleo_vm_init_default_stream_factory(
	lexleo_vm_t *vm
) {
	LEXLEO_ASSERT(vm && vm->mem_ops);

	if (vm->stream_factory != NULL) {
		return LEXLEO_VM_STATUS_OK;
	}

	stream_status_t stream_status = STREAM_STATUS_OK;

	stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();

	stream_status =
		stream_create_factory(
			&vm->stream_factory,
			&stream_factory_cfg,
			vm->mem_ops
		);

	if (stream_status == STREAM_STATUS_OOM) {
		return LEXLEO_VM_STATUS_OOM;
	}
	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	return LEXLEO_VM_STATUS_OK;
}

static lexleo_vm_status_t lexleo_vm_init_default_stream_io_creator(
	lexleo_vm_t *vm
) {
	LEXLEO_ASSERT(vm && vm->stream_factory && vm->mem_ops && vm->stdio_ops);

	if (vm->stream_io_creator != NULL) {
		return LEXLEO_VM_STATUS_OK;
	}

	stream_adapter_desc_t stdio_stream_adapter_desc = {0};
	stdio_stream_cfg_t stdio_stream_cfg = stdio_stream_default_cfg();
	stdio_stream_env_t stdio_stream_env =
		stdio_stream_default_env(
			vm->stdio_ops,
			vm->mem_ops,
			vm->mem_ops
		);
	stream_status_t stream_status =
		stdio_stream_create_desc(
			&stdio_stream_adapter_desc,
			LEXLEO_VM_STDIO_CREATOR_DEFAULT_KEY,
			&stdio_stream_cfg,
			&stdio_stream_env,
			vm->mem_ops
		);

	if (stream_status == STREAM_STATUS_OOM) {
		return LEXLEO_VM_STATUS_OOM;
	}
	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	stream_status =
		stream_factory_add_adapter(
			vm->stream_factory,
			&stdio_stream_adapter_desc
		);
	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	stream_status =
		stream_create_io_creator(
			&vm->stream_io_creator,
			vm->stream_factory,
			LEXLEO_VM_STDIO_CREATOR_DEFAULT_KEY,
			vm->mem_ops
	);

	if (stream_status == STREAM_STATUS_OOM) {
		return LEXLEO_VM_STATUS_OOM;
	}
	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	return LEXLEO_VM_STATUS_OK;
}

static lexleo_vm_status_t lexleo_vm_init_default_stream_file_creator(
	lexleo_vm_t *vm
) {
	LEXLEO_ASSERT(vm && vm->stream_factory && vm->mem_ops && vm->file_ops);

	if (vm->stream_file_creator != NULL) {
		return LEXLEO_VM_STATUS_OK;
	}

	stream_adapter_desc_t fs_stream_adapter_desc = {0};
	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	fs_stream_env_t fs_stream_env =
		fs_stream_default_env(
			vm->file_ops,
			vm->mem_ops,
			vm->mem_ops
		);
	stream_status_t stream_status =
		fs_stream_create_desc(
			&fs_stream_adapter_desc,
			LEXLEO_VM_FILE_CREATOR_DEFAULT_KEY,
			&fs_stream_cfg,
			&fs_stream_env,
			vm->mem_ops
		);

	if (stream_status == STREAM_STATUS_OOM) {
		return LEXLEO_VM_STATUS_OOM;
	}
	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	stream_status =
		stream_factory_add_adapter(
			vm->stream_factory,
			&fs_stream_adapter_desc
		);

	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	stream_status =
		stream_create_file_creator(
			&vm->stream_file_creator,
			vm->stream_factory,
			LEXLEO_VM_FILE_CREATOR_DEFAULT_KEY,
			vm->mem_ops
	);

	if (stream_status == STREAM_STATUS_OOM) {
		return LEXLEO_VM_STATUS_OOM;
	}
	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	return LEXLEO_VM_STATUS_OK;
}

static lexleo_vm_status_t lexleo_vm_init_default_stream_buffer_creator(
	lexleo_vm_t *vm
) {
	LEXLEO_ASSERT(vm && vm->stream_factory && vm->mem_ops);

	if (vm->stream_buffer_creator != NULL) {
		return LEXLEO_VM_STATUS_OK;
	}

	stream_adapter_desc_t buffer_stream_adapter_desc = {0};
	dynamic_buffer_stream_cfg_t dbs_stream_cfg =
		dynamic_buffer_stream_default_cfg();
	dynamic_buffer_stream_env_t dbs_stream_env =
		dynamic_buffer_stream_default_env(
			vm->mem_ops,
			vm->mem_ops
		);
	stream_status_t stream_status =
		dynamic_buffer_stream_create_desc(
			&buffer_stream_adapter_desc,
			LEXLEO_VM_BUFFER_CREATOR_DEFAULT_KEY,
			&dbs_stream_cfg,
			&dbs_stream_env,
			vm->mem_ops
		);

	if (stream_status == STREAM_STATUS_OOM) {
		return LEXLEO_VM_STATUS_OOM;
	}
	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	stream_status =
		stream_factory_add_adapter(
			vm->stream_factory,
			&buffer_stream_adapter_desc
		);

	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	stream_status =
		stream_create_buffer_creator(
			&vm->stream_buffer_creator,
			vm->stream_factory,
			LEXLEO_VM_BUFFER_CREATOR_DEFAULT_KEY,
			vm->mem_ops
	);

	if (stream_status == STREAM_STATUS_OOM) {
		return LEXLEO_VM_STATUS_OOM;
	}
	LEXLEO_ASSERT(stream_status == STREAM_STATUS_OK);

	return LEXLEO_VM_STATUS_OK;
}
