/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file cli.c
 * @ingroup cli_group
 * @brief Minimal packaged CLI bootstrap for `lexleo`.
 *
 * @details
 * This translation unit implements the current command-line entry behavior for
 * packaged `lexleo` builds.
 *
 * The current implementation:
 * - resolves the effective log-file path,
 * - opens or creates the target log file,
 * - writes a probe entry,
 * - flushes and closes the file,
 * - and reports the resolved path on standard output.
 *
 * This behavior is intentionally minimal and may evolve as the CLI module
 * grows.
 */

#include "internal/cli_env.h"
#include "internal/cli_log_path.h"

#include "cli.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/stdio/osal_stdio_ops.h"
#include "osal/file/osal_file_ops.h"
#include "osal/str/osal_str_ops.h"
#include "osal/str/osal_str.h"
#include "osal/time/osal_time_ops.h"

#include "policy/lexleo_assert.h"

#define CLI_LOG_PATH_BUFFER_SIZE 1024

static bool cli_cr_init(cli_env_t *out)
{
	if (!out)
		return false;

	out->mem_ops = osal_mem_default_ops();
	out->stdio_ops = osal_stdio_default_ops();
	out->file_ops = osal_file_default_ops();
	out->str_ops = osal_str_default_ops();
	out->time_ops = osal_time_default_ops();
}

/* Minimal packaging bootstrap:
   at this stage, the packaged CLI only resolves the log path,
   creates/appends the log file, writes a probe message, and reports
   the resolved path on standard output. */
static int cli_run(const cli_env_t *env)
{
	if (!env) {
		return 1;
	}

	LEXLEO_ASSERT(
		   env->mem_ops
		&& env->stdio_ops
		&& env->file_ops
		&& env->str_ops
		&& env->time_ops
	);

	OSAL_STDIO *out = env->stdio_ops->get_stdout();

	char log_path[CLI_LOG_PATH_BUFFER_SIZE] = { 0 };
	bool ok = cli_resolve_log_path(log_path, sizeof(log_path), env);

	(void)env->stdio_ops->write("LexLeo CLI started\n", 1, 19, out);

	if (!ok) {
		(void)env->stdio_ops->write("Failed to resolve log path\n", 1, 27, out);
		(void)env->stdio_ops->flush(out);
		return 1;
	}

	OSAL_FILE *log_file = NULL;
	osal_file_status_t st = env->file_ops->open(&log_file, log_path, "ab", env->mem_ops);
	if (st != OSAL_FILE_STATUS_OK) {
		(void)env->stdio_ops->write("Failed to open log file: ", 1, 25, out);
		(void)env->stdio_ops->write(log_path, 1, osal_strlen(log_path), out);
		(void)env->stdio_ops->write("\n", 1, 1, out);
		(void)env->stdio_ops->flush(out);
		return 1;
	}

	static const char probe[] = "LexLeo probe log created successfully\n";

	size_t written =
		env->file_ops->write(
			probe,
			1,
			sizeof(probe) - 1,
			log_file,
			&st
		);

	if (st != OSAL_FILE_STATUS_OK || written != sizeof(probe) - 1) {
		(void)env->file_ops->close(log_file);
		(void)env->stdio_ops->write("Failed to write probe log\n", 1, 26, out);
		(void)env->stdio_ops->flush(out);
		return 1;
	}

	st = env->file_ops->flush(log_file);
	if (st != OSAL_FILE_STATUS_OK) {
		(void)env->file_ops->close(log_file);
		(void)env->stdio_ops->write("Failed to flush probe log\n", 1, 26, out);
		(void)env->stdio_ops->flush(out);
		return 1;
	}

	st = env->file_ops->close(log_file);
	if (st != OSAL_FILE_STATUS_OK) {
		(void)env->stdio_ops->write("Failed to close probe log\n", 1, 26, out);
		(void)env->stdio_ops->flush(out);
		return 1;
	}

	(void)env->stdio_ops->write("Log file created at: ", 1, 21, out);
	(void)env->stdio_ops->write(log_path, 1, osal_strlen(log_path), out);
	(void)env->stdio_ops->write("\n", 1, 1, out);
	(void)env->stdio_ops->flush(out);

	return 0;
}

int cli_main(void)
{
	cli_env_t env;

	if (!cli_cr_init(&env)) {
		return 1;
	}

	return cli_run(&env);
}