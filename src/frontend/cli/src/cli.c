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

#include "internal/cli_log_path.h"

#include "cli.h"

#include "osal/file/osal_file_ops.h"
#include "osal/mem/osal_mem.h"
#include "osal/stdio/osal_stdio_ops.h"

#define CLI_LOG_PATH_BUFFER_SIZE 1024

/* Minimal packaging bootstrap:
   at this stage, the packaged CLI only resolves the log path,
   creates/appends the log file, writes a probe message, and reports
   the resolved path on standard output. */
int cli_run(void)
{
	const osal_stdio_ops_t *stdio_ops = osal_stdio_default_ops();
	const osal_file_ops_t *file_ops = osal_file_default_ops();
	const osal_mem_ops_t *mem_ops = osal_mem_default_ops();
	OSAL_STDIO *out = stdio_ops->get_stdout();

	char log_path[CLI_LOG_PATH_BUFFER_SIZE] = { 0 };
	bool ok = cli_resolve_log_path(log_path, sizeof(log_path));

	(void)stdio_ops->write("LexLeo CLI started\n", 1, 19, out);

	if (!ok) {
		(void)stdio_ops->write("Failed to resolve log path\n", 1, 27, out);
		(void)stdio_ops->flush(out);
		return 1;
	}

	OSAL_FILE *log_file = NULL;
	osal_file_status_t st = file_ops->open(&log_file, log_path, "ab", mem_ops);
	if (st != OSAL_FILE_STATUS_OK) {
		(void)stdio_ops->write("Failed to open log file: ", 1, 25, out);
		(void)stdio_ops->write(log_path, 1, osal_strlen(log_path), out);
		(void)stdio_ops->write("\n", 1, 1, out);
		(void)stdio_ops->flush(out);
		return 1;
	}

	static const char probe[] = "LexLeo probe log created successfully\n";

	size_t written = file_ops->write(
		probe,
		1,
		sizeof(probe) - 1,
		log_file,
		&st
	);

	if (st != OSAL_FILE_STATUS_OK || written != sizeof(probe) - 1) {
		(void)file_ops->close(log_file);
		(void)stdio_ops->write("Failed to write probe log\n", 1, 26, out);
		(void)stdio_ops->flush(out);
		return 1;
	}

	st = file_ops->flush(log_file);
	if (st != OSAL_FILE_STATUS_OK) {
		(void)file_ops->close(log_file);
		(void)stdio_ops->write("Failed to flush probe log\n", 1, 26, out);
		(void)stdio_ops->flush(out);
		return 1;
	}

	st = file_ops->close(log_file);
	if (st != OSAL_FILE_STATUS_OK) {
		(void)stdio_ops->write("Failed to close probe log\n", 1, 26, out);
		(void)stdio_ops->flush(out);
		return 1;
	}

	(void)stdio_ops->write("Log file created at: ", 1, 21, out);
	(void)stdio_ops->write(log_path, 1, osal_strlen(log_path), out);
	(void)stdio_ops->write("\n", 1, 1, out);
	(void)stdio_ops->flush(out);

	return 0;
}
