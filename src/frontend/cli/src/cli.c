/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file cli.c
 * @ingroup cli_group
 * @brief Minimal packaged CLI bootstrap for `lexleo`.
 *
 * @details
 * todo
 */

#include "cli.h"

#include "lexleo_app/lexleo_app.h"

int cli_main(void)
{
	lexleo_app_t *app;

	lexleo_app_cfg_t app_cfg = lexleo_app_default_cfg();

	if
	(
		lexleo_app_create(
			&app,
			&app_cfg
		)
		!=
		LEXLEO_APP_STATUS_OK
	) {
		return 1;
	}

	if (lexleo_app_complete_default_init(app) != LEXLEO_APP_STATUS_OK) {
		lexleo_app_destroy(&app);
		return 1;
	}

	if (lexleo_app_run(app) != LEXLEO_APP_STATUS_OK) {
		lexleo_app_destroy(&app);
		return 1;
	}

	lexleo_app_destroy(&app);

	return 0;
}

// follow legacy

/*
static bool cli_create_app(lexleo_app_t **out)
{
	if (!out) {
		return false;
	}

	osal_mem_ops_t *mem_ops = osal_mem_default_ops();

	LEXLEO_ASSERT(mem_ops && mem_ops->calloc);

	lexleo_app_t *tmp = mem_ops->calloc(1, sizeof(*tmp));

	tmp->mem_ops = mem_ops;
	tmp->stdio_ops = osal_stdio_default_ops();
	tmp->file_ops = osal_file_default_ops();
	tmp->str_ops = osal_str_default_ops();
	tmp->time_ops = osal_time_default_ops();

	tmp->stdin = tmp->stdio_ops->get_stdin();
	tmp->stdout = tmp->stdio_ops->get_stdout();
	tmp->stderr = tmp->stdio_ops->get_stderr();

	stream_t *logger_stream = NULL;
	char log_path[CLI_LOG_PATH_BUFFER_SIZE] = { 0 };
	bool ret =
		lexleo_app_resolve_log_path(
			&log_path,
			sizeof(log_path),
			tmp->mem_ops,
			tmp->file_ops
		);
	const stream_file_creator_args_t logger_args = {
		.path = log_path,
		.mode = "ab"
	};
	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	stream_env_t stream_env = stream_default_env(tmp->mem_ops);
	fs_stream_env_t fs_stream_env =
		fs_stream_default_env(
			tmp->file_ops,
			tmp->mem_ops,
			&stream_env
		);
	stream_status_t stream_status =
		fs_stream_create_stream(
			&logger_stream,
			&logger_args,
			&fs_stream_cfg,
			&fs_stream_env
		);
	logger_default_cfg_t logger_default_cfg = logger_default_default_cfg();
	logger_env_t logger_env = logger_default_env(tmp->mem_ops);
	logger_default_env_t logger_default_env =
		logger_default_default_env(
			logger_stream,
			tmp->time_ops,
			tmp->mem_ops,
			&logger_env
		);
	logger_status_t logger_status =
		logger_default_create_logger(
			&tmp->logger,
			&logger_default_cfg,
			&logger_default_env
		);

	// init tmp->vm ; first migrate lexleo_vm is usual pattern of handle-based

	return true;
}
*/

/* Minimal packaging bootstrap:
   at this stage, the packaged CLI only resolves the log path,
   creates/appends the log file, writes a probe message, and reports
   the resolved path on standard output. */
/*
static int cli_run_app(const lexleo_app_t *env)
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
	bool ok = lexleo_app_resolve_log_path(log_path, sizeof(log_path), env->mem_ops, env->file_ops);

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
*/
