/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_white_box_tests_access.c
 * @ingroup lexleo_vm_white_box_tests_access_group
 * @brief White-box test access helper implementation for the LexLeo VM module.
 *
 * @details
 * This file implements the test-only helpers used to inject and observe
 * selected internal owned-resource fields of LexLeo VM handles.
 */

#include "lexleo_vm/tests/lexleo_vm_white_box_tests_access.h"

#include "internal/lexleo_vm_handle.h"
#include "internal/lexleo_vm_owned_resources_type.h"

#include "policy/lexleo_assert.h"

const osal_mem_ops_t *lexleo_vm_get_mem_ops(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->mem_ops;
}

const osal_stdio_ops_t *lexleo_vm_get_stdio_ops(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->stdio_ops;
}

const osal_file_ops_t *lexleo_vm_get_file_ops(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->file_ops;
}

const osal_str_ops_t *lexleo_vm_get_str_ops(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->str_ops;
}

const osal_time_ops_t *lexleo_vm_get_time_ops(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->time_ops;
}

stream_t *lexleo_vm_get_in(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->in;
}

stream_t *lexleo_vm_get_out(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->out;
}

stream_t *lexleo_vm_get_err(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->err;
}

logger_t *lexleo_vm_get_logger(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->logger;
}

void lexleo_vm_inject_stream_factory(
	lexleo_vm_t *vm,
	stream_factory_t *stream_factory
) {
	LEXLEO_ASSERT(vm && vm->lexleo_vm_owned_resources);
	vm->lexleo_vm_owned_resources->stream_factory = stream_factory;
}

void lexleo_vm_inject_stream_standard_stream_creator(
	lexleo_vm_t *vm,
	stream_standard_stream_creator_t *stream_standard_stream_creator
) {
	LEXLEO_ASSERT(vm && vm->lexleo_vm_owned_resources);
	vm->lexleo_vm_owned_resources->stream_standard_stream_creator = stream_standard_stream_creator;
}

void lexleo_vm_inject_stream_regular_file_creator(
	lexleo_vm_t *vm,
	stream_regular_file_creator_t *stream_regular_file_creator
) {
	LEXLEO_ASSERT(vm && vm->lexleo_vm_owned_resources);
	vm->lexleo_vm_owned_resources->stream_regular_file_creator = stream_regular_file_creator;
}

void lexleo_vm_inject_stream_dynamic_buffer_creator(
	lexleo_vm_t *vm,
	stream_dynamic_buffer_creator_t *stream_dynamic_buffer_creator
) {
	LEXLEO_ASSERT(vm && vm->lexleo_vm_owned_resources);
	vm->lexleo_vm_owned_resources->stream_dynamic_buffer_creator = stream_dynamic_buffer_creator;
}

stream_factory_t *lexleo_vm_get_stream_factory(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm && vm->lexleo_vm_owned_resources);
	return vm->lexleo_vm_owned_resources->stream_factory;
}

stream_standard_stream_creator_t *lexleo_vm_get_stream_standard_stream_creator(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm && vm->lexleo_vm_owned_resources);
	return vm->lexleo_vm_owned_resources->stream_standard_stream_creator;
}

stream_regular_file_creator_t *lexleo_vm_get_stream_regular_file_creator(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm && vm->lexleo_vm_owned_resources);
	return vm->lexleo_vm_owned_resources->stream_regular_file_creator;
}

stream_dynamic_buffer_creator_t *lexleo_vm_get_stream_dynamic_buffer_creator(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm && vm->lexleo_vm_owned_resources);
	return vm->lexleo_vm_owned_resources->stream_dynamic_buffer_creator;
}
