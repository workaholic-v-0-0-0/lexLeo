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

#include "stream/cr/stream_cr_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void lexleo_vm_inject_stream_factory(
	lexleo_vm_t *vm,
	stream_factory_t *stream_factory);
void lexleo_vm_inject_stream_io_creator(
	lexleo_vm_t *vm,
	stream_io_creator_t *stream_io_creator);
void lexleo_vm_inject_stream_file_creator(
	lexleo_vm_t *vm,
	stream_file_creator_t *stream_file_creator);
void lexleo_vm_inject_stream_buffer_creator(
	lexleo_vm_t *vm,
	stream_buffer_creator_t *stream_buffer_creator);

stream_factory_t *lexleo_vm_get_stream_factory(lexleo_vm_t *vm);
stream_io_creator_t *lexleo_vm_get_stream_io_creator(lexleo_vm_t *vm);
stream_file_creator_t *lexleo_vm_get_stream_file_creator(lexleo_vm_t *vm);
stream_buffer_creator_t *lexleo_vm_get_stream_buffer_creator(lexleo_vm_t *vm);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_VM_WHITE_BOX_TESTS_ACCESS_H */
