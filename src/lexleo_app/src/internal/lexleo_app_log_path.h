/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_log_path.h
 * @ingroup lexleo_app_internal_group
 * @brief Internal services for log-path resolution.
 *
 * @details
 * This header declares the internal services used to resolve the effective
 * log-file path, access platform-specific defaults, and ensure that the
 * parent directory of the target log file exists.
 */

#ifndef LEXLEO_APP_LOG_PATH_H
#define LEXLEO_APP_LOG_PATH_H

#include "osal/file/osal_file_types.h"
#include "osal/mem/osal_mem_types.h"

#include "policy/lexleo_cstd_types.h"

#define LEXLEO_APP_LOG_PATH_BUFFER_SIZE 1024

bool lexleo_app_platform_ensure_log_parent_dir_exists(
	const char *log_path,
	const osal_file_ops_t *file_ops);
const char *lexleo_app_platform_default_config_path(void);
bool lexleo_app_platform_resolve_default_log_path(char* out, size_t out_size);
bool lexleo_app_resolve_log_path(
	char *out,
	size_t out_size,
	const osal_mem_ops_t *mem_ops,
	const osal_file_ops_t *file_ops);

#endif // LEXLEO_APP_LOG_PATH_H
