/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream.c
 * @ingroup dynamic_buffer_stream_internal_group
 * @brief `dynamic_buffer_stream` adapter implementation.
 *
 * @details
 * In-memory dynamic-buffer-backed stream adapter.
 */

#include "internal/dynamic_buffer_stream_state.h"
#include "internal/dynamic_buffer_stream_handle.h"
#include "internal/dynamic_buffer_stream_ctor_ud.h"

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_api.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/mem/osal_mem.h"

#include "policy/lexleo_assert.h"

static size_t dynamic_buffer_stream_read(
	void *backend,
	void *buf,
	size_t n,
	stream_status_t *st)
{
	if (st) {
		*st = STREAM_STATUS_OK;
	}

	if (!backend || (!buf && n)) {
		if (st) {
			*st = STREAM_STATUS_INVALID;
		}
		return (size_t)0;
	}

	if (n == 0) {
		return (size_t)0;
	}

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)backend;

	LEXLEO_ASSERT(dbs->mem);

	dynamic_buffer_t *dbuf = &dbs->state.dbuf;
	LEXLEO_ASSERT(dbuf->read_pos <= dbuf->len);

	if (dbuf->read_pos >= dbuf->len) {
		if (st) {
			*st = STREAM_STATUS_EOF;
		}
		return (size_t)0;
	}

	size_t avail = dbuf->len - dbuf->read_pos;
	size_t ret = (avail < n) ? avail : n;

	osal_memcpy(buf, dbuf->buf + dbuf->read_pos, ret);
	dbuf->read_pos += ret;

	return ret;
}

static stream_status_t dynamic_buffer_stream_buffer_reserve(
	dynamic_buffer_stream_t *dbs,
	size_t cap)
{
	if (!dbs || !dbs->mem || !dbs->mem->realloc) {
		return STREAM_STATUS_INVALID;
	}

	dynamic_buffer_t *dbuf = &dbs->state.dbuf;

	if (dbuf->cap >= cap) {
		return STREAM_STATUS_OK;
	}

	void *new_buf = dbs->mem->realloc(dbuf->buf, cap);
	if (!new_buf) {
		return STREAM_STATUS_OOM;
	}

	dbuf->cap = cap;
	dbuf->buf = new_buf;

	return STREAM_STATUS_OK;
}

static size_t dynamic_buffer_stream_next_cap(size_t cap)
{
	if (cap > SIZE_MAX / 2) {
		return (size_t)0;
	}
	return (size_t)(2 * cap);
}

static size_t dynamic_buffer_stream_write(
	void *backend,
	const void *buf,
	size_t n,
	stream_status_t *st)
{
	if (st) {
		*st = STREAM_STATUS_OK;
	}

	if (!backend || (!buf && n)) {
		if (st) {
			*st = STREAM_STATUS_INVALID;
		}
		return (size_t)0;
	}

	if (n == 0) {
		return (size_t)0;
	}

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)backend;
	dynamic_buffer_t *dbuf = &dbs->state.dbuf;

	LEXLEO_ASSERT(dbs->mem && dbuf->cap > 0);

	if (n > SIZE_MAX - dbuf->len) {
		if (st) {
			*st = STREAM_STATUS_INVALID;
		}
		return (size_t)0;
	}

	size_t need = dbuf->len + n;
	size_t new_cap = dbuf->cap;

	if (new_cap == 0) {
		if (st) {
			*st = STREAM_STATUS_INVALID;
		}
		return (size_t)0;
	}

	while (need > new_cap) {
		size_t grown = dynamic_buffer_stream_next_cap(new_cap);
		if (grown == 0 || grown <= new_cap) {
			if (st) {
				*st = STREAM_STATUS_OOM;
			}
			return (size_t)0;
		}
		new_cap = grown;
	}

	stream_status_t rst =
		dynamic_buffer_stream_buffer_reserve(dbs, new_cap);
	if (rst != STREAM_STATUS_OK) {
		if (st) {
			*st = rst;
		}
		return (size_t)0;
	}

	osal_memcpy(dbuf->buf + dbuf->len, buf, n);
	dbuf->len += n;

	return n;
}

static stream_status_t dynamic_buffer_stream_flush(void *backend)
{
	(void)backend;
	return STREAM_STATUS_OK;
}

static stream_status_t dynamic_buffer_stream_close(void *backend)
{
	if (!backend) {
		return STREAM_STATUS_OK;
	}

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)backend;
	dynamic_buffer_t *dbuf = &dbs->state.dbuf;

	LEXLEO_ASSERT(dbs->mem && dbs->mem->free);

	if (dbuf->autoclose && dbuf->buf) {
		dbs->mem->free(dbuf->buf);
		dbuf->buf = NULL;
	}

	dbs->mem->free(dbs);
	return STREAM_STATUS_OK;
}

static const stream_vtbl_t g_dynamic_buffer_stream_vtbl = {
	.read = dynamic_buffer_stream_read,
	.write = dynamic_buffer_stream_write,
	.flush = dynamic_buffer_stream_flush,
	.close = dynamic_buffer_stream_close
};

