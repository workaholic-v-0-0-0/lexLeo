/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream.c
 * @ingroup stdio_stream_internal_group
 * @brief `stdio_stream` adapter implementation.
 *
 * @details
 * This module implements the `stdio_stream` adapter, which exposes the
 * process standard I/O streams (`stdin`, `stdout`, and `stderr`) through the
 * generic `stream` port.
 */

#include "internal/stdio_stream_handle.h"
#include "internal/stdio_stream_ctor_ud.h"

#include "stdio_stream/cr/stdio_stream_cr_api.h"

#include "stream/adapters/stream_env.h"

#include "osal/mem/osal_mem.h"

#include "policy/lexleo_assert.h"

stdio_stream_cfg_t stdio_stream_default_cfg(void) {
	return (stdio_stream_cfg_t){ .reserved = 0 };
}

stdio_stream_env_t stdio_stream_default_env(
	const osal_stdio_ops_t *stdio_ops,
	const osal_mem_ops_t *mem,
	const stream_env_t *port_env)
{
	LEXLEO_ASSERT(stdio_ops && mem && port_env);
	return
		(stdio_stream_env_t){
			.stdio_ops = stdio_ops,
			.mem = mem,
			.port_env = *port_env
		};
}

static size_t stdio_stream_read(
	void *backend,
	void *buf,
	size_t n,
	stream_status_t *st)
{
	if (!backend || (n > 0 && !buf)) {
		if (st) *st = STREAM_STATUS_INVALID;
		return 0;
	}

	stdio_stream_t *s = (stdio_stream_t *)backend;

	LEXLEO_ASSERT(
		   s->stdio
		&& s->stdio_ops
		&& s->stdio_ops->stdin
	);

	if (s->stdio != s->stdio_ops->stdin()) {
		if (st) *st = STREAM_STATUS_IO_ERROR;
		return 0;
	}

	if (st) *st = STREAM_STATUS_OK;

	if (n == 0)
		return 0;

	LEXLEO_ASSERT(s->stdio_ops->read);

	return s->stdio_ops->read(buf, 1, n, s->stdio);
}

static size_t stdio_stream_write(
	void *backend,
	const void *buf,
	size_t n,
	stream_status_t *st)
{
	if (!backend || (n > 0 && !buf)) {
		if (st) *st = STREAM_STATUS_INVALID;
		return 0;
	}

	stdio_stream_t *s = (stdio_stream_t *)backend;

	LEXLEO_ASSERT(
		   s->stdio
		&& s->stdio_ops
		&& s->stdio_ops->stdin
	);

	if (s->stdio == s->stdio_ops->stdin()) {
		if (st) *st = STREAM_STATUS_IO_ERROR;
		return 0;
	}

	if (st) *st = STREAM_STATUS_OK;

	if (n == 0)
		return 0;

	LEXLEO_ASSERT(s->stdio_ops->write);

	return s->stdio_ops->write(buf, 1, n, s->stdio);
}

static stream_status_t stdio_stream_flush(void *backend)
{
	if (!backend)
		return STREAM_STATUS_INVALID;

	stdio_stream_t *s = (stdio_stream_t *)backend;

	LEXLEO_ASSERT(
		   s->stdio
		&& s->stdio_ops
		&& s->stdio_ops->stdin
	);

	if (s->stdio == s->stdio_ops->stdin())
		return STREAM_STATUS_IO_ERROR;

	LEXLEO_ASSERT(s->stdio_ops->flush);

	s->stdio_ops->flush(s->stdio);

	return STREAM_STATUS_OK;
}

static stream_status_t stdio_stream_close(void *backend)
{
	if (!backend)
		return STREAM_STATUS_INVALID;

	stdio_stream_t *stdio_stream = (stdio_stream_t *)backend;

	LEXLEO_ASSERT(stdio_stream->mem_ops && stdio_stream->mem_ops->free);

	stdio_stream->mem_ops->free(stdio_stream);
	return STREAM_STATUS_OK;
}

static const stream_vtbl_t STDIO_STREAM_VTBL = {
	.read = stdio_stream_read,
	.write = stdio_stream_write,
	.flush = stdio_stream_flush,
	.close = stdio_stream_close
};

