/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_white_box_tests_access.h
 * @ingroup lexleo_vm_white_box_tests_access_group
 * @brief White-box test access helpers for LexLeo VM handles.
 *
 * @details
 * This header declares test-only helpers used to inject and observe selected
 * internal owned-resource fields of LexLeo VM handles.
 *
 * These helpers are not part of the production API.
 */

#ifndef LEXLEO_VM_WHITE_BOX_TESTS_ACCESS_H
#define LEXLEO_VM_WHITE_BOX_TESTS_ACCESS_H

#include "lexleo_vm/borrowers/lexleo_vm_types.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#include "stream/common/stream_opaque_type.h"
#include "stream/common/stream_factory_opaque_type.h"
#include "stream/owners/stream_creators_api.h"

#include "logger/common/logger_opaque_type.h"

#ifdef __cplusplus
extern "C" {
#endif

const osal_mem_ops_t *lexleo_vm_get_mem_ops(lexleo_vm_t *vm);
const osal_stdio_ops_t *lexleo_vm_get_stdio_ops(lexleo_vm_t *vm);
const osal_file_ops_t *lexleo_vm_get_file_ops(lexleo_vm_t *vm);
const osal_str_ops_t *lexleo_vm_get_str_ops(lexleo_vm_t *vm);
const osal_time_ops_t *lexleo_vm_get_time_ops(lexleo_vm_t *vm);
stream_t *lexleo_vm_get_in(lexleo_vm_t *vm);
stream_t *lexleo_vm_get_out(lexleo_vm_t *vm);
stream_t *lexleo_vm_get_err(lexleo_vm_t *vm);
logger_t *lexleo_vm_get_logger(lexleo_vm_t *vm);

void lexleo_vm_inject_stream_factory(
	lexleo_vm_t *vm,
	stream_factory_t *stream_factory);
void lexleo_vm_inject_stream_standard_stream_creator(
	lexleo_vm_t *vm,
	stream_standard_stream_creator_t *stream_standard_stream_creator);
void lexleo_vm_inject_stream_regular_file_creator(
	lexleo_vm_t *vm,
	stream_regular_file_creator_t *stream_regular_file_creator);
void lexleo_vm_inject_stream_dynamic_buffer_creator(
	lexleo_vm_t *vm,
	stream_dynamic_buffer_creator_t *stream_dynamic_buffer_creator);

stream_factory_t *lexleo_vm_get_stream_factory(lexleo_vm_t *vm);
stream_standard_stream_creator_t *lexleo_vm_get_stream_standard_stream_creator(lexleo_vm_t *vm);
stream_regular_file_creator_t *lexleo_vm_get_stream_regular_file_creator(lexleo_vm_t *vm);
stream_dynamic_buffer_creator_t *lexleo_vm_get_stream_dynamic_buffer_creator(lexleo_vm_t *vm);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_VM_WHITE_BOX_TESTS_ACCESS_H */