static stream_status_t dynamic_buffer_stream_create_backend(
	dynamic_buffer_stream_t **out,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env
) {
	if (
		   !out
		|| !cfg
		|| !cfg->default_cap
		|| !env
		|| !env->adapter_mem_ops
		|| !env->adapter_mem_ops->calloc
		|| !env->adapter_mem_ops->realloc
		|| !env->adapter_mem_ops->free
	) {
		return STREAM_STATUS_INVALID;
	}

	dynamic_buffer_stream_t *backend =
		(dynamic_buffer_stream_t *)
		env->adapter_mem_ops->calloc(1, sizeof(*backend));
	if (!backend) {
		return STREAM_STATUS_OOM;
	}

	backend->mem = env->adapter_mem_ops;
	backend->state.dbuf.cap = cfg->default_cap;
	backend->state.dbuf.buf =
		(char *)
		env->adapter_mem_ops->calloc(backend->state.dbuf.cap, sizeof(char));

	if (!backend->state.dbuf.buf) {
		env->adapter_mem_ops->free(backend);
		return STREAM_STATUS_OOM;
	}

	backend->state.dbuf.len = 0;
	backend->state.dbuf.read_pos = 0;
	backend->state.dbuf.autoclose = true;

	*out = backend;
	return STREAM_STATUS_OK;
}

static void dynamic_buffer_stream_destroy_ud_ctor(
	const void *ud,
	const osal_mem_ops_t *mem)
{
	if (!ud) {
		return;
	}

	LEXLEO_ASSERT(mem && mem->free);
	mem->free((void *)ud);
}

stream_status_t dynamic_buffer_stream_create_stream(
	stream_t **out,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env)
{
	if (!out || !env || !cfg) {
		return STREAM_STATUS_INVALID;
	}

	dynamic_buffer_stream_t *backend = NULL;
	stream_status_t st =
		dynamic_buffer_stream_create_backend(&backend, cfg, env);
	if (st != STREAM_STATUS_OK) {
		return st;
	}

	stream_t *tmp = NULL;
	stream_env_t stream_env =
		stream_default_env(
			&g_dynamic_buffer_stream_vtbl,
			env->port_mem_ops
		);
	st = stream_create(&tmp, &stream_env);
	if (st != STREAM_STATUS_OK) {
		stream_status_t st2 = dynamic_buffer_stream_close(backend);
		LEXLEO_ASSERT(st2 == STREAM_STATUS_OK);
		return st;
	}

	st = stream_complete_default_init(tmp, backend);
	LEXLEO_ASSERT(st == STREAM_STATUS_OK);

	*out = tmp;
	return STREAM_STATUS_OK;
}

static stream_status_t dynamic_buffer_stream_ctor(
	const void *ud,
	const void *args,
	stream_t **out
) {
	const dynamic_buffer_stream_ctor_ud_t *ctor_ud =
		(const dynamic_buffer_stream_ctor_ud_t *)ud;

	if (args || !out || !ctor_ud) {
		return STREAM_STATUS_INVALID;
	}

	stream_t *tmp = NULL;
	stream_status_t st = dynamic_buffer_stream_create_stream(
		&tmp,
		&ctor_ud->cfg,
		&ctor_ud->env);

	if (st != STREAM_STATUS_OK) {
		return st;
	}

	*out = tmp;
	return STREAM_STATUS_OK;
}

stream_status_t dynamic_buffer_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env,
	const osal_mem_ops_t *mem)
{
	if (out) {
		*out = (stream_adapter_desc_t){0};
	}

	stream_adapter_desc_t tmp = {0};

	if (!out
		|| !key
		|| *key == '\0'
		|| !cfg
		|| !cfg->default_cap
		|| !env
		|| !mem
		|| !mem->calloc
		|| !mem->free) {
		return STREAM_STATUS_INVALID;
	}

	tmp.key = key;
	tmp.ctor = dynamic_buffer_stream_ctor;

	dynamic_buffer_stream_ctor_ud_t *ud =
		(dynamic_buffer_stream_ctor_ud_t *)mem->calloc(1, sizeof(*ud));
	if (!ud) {
		return STREAM_STATUS_OOM;
	}

	tmp.ud = ud;
	osal_memcpy(&ud->cfg, cfg, sizeof(*cfg));
	osal_memcpy(&ud->env, env, sizeof(*env));
	tmp.ud_dtor = dynamic_buffer_stream_destroy_ud_ctor;

	*out = tmp;
	return STREAM_STATUS_OK;
}

dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void)
{
	return (dynamic_buffer_stream_cfg_t){
		.default_cap = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY
	};
}

dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
	const osal_mem_ops_t *adapter_mem_ops,
	const osal_mem_ops_t *port_mem_ops
) {
	LEXLEO_ASSERT(adapter_mem_ops && port_mem_ops);

	return (dynamic_buffer_stream_env_t){
		.adapter_mem_ops = adapter_mem_ops,
		.port_mem_ops = port_mem_ops
	};
}
