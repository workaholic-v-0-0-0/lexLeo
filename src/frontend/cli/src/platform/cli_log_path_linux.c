/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file cli_log_path_linux.c
 * @ingroup cli_internal_group
 * @brief Linux-specific CLI log-path services.
 *
 * @details
 * This translation unit provides the Linux implementation of the internal
 * CLI log-path services, including:
 * - the default application configuration path,
 * - the default log-file path resolution,
 * - and creation of the parent directory hierarchy for the target log file.
 */

#include "internal/cli_log_path.h"
#include "internal/cli_env.h"

#include "osal/str/osal_str.h"
#include "osal/file/osal_file_ops.h"
#include "osal/env/osal_env.h"

#include "policy/lexleo_cstd_lib.h"

#define CLI_DEFAULT_CONFIG_PATH "/etc/lexleo/lexleo.conf"

bool cli_platform_ensure_log_parent_dir_exists(
	const char *log_path,
	const cli_env_t *env
	)
{
	char dir[1024];
	char *p;
	int ret;
	osal_file_status_t st;

	if (!log_path || log_path[0] == '\0') {
		return false;
	}

	ret = osal_snprintf(dir, sizeof(dir), "%s", log_path);
	if (ret < 0 || (size_t)ret >= sizeof(dir)) {
		return false;
	}

	p = osal_strrchr(dir, '/');
	if (!p || p == dir) {
		return false;
	}

	*p = '\0';

	for (p = dir + 1; *p; ++p) {
		if (*p != '/')
			continue;

		*p = '\0';
		st = env->file_ops->mkdir(dir);
		if (st != OSAL_FILE_STATUS_OK && st != OSAL_FILE_STATUS_EXISTS) {
			*p = '/';
			return false;
		}

		*p = '/';
	}

	st = env->file_ops->mkdir(dir);
	if (st != OSAL_FILE_STATUS_OK && st != OSAL_FILE_STATUS_EXISTS)
		return false;

	return true;
}

const char *cli_platform_default_config_path(void)
{
	return CLI_DEFAULT_CONFIG_PATH;
}

bool cli_platform_resolve_default_log_path(char *buf, size_t buf_size)
{
	const char *home;
	int ret;

	if (!buf || buf_size == 0)
		return false;

	home = osal_getenv("HOME");
	if (!home || !*home)
		return false;

	ret = osal_snprintf(
		buf,
		buf_size,
		"%s/.local/state/lexleo/lexleo.log",
		home
	);

	if (ret < 0 || (size_t)ret >= buf_size)
		return false;

	return true;
}
