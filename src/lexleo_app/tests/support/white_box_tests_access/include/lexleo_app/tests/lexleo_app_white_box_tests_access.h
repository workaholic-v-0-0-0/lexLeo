/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_white_box_tests_access.h
 * @ingroup lexleo_app_white_box_tests_access_group
 * @brief White-box test access helpers for lexleo_app.
 *
 * @details
 * This header declares test-only helpers used to inject and observe
 * internal fields of LexLeo application handles.
 */

#ifndef LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_H
#define LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_H

#include "lexleo_app/common/lexleo_app_action_type.h"
#include "lexleo_app/common/lexleo_app_stream_owner_opaque_type_opaque.h"
#include "lexleo_app/borrowers/lexleo_app_borrowers_types.h"
#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_types.h"

#include "stream/common/stream_opaque_type.h"
#include "stream/common/stream_factory_opaque_type.h"
#include "stream/owners/stream_creators_api.h"

#include "logger/common/logger_opaque_type.h"

#include "lexleo_vm/borrowers/lexleo_vm_borrowers_types.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void lexleo_app_inject_in(lexleo_app_t *app, stream_t *in);
void lexleo_app_inject_out(lexleo_app_t *app, stream_t *out);
void lexleo_app_inject_err(lexleo_app_t *app, stream_t *err);
void lexleo_app_inject_log_path(lexleo_app_t *app, const char *log_path);
void lexleo_app_inject_logger(lexleo_app_t *app, logger_t *logger);
void lexleo_app_inject_logger_stream(
	lexleo_app_t *app,
	stream_t *logger_stream);
void lexleo_app_inject_stream_factory(
	lexleo_app_t *app,
	stream_factory_t *stream_factory);
void lexleo_app_inject_stream_standard_stream_creator(
	lexleo_app_t *app,
	stream_standard_stream_creator_t *stream_standard_stream_creator);
void lexleo_app_inject_stream_regular_file_creator(
	lexleo_app_t *app,
	stream_regular_file_creator_t *stream_regular_file_creator);
void lexleo_app_inject_stream_dynamic_buffer_creator(
	lexleo_app_t *app,
	stream_dynamic_buffer_creator_t *stream_dynamic_buffer_creator);
void lexleo_app_inject_vm_in(lexleo_app_t *app, stream_t *vm_in);
void lexleo_app_inject_vm_out(lexleo_app_t *app, stream_t *vm_out);
void lexleo_app_inject_vm(lexleo_app_t *app, lexleo_vm_t *vm);

const osal_mem_ops_t *lexleo_app_get_mem_ops(lexleo_app_t *app);
const osal_stdio_ops_t *lexleo_app_get_stdio_ops(lexleo_app_t *app);
const osal_file_ops_t *lexleo_app_get_file_ops(lexleo_app_t *app);
const osal_str_ops_t *lexleo_app_get_str_ops(lexleo_app_t *app);
const osal_time_ops_t *lexleo_app_get_time_ops(lexleo_app_t *app);
const char *const *lexleo_app_get_input_paths(lexleo_app_t *app);
const char *const *lexleo_app_get_input_dynamic_libraries(lexleo_app_t *app);
const char *lexleo_app_get_output_path(lexleo_app_t *app);
lexleo_app_action_t lexleo_app_get_action(lexleo_app_t *app);
stream_t *lexleo_app_get_in(lexleo_app_t *app);
stream_t *lexleo_app_get_out(lexleo_app_t *app);
stream_t *lexleo_app_get_err(lexleo_app_t *app);
char *lexleo_app_get_log_path(lexleo_app_t *app);
stream_t *lexleo_app_get_logger_stream(lexleo_app_t *app);
logger_t *lexleo_app_get_logger(lexleo_app_t *app);
lexleo_app_stream_owner_opaque_t *lexleo_app_get_stream_owner_opaque(lexleo_app_t *app);
stream_factory_t *lexleo_app_get_stream_factory(lexleo_app_t *app);
stream_standard_stream_creator_t *
	lexleo_app_get_stream_standard_stream_creator(lexleo_app_t *app);
stream_regular_file_creator_t *
	lexleo_app_get_stream_regular_file_creator(lexleo_app_t *app);
stream_dynamic_buffer_creator_t *
	lexleo_app_get_stream_dynamic_buffer_creator(lexleo_app_t *app);
stream_t *lexleo_app_get_vm_in(lexleo_app_t *app);
stream_t *lexleo_app_get_vm_out(lexleo_app_t *app);
lexleo_vm_t *lexleo_app_get_vm(lexleo_app_t *app);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_H */
