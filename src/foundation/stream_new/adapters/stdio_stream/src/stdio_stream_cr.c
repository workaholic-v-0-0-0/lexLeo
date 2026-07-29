/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_cr.c
 * @ingroup stdio_stream_internal_group
 * @brief Composition Root support implementation for the `stdio_stream`
 * adapter.
 *
 * @details
 * Implements the CR-facing construction and registration services for the
 * `stdio_stream` adapter, including backend creation from external borrowed
 * dependencies, completion of backend initialization with internal borrowed
 * dependencies, and adapter provider construction.
 */

#include "internal/stdio_stream_handle.h"
#include "internal/stdio_stream_ctor_ud.h"

#include "stdio_stream/cr/stdio_stream_cr_api.h"

#include "osal/mem/osal_mem.h"

#include "policy/lexleo_assert.h"

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

const stream_vtbl_t *stdio_stream_vtbl(void)
{
	return &g_stdio_stream_vtbl;
}

stdio_stream_cfg_t stdio_stream_default_cfg(void)
{
	return (stdio_stream_cfg_t) { .reserved = 0 };
}

stdio_stream_env_t stdio_stream_default_env(
	const osal_stdio_ops_t *stdio_ops,
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(stdio_ops && mem_ops);

	return (stdio_stream_env_t){
		.stdio_ops = stdio_ops,
		.mem_ops = mem_ops
	};
}

stdio_stream_status_t stdio_stream_create(
	stdio_stream_t **out,
	const stdio_stream_env_t *env
) {
	LEXLEO_ASSERT(env && env->mem_ops && env->mem_ops->calloc);

	stdio_stream_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return STDIO_STREAM_STATUS_OOM;
	}

	tmp->stdio_ops = env->stdio_ops;
	tmp->mem_ops = env->mem_ops;

	*out = tmp;
	return STDIO_STREAM_STATUS_OK;
}

stdio_stream_status_t stdio_stream_complete_default_init(
	stdio_stream_t *stdio_stream,
	const stdio_stream_cfg_t *cfg,
	const stream_standard_stream_creator_args_t *args
) {
	(void)cfg;

	LEXLEO_ASSERT(
		   stdio_stream
		&& stdio_stream->stdio_ops
		&& stdio_stream->stdio_ops
		&& stdio_stream->stdio_ops->get_stdin
		&& stdio_stream->stdio_ops->get_stdout
		&& stdio_stream->stdio_ops->get_stderr
		&& (
			   args->kind == STREAM_STANDARD_STREAM_KIND_STDIN
			|| args->kind == STREAM_STANDARD_STREAM_KIND_STDOUT
			|| args->kind == STREAM_STANDARD_STREAM_KIND_STDERR
		)
	);

	if (stdio_stream->stdio) {
		return STDIO_STREAM_STATUS_OK;
	}

	switch (args->kind) {
		case STREAM_STANDARD_STREAM_KIND_STDIN:
			stdio_stream->stdio = stdio_stream->stdio_ops->get_stdin();
			break;
		case STREAM_STANDARD_STREAM_KIND_STDOUT:
			stdio_stream->stdio = stdio_stream->stdio_ops->get_stdout();
			break;
		case STREAM_STANDARD_STREAM_KIND_STDERR:
			stdio_stream->stdio = stdio_stream->stdio_ops->get_stderr();
			break;
		default:
			LEXLEO_ASSERT(false);
	}

	return STDIO_STREAM_STATUS_OK;
}

static stream_status_t stdio_stream_status_to_stream_status(
	stdio_stream_status_t status
) {
	switch (status) {
		case STDIO_STREAM_STATUS_OK: return STREAM_STATUS_OK;
		case STDIO_STREAM_STATUS_OOM: return STREAM_STATUS_OOM;
		default: return STREAM_STATUS_IO_ERROR;
	}
}

static stream_status_t stdio_stream_ctor(
	const void *ud,
	const void *args,
	void **out
) {
	const stdio_stream_ctor_ud_t *stdio_stream_ctor_ud =
		(const stdio_stream_ctor_ud_t *)ud;
	const stream_standard_stream_creator_args_t *stream_standard_stream_creator_args =
		(const stream_standard_stream_creator_args_t *)args;

	LEXLEO_ASSERT(
		   stdio_stream_ctor_ud
		&& stream_standard_stream_creator_args
		&& out
	);

	stdio_stream_t *tmp = NULL;

	stdio_stream_status_t stdio_stream_st =
		stdio_stream_create(
			&tmp,
			&stdio_stream_ctor_ud->env
		);
	if (stdio_stream_st != STDIO_STREAM_STATUS_OK) {
		return stdio_stream_status_to_stream_status(stdio_stream_st);
	}

	stdio_stream_st =
		stdio_stream_complete_default_init(
			tmp,
			&stdio_stream_ctor_ud->cfg,
			stream_standard_stream_creator_args
		);
	if (stdio_stream_st != STDIO_STREAM_STATUS_OK) {
		stdio_stream_vtbl()->close(tmp);
		return stdio_stream_status_to_stream_status(stdio_stream_st);
	}

	*out = tmp;
	return STREAM_STATUS_OK;
}

static void stdio_stream_ctor_ud_dtor(
	void *ud,
	const osal_mem_ops_t *mem
) {
	stdio_stream_ctor_ud_t *stdio_stream_ctor_ud =
		(stdio_stream_ctor_ud_t *)ud;

	LEXLEO_ASSERT(stdio_stream_ctor_ud && mem && mem->free);

	mem->free(stdio_stream_ctor_ud);
}

stdio_stream_status_t stdio_stream_create_adapter_provider(
	stream_adapter_provider_t **out,
	const stdio_stream_cfg_t *cfg,
	const stdio_stream_env_t *env
) {
	LEXLEO_ASSERT(
		   out
		&& cfg
		&& env
		&& env->mem_ops
		&& env->mem_ops->calloc
		&& env->mem_ops->free
	);

	stdio_stream_ctor_ud_t *stdio_stream_ctor_ud =
		env->mem_ops->calloc(1, sizeof(*stdio_stream_ctor_ud));
	if (!stdio_stream_ctor_ud) {
		return STDIO_STREAM_STATUS_OOM;
	}

	stdio_stream_ctor_ud->cfg = *cfg;
	stdio_stream_ctor_ud->env = *env;

	stream_adapter_provider_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		stdio_stream_ctor_ud_dtor(stdio_stream_ctor_ud, env->mem_ops);
		return STDIO_STREAM_STATUS_OOM;
	}

	tmp->backend_ctor = stdio_stream_ctor;
	tmp->vtbl = stdio_stream_vtbl();
	tmp->ud = stdio_stream_ctor_ud;
	tmp->ud_dtor = stdio_stream_ctor_ud_dtor;
	tmp->mem = env->mem_ops;

	*out = tmp;
	return STDIO_STREAM_STATUS_OK;
}
