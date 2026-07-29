/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_white_box_tests_access.c
 * @ingroup lexleo_vm_white_box_tests_access_group
 * @brief White-box test access helper implementation for the LexLeo VM module.
 *
 * @details
 * This file implements test-only helpers used to inject and observe
 * internal fields of LexLeo VM handles.
 */

#include "internal/lexleo_vm_handle.h"
#include "internal/lexleo_vm_owned_resources_type.h"

#include "lexleo_vm/tests/lexleo_vm_white_box_tests_access.h"

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

logger_t *lexleo_vm_get_logger(lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(vm);
	return vm->logger;
}