stream_status_t stdio_stream_create_stream(
	stream_t **out,
	const stdio_stream_args_t *args,
	const stdio_stream_cfg_t *cfg,
	const stdio_stream_env_t *env)
{
	if (
		   !out
		|| !args
		|| !cfg
		|| !env
		|| args->kind >= STDIO_STREAM_KIND_COUNT)
	{
		return STREAM_STATUS_INVALID;
	}

	LEXLEO_ASSERT(env->mem && env->mem->calloc);

	stdio_stream_t *stdio_stream = env->mem->calloc(1, sizeof(*stdio_stream));
	if (!stdio_stream) {
		return STREAM_STATUS_OOM;
	}

	const osal_stdio_ops_t *stdio_ops = env->stdio_ops;

	stdio_stream->stdio_ops = stdio_ops;
	stdio_stream->mem_ops = env->mem;

	LEXLEO_ASSERT(
		   stdio_ops
		&& stdio_ops->stdin
		&& stdio_ops->stdout
		&& stdio_ops->stderr
	);

	switch (args->kind) {
		case STDIO_STREAM_KIND_STDIN:
			stdio_stream->stdio = stdio_ops->stdin();
			break;
		case STDIO_STREAM_KIND_STDOUT:
			stdio_stream->stdio = stdio_ops->stdout();
			break;
		case STDIO_STREAM_KIND_STDERR:
			stdio_stream->stdio = stdio_ops->stderr();
			break;
		default:
			LEXLEO_ASSERT(stdio_stream->mem_ops &&  stdio_stream->mem_ops->free);
			stdio_stream->mem_ops->free(stdio_stream);
			return STREAM_STATUS_INVALID;
	}

	stream_t *tmp = NULL;

	stream_status_t st =
		stream_create(
			&tmp,
			&STDIO_STREAM_VTBL,
			stdio_stream,
			&env->port_env);

	if (st != STREAM_STATUS_OK) {
		LEXLEO_ASSERT(stdio_stream->mem_ops &&  stdio_stream->mem_ops->free);
		stdio_stream->mem_ops->free(stdio_stream);
		return st;
	}

	if (!tmp) {
		LEXLEO_ASSERT(stdio_stream->mem_ops &&  stdio_stream->mem_ops->free);
		stdio_stream->mem_ops->free(stdio_stream);
		return STREAM_STATUS_OOM;
	}

	*out = tmp;

	return STREAM_STATUS_OK;
}

static void stdio_stream_destroy_ud_ctor(
	const void *ud,
	const osal_mem_ops_t *mem)
{
	if (!ud) {
		return;
	}

	LEXLEO_ASSERT(mem && mem->free);
	mem->free((void *)ud);
}

stream_status_t stdio_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const stdio_stream_cfg_t *cfg,
	const stdio_stream_env_t *env,
	const osal_mem_ops_t *mem)
{
	if (out) {
		*out = (stream_adapter_desc_t){0};
	}

	stream_adapter_desc_t tmp = {0};

	if (
		   !out
		|| !key
		|| *key == '\0'
		|| !cfg
		|| !env
		|| !mem)
	{
		return STREAM_STATUS_INVALID;
	}

	LEXLEO_ASSERT(mem->calloc && mem->free);

	tmp.key = key;
	tmp.ctor = stdio_stream_ctor;

	stdio_stream_ctor_ud_t *ud =
		(stdio_stream_ctor_ud_t *)mem->calloc(1, sizeof(*ud));
	if (!ud) {
		return STREAM_STATUS_OOM;
	}

	tmp.ud = ud;
	osal_memcpy(&ud->cfg, cfg, sizeof(*cfg));
	osal_memcpy(&ud->env, env, sizeof(*env));
	tmp.ud_dtor = stdio_stream_destroy_ud_ctor;

	*out = tmp;
	return STREAM_STATUS_OK;
}

stream_status_t stdio_stream_ctor(
	const void *ud,
	const void *args,
	stream_t **out)
{
	const stdio_stream_ctor_ud_t *ctor_ud =
		(const stdio_stream_ctor_ud_t *)ud;

	if (
		   !args
		|| !out
		|| !ctor_ud)
	{
		return STREAM_STATUS_INVALID;
	}

	stream_t *tmp = NULL;
	stream_status_t st =
		stdio_stream_create_stream(
				&tmp,
				(const stdio_stream_args_t *)args,
			&ctor_ud->cfg,
			&ctor_ud->env);

	if (st != STREAM_STATUS_OK) {
		return st;
	}

	*out = tmp;
	return STREAM_STATUS_OK;
}
