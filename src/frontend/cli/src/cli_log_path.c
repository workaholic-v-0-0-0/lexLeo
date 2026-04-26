/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file cli_log_path.c
 * @ingroup cli_internal_group
 * @brief Common CLI log-path resolution services.
 *
 * @details
 * This translation unit implements the platform-independent part of the
 * internal CLI log-path services.
 *
 * It provides:
 * - parsing of the application configuration file to read `log_path`,
 * - fallback to the platform-default log-file path when needed,
 * - and delegation to platform-specific helpers for default-path discovery
 *   and parent-directory preparation.
 */

#include "internal/cli_log_path.h"
#include "internal/cli_env.h"

#include "osal/file/osal_file_ops.h"
#include "osal/str/osal_str_ops.h"
#include "osal/str/osal_str.h"
#include "osal/stdio/osal_stdio.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem.h"

static bool cli_trim_in_place(
	char* s,
	const cli_env_t *env
) {
	char *begin;
	char *end;

	if (!s)
		return false;

	begin = s;
	while (*begin && osal_isspace((unsigned char)*begin)) begin++;

	if (begin != s) {
		osal_memmove(
			s,
			begin,
			osal_strlen(begin) + 1);
	}

	if (*s == '\0')
		return true;

	end = s + osal_strlen(s) - 1;
	while (osal_isspace((unsigned char)*end)) {
		*end = '\0';
		end--;
	}

	return true;
}

static bool cli_copy_string(
	char *out,
	size_t out_size,
	const char *src,
	const cli_env_t *env
) {
	int ret;

	if (!out || out_size == 0 || !src)
		return false;

	ret = osal_snprintf(out, out_size, "%s", src);
	if (ret < 0 || (size_t)ret >= out_size)
		return false;

	return true;
}

static bool cli_try_read_log_path_from_config(
	char* out,
	size_t out_size,
	const char *config_path,
	const cli_env_t *env
) {
	if (!out || out_size == 0 || !config_path)
		return false;

	OSAL_FILE *cfg_file = NULL;
	osal_file_status_t st =
		env->file_ops->open(
			&cfg_file,
			config_path,
			"rb",
			env->mem_ops
		);

	if (st != OSAL_FILE_STATUS_OK)
		return false;

	char line[255] = { 0 };
	bool in_logger_section = false;
	while (env->file_ops->gets(line, sizeof(line), cfg_file, &st)) {
		if (st != OSAL_FILE_STATUS_OK) {
			(void)env->file_ops->close(cfg_file);
			return false;
		}

		if (!cli_trim_in_place(line, env))
			continue;

		if (line[0] == '\0' || line[0] == ';' || line[0] == '#')
			continue;

		if (line[0] == '[') {
			in_logger_section = (osal_strcmp(line, "[logger]") == 0);
			continue;
		}

		if (!in_logger_section)
			continue;

		char* eq = osal_strchr(line, '=');
		if (!eq)
			continue;

		*eq = '\0';
		char* key = line;
		char* val = eq + 1;

		if (!cli_trim_in_place(key, env) || !cli_trim_in_place(val, env)) {
			(void)env->file_ops->close(cfg_file);
			return false;
		}

		if (osal_strcmp(key, "log_path") != 0)
			continue;

		if (val[0] == '\0') {
			(void)env->file_ops->close(cfg_file);
			return false;
		}

		if (val[0] == '"' || val[0] == '\'') {
			size_t len = osal_strlen(val);
			if (len >= 2 && val[len - 1] == val[0]) {
				val[len - 1] = '\0';
				val++;
			}
		}

		(void)env->file_ops->close(cfg_file);
		return cli_copy_string(out, out_size, val, env);
	}

	(void)env->file_ops->close(cfg_file);
	return false;
}

/* cli_resolve_log_path() tries to read log_path from the application
   configuration, and falls back to the platform-default path only
   when no valid configured path is provided. */
bool cli_resolve_log_path(
	char *out,
	size_t out_size,
	const cli_env_t *env
) {
	if (!out || out_size == 0)
		return false;

	out[0] = '\0';
	const char *config_path = cli_platform_default_config_path();

	if (!cli_try_read_log_path_from_config(out, out_size, config_path, env)) {
		if (!cli_platform_resolve_default_log_path(out, out_size))
			return false;
	}

	return cli_platform_ensure_log_parent_dir_exists(out, env);
}
