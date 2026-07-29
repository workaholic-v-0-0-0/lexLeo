/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_handle.h
 * @ingroup lexleo_app_internal_group
 * @brief Private application handle representation for lexleo_app.
 *
 * @details
 * Defines the application handle fields storing borrowed runtime operations,
 * owned streams, logger and VM, and the log-path buffer.
 */

#ifndef LEXLEO_LEXLEO_APP_HANDLE_H
#define LEXLEO_LEXLEO_APP_HANDLE_H

#include "lexleo_app/common/lexleo_app_limits.h"
#include "lexleo_app/common/lexleo_app_action_type.h"
#include "lexleo_app/common/lexleo_app_stream_owner_opaque_type_opaque.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#include "stream/common/stream_opaque_type.h"

#include "logger/common/logger_opaque_type.h"

#include "lexleo_vm/borrowers/lexleo_vm_borrowers_types.h"

#define LEXLEO_APP_LOG_PATH_BUFFER_SIZE 1024

struct lexleo_app_t {

	const osal_mem_ops_t *mem_ops;
	const osal_stdio_ops_t *stdio_ops;
	const osal_file_ops_t *file_ops;
	const osal_str_ops_t *str_ops;
	const osal_time_ops_t *time_ops;

	const char *input_paths[LEXLEO_APP_MAX_INPUT_PATHS];
	const char *input_dynamic_libraries[LEXLEO_APP_MAX_INPUT_DYN_LIB];
	const char *output_path;

	lexleo_app_action_t action;

	stream_t *in;
	stream_t *out;
	stream_t *err;

	char log_path[LEXLEO_APP_LOG_PATH_BUFFER_SIZE];
	stream_t *logger_stream;
	logger_t *logger;

	lexleo_app_stream_owner_opaque_t *stream_owner_opaque;

	stream_t *vm_in;
	stream_t *vm_out;
	lexleo_vm_t *vm;

};

#endif /* LEXLEO_LEXLEO_APP_HANDLE_H */
