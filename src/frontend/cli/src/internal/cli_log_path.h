/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file cli_log_path.h
 * @ingroup cli_internal_group
 * @brief Internal services for CLI log-path resolution.
 *
 * @details
 * This header declares the internal services used to resolve the effective
 * CLI log-file path, access platform-specific defaults, and ensure that the
 * parent directory of the target log file exists.
 */

#ifndef LEXLEO_CLI_LOG_PATH_H
#define LEXLEO_CLI_LOG_PATH_H

#include "internal/cli_env.h"

#include "policy/lexleo_cstd_types.h"

bool cli_platform_ensure_log_parent_dir_exists(const char *log_path, const cli_env_t *env);
const char *cli_platform_default_config_path(void);
bool cli_platform_resolve_default_log_path(char* out, size_t out_size);
bool cli_resolve_log_path(char *out, size_t out_size, const cli_env_t *env);

#endif // LEXLEO_CLI_LOG_PATH_H
