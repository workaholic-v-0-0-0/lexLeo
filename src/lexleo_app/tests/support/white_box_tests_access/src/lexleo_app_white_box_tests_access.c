/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_white_box_tests_access.c
 * @ingroup lexleo_app_white_box_tests_access_group
 * @brief White-box test access helper implementation for the LexLeo
 * application module.
 *
 * @details
 * This file implements test-only helpers used to inject and observe
 * internal fields of LexLeo application handles.
 */

#include "lexleo_app/tests/lexleo_app_white_box_tests_access.h"

#include "internal/lexleo_app_handle.h"

#include "osal/str/osal_str.h"
#include "osal/mem/osal_mem.h"

#include "policy/lexleo_assert.h"

void lexleo_app_inject_in(lexleo_app_t *app, stream_t *in)
{
	LEXLEO_ASSERT(app);
	app->in = in;
}

void lexleo_app_inject_out(lexleo_app_t *app, stream_t *out)
{
	LEXLEO_ASSERT(app);
	app->out = out;
}

void lexleo_app_inject_err(lexleo_app_t *app, stream_t *err)
{
	LEXLEO_ASSERT(app);
	app->err = err;
}

void lexleo_app_set_log_path(lexleo_app_t *app, const char *log_path)
{
	LEXLEO_ASSERT(
		   app
		&& log_path
		&& osal_strlen(log_path) < sizeof(app->log_path)
	);
	osal_memcpy(app->log_path, log_path, osal_strlen(log_path) + 1);
}

void lexleo_app_inject_logger(lexleo_app_t *app, logger_t *logger)
{
	LEXLEO_ASSERT(app);
	app->logger = logger;
}

void lexleo_app_inject_logger_stream(
	lexleo_app_t *app,
	stream_t *logger_stream
) {
	LEXLEO_ASSERT(app);
	app->logger_stream = logger_stream;
}

void lexleo_app_inject_vm(lexleo_app_t *app, lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(app);
	app->vm = vm;
}

const osal_mem_ops_t *lexleo_app_get_mem_ops(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->mem_ops;
}

const osal_stdio_ops_t *lexleo_app_get_stdio_ops(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->stdio_ops;
}

const osal_file_ops_t *lexleo_app_get_file_ops(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->file_ops;
}

const osal_str_ops_t *lexleo_app_get_str_ops(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->str_ops;
}

const osal_time_ops_t *lexleo_app_get_time_ops(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->time_ops;
}

stream_t *lexleo_app_get_in(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->in;
}

stream_t *lexleo_app_get_out(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->out;
}

stream_t *lexleo_app_get_err(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->err;
}

char *lexleo_app_get_log_path(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->log_path;
}

stream_t *lexleo_app_get_logger_stream(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->logger_stream;
}

logger_t *lexleo_app_get_logger(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->logger;
}

lexleo_vm_t *lexleo_app_get_vm(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);
	return app->vm;
}
