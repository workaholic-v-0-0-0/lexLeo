/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream.c
 * @ingroup stdio_stream_internal_group
 * @brief `stdio_stream` adapter implementation.
 *
 * @details
 * Standard-I/O-backed implementation of the `stream` port.
 */

#include "internal/stdio_stream_handle.h"
#include "internal/stdio_stream_ctor_ud.h"

#include "stdio_stream/cr/stdio_stream_cr_api.h"

#include "stream/adapters/stream_adapters_api.h"

#include "osal/mem/osal_mem.h"

#include "policy/lexleo_assert.h"

stdio_stream_cfg_t stdio_stream_default_cfg(void)
{
	return (stdio_stream_cfg_t){ .reserved = 0 };
}

stdio_stream_env_t stdio_stream_default_env(
	const osal_stdio_ops_t *stdio_ops,
	const osal_mem_ops_t *adapter_mem_ops,
	const osal_mem_ops_t *port_mem_ops)
{
	LEXLEO_ASSERT(stdio_ops && adapter_mem_ops && port_mem_ops);
	return
		(stdio_stream_env_t){
			.stdio_ops = stdio_ops,
			.adapter_mem_ops = adapter_mem_ops,
			.port_mem_ops = port_mem_ops
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
		&& s->stdio_ops->get_stdin
	);

	if (s->stdio != s->stdio_ops->get_stdin()) {
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
		&& s->stdio_ops->get_stdin
	);

	if (s->stdio == s->stdio_ops->get_stdin()) {
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
		&& s->stdio_ops->get_stdin
	);

	if (s->stdio == s->stdio_ops->get_stdin())
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

static const stream_vtbl_t g_stdio_stream_vtbl = {
	.read = stdio_stream_read,
	.write = stdio_stream_write,
	.flush = stdio_stream_flush,
	.close = stdio_stream_close
};

static stream_status_t stdio_stream_create_backend(
	stdio_stream_t **out,
	const stream_io_creator_args_t *args,
	const stdio_stream_cfg_t *cfg,
	const stdio_stream_env_t *env
) {
	(void)cfg; // reserved for future use

	if (
		   !out
		|| !args
		|| (
			   args->kind != STREAM_IO_INPUT
			&& args->kind != STREAM_IO_OUTPUT
			&& args->kind != STREAM_IO_ERR)
		|| !cfg
		|| !env
		|| !env->stdio_ops
		|| !env->adapter_mem_ops
	) {
		return STREAM_STATUS_INVALID;
	}

	LEXLEO_ASSERT(
		   env->stdio_ops->get_stdin
		&& env->stdio_ops->get_stdout
		&& env->stdio_ops->get_stderr
		&& env->adapter_mem_ops->calloc
		&& env->adapter_mem_ops->free
	);

	stdio_stream_t *backend =
		env->adapter_mem_ops->calloc(1, sizeof(*backend));
	if (!backend) {
		return STREAM_STATUS_OOM;
	}

	backend->stdio_ops = env->stdio_ops;

	switch (args->kind) {
		case STREAM_IO_INPUT:
			backend->stdio = env->stdio_ops->get_stdin();
			break;
		case STREAM_IO_OUTPUT:
			backend->stdio = env->stdio_ops->get_stdout();
			break;
		case STREAM_IO_ERR:
			backend->stdio = env->stdio_ops->get_stderr();
			break;
		default: LEXLEO_ASSERT(0);
	}

	backend->mem_ops = env->adapter_mem_ops;

	*out = backend;
	return STREAM_STATUS_OK;
}

stream_status_t stdio_stream_create_stream(
	stream_t **out,
	const stream_io_creator_args_t *args,
	const stdio_stream_cfg_t *cfg,
	const stdio_stream_env_t *env)
{
	if (
		   !out
		|| !args
		|| !cfg
		|| !env
		|| !env->port_mem_ops
	) {
		return STREAM_STATUS_INVALID;
	}

	stdio_stream_t *backend = NULL;
	stream_status_t st = stdio_stream_create_backend(&backend, args, cfg, env);
	if (st != STREAM_STATUS_OK) {
		return st;
	}

	stream_t *tmp = NULL;
	stream_env_t stream_env =
		stream_default_env(
			&g_stdio_stream_vtbl,
			env->port_mem_ops
		);
	st = stream_create(&tmp, &stream_env);
	if (st != STREAM_STATUS_OK) {
		stream_status_t st2 = stdio_stream_close(backend);
		LEXLEO_ASSERT(st2 == STREAM_STATUS_OK);
		return st;
	}

	st = stream_complete_default_init(tmp, backend);
	LEXLEO_ASSERT(st == STREAM_STATUS_OK);

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

static stream_status_t stdio_stream_ctor(
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
			(const stream_io_creator_args_t *)args,
			&ctor_ud->cfg,
			&ctor_ud->env
		);

	if (st != STREAM_STATUS_OK) {
		return st;
	}

	*out = tmp;
	return STREAM_STATUS_OK;
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

	tmp.ud = (void *)ud;
	osal_memcpy(&ud->cfg, cfg, sizeof(*cfg));
	osal_memcpy(&ud->env, env, sizeof(*env));
	tmp.ud_dtor = stdio_stream_destroy_ud_ctor;

	*out = tmp;
	return STREAM_STATUS_OK;
}

