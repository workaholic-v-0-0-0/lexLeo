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

#include "policy/lexleo_assert.h"

void lexleo_vm_inject_stream_factory(
	lexleo_vm_t *vm,
	stream_factory_t *stream_factory
) {
	LEXLEO_ASSERT(vm);
	vm->stream_factory = stream_factory;
}

void lexleo_vm_inject_stream_io_creator(
	lexleo_vm_t *vm,
	stream_io_creator_t *stream_io_creator
) {
	LEXLEO_ASSERT(vm);
	vm->stream_io_creator = stream_io_creator;
}

void lexleo_vm_inject_stream_file_creator(
	lexleo_vm_t *vm,
	stream_file_creator_t *stream_file_creator
) {
	LEXLEO_ASSERT(vm);
	vm->stream_file_creator = stream_file_creator;
}

void lexleo_vm_inject_stream_buffer_creator(
	lexleo_vm_t *vm,
	stream_buffer_creator_t *stream_buffer_creator
) {
	LEXLEO_ASSERT(vm);
	vm->stream_buffer_creator = stream_buffer_creator;
}

stream_factory_t *lexleo_vm_get_stream_factory(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->stream_factory;
}

stream_io_creator_t *lexleo_vm_get_stream_io_creator(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->stream_io_creator;
}

stream_file_creator_t *lexleo_vm_get_stream_file_creator(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->stream_file_creator;
}

stream_buffer_creator_t *lexleo_vm_get_stream_buffer_creator(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->stream_buffer_creator;
}
