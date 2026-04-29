/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_stdio.c
 * @ingroup osal_stdio_internal_group
 * @brief Default implementation of the `osal_stdio` module.
 *
 * @details
 * This file implements the default standard I/O backend and exposes it through
 * the injectable `osal_stdio_ops_t` operations table.
 */

#include "internal/osal_stdio_internal.h"

#include "osal/stdio/osal_stdio_ops.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_assert.h"
#include "policy/lexleo_cstd_io.h"
#include "policy/lexleo_cstd_arg.h"

static struct OSAL_STDIO g_stdin  = { .fp = NULL };
static struct OSAL_STDIO g_stdout = { .fp = NULL };
static struct OSAL_STDIO g_stderr = { .fp = NULL };

static OSAL_STDIO *osal_stdio_stdin(void)
{
	if (!g_stdin.fp) g_stdin.fp = stdin;
	return &g_stdin;
}

static OSAL_STDIO *osal_stdio_stdout(void)
{
	if (!g_stdout.fp) g_stdout.fp = stdout;
	return &g_stdout;
}

static OSAL_STDIO *osal_stdio_stderr(void)
{
	if (!g_stderr.fp) g_stderr.fp = stderr;
	return &g_stderr;
}

static size_t osal_stdio_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio)
{
	LEXLEO_ASSERT(stdio);
	return fread(ptr, size, nmemb, stdio->fp);
}

static size_t osal_stdio_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio)
{
	LEXLEO_ASSERT(stdio);
	return fwrite(ptr, size, nmemb, stdio->fp);
}

static int osal_stdio_flush(OSAL_STDIO *stdio)
{
	LEXLEO_ASSERT(stdio);
	return fflush(stdio->fp);
}

static int osal_stdio_error(OSAL_STDIO *stdio)
{
	LEXLEO_ASSERT(stdio);
	return ferror(stdio->fp);
}

static int osal_stdio_eof(OSAL_STDIO *stdio)
{
	LEXLEO_ASSERT(stdio);
	return feof(stdio->fp);
}

const osal_stdio_ops_t *osal_stdio_default_ops(void)
{
	static const osal_stdio_ops_t OPS = {
		.get_stdin = osal_stdio_stdin,
		.get_stdout = osal_stdio_stdout,
		.get_stderr = osal_stdio_stderr,
		.read = osal_stdio_read,
		.write = osal_stdio_write,
		.flush = osal_stdio_flush,
		.error = osal_stdio_error,
		.eof = osal_stdio_eof,
	};
	return &OPS;
}
