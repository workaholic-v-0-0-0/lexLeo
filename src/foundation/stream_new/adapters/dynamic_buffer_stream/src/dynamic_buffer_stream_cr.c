/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_cr.c
 * @ingroup dynamic_buffer_stream_internal_group
 * @brief Composition Root support implementation for the
 * `dynamic_buffer_stream` adapter.
 *
 * @details
 * Implements the CR-facing construction and registration services for the
 * `dynamic_buffer_stream` adapter, including backend creation, default
 * initialization, and adapter provider construction.
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
	if (!backend || (!buf && n)) {
		if (st) {
			*st = STREAM_STATUS_INVALID;
		}
		return (size_t)0;
	}

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)backend;

	LEXLEO_ASSERT(
		   dbs->mem_ops
		&& dbs->state.dbuf.buf
		&& dbs->state.dbuf.cap > 0
		&& dbs->state.dbuf.read_pos <= dbs->state.dbuf.len
		&& dbs->state.dbuf.len <= dbs->state.dbuf.cap
	);

	if (st) {
		*st = STREAM_STATUS_OK;
	}

	if (n == 0) {
		return (size_t)0;
	}

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
	if (!dbs || !dbs->mem_ops || !dbs->mem_ops->realloc) {
		return STREAM_STATUS_INVALID;
	}

	dynamic_buffer_t *dbuf = &dbs->state.dbuf;

	if (dbuf->cap >= cap) {
		return STREAM_STATUS_OK;
	}

	void *new_buf = dbs->mem_ops->realloc(dbuf->buf, cap);
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
	if (!backend || (!buf && n)) {
		if (st) {
			*st = STREAM_STATUS_INVALID;
		}
		return (size_t)0;
	}

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)backend;

	LEXLEO_ASSERT(
		   dbs->mem_ops
		&& dbs->state.dbuf.buf
		&& dbs->state.dbuf.cap > 0
		&& dbs->state.dbuf.len <= dbs->state.dbuf.cap
		&& dbs->state.dbuf.read_pos <= dbs->state.dbuf.len
	);

	if (st) {
		*st = STREAM_STATUS_OK;
	}

	if (n == 0) {
		return (size_t)0;
	}

	dynamic_buffer_t *dbuf = &dbs->state.dbuf;

	LEXLEO_ASSERT(dbs->mem_ops && dbuf->cap > 0);

	if (n > SIZE_MAX - dbuf->len) {
		if (st) {
			*st = STREAM_STATUS_INVALID;
		}
		return (size_t)0;
	}

	size_t need = dbuf->len + n;
	size_t new_cap = dbuf->cap;

	while (need > new_cap) {
		size_t grown = dynamic_buffer_stream_next_cap(new_cap);

		if (grown == 0) {
			new_cap = need;
			break;
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

	LEXLEO_ASSERT(dbs->mem_ops && dbs->mem_ops->free);

	if (dbuf->buf) {
		dbs->mem_ops->free(dbuf->buf);
		dbuf->buf = NULL;
	}

	dbs->mem_ops->free(dbs);
	return STREAM_STATUS_OK;
}

static const stream_vtbl_t g_dynamic_buffer_stream_vtbl = {
	.read = dynamic_buffer_stream_read,
	.write = dynamic_buffer_stream_write,
	.flush = dynamic_buffer_stream_flush,
	.close = dynamic_buffer_stream_close
};

const stream_vtbl_t *dynamic_buffer_stream_vtbl(void)
{
	return &g_dynamic_buffer_stream_vtbl;
}

dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void)
{
	return (dynamic_buffer_stream_cfg_t) {
		.default_cap = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY
	};
}

dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(mem_ops);

	return (dynamic_buffer_stream_env_t){ .mem_ops = mem_ops };
}

dynamic_buffer_stream_status_t dynamic_buffer_stream_create(
	dynamic_buffer_stream_t **out,
	const dynamic_buffer_stream_env_t *env
) {
	LEXLEO_ASSERT(
		   out
		&& env
		&& env->mem_ops
		&& env->mem_ops->calloc
	);

	dynamic_buffer_stream_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return DYNAMIC_BUFFER_STREAM_STATUS_OOM;
	}

	tmp->mem_ops = env->mem_ops;

	*out = tmp;
	return DYNAMIC_BUFFER_STREAM_STATUS_OK;
}

dynamic_buffer_stream_status_t dynamic_buffer_stream_complete_default_init(
	dynamic_buffer_stream_t *dynamic_buffer_stream,
	const dynamic_buffer_stream_cfg_t *cfg
) {
	LEXLEO_ASSERT(
		   dynamic_buffer_stream
		&& dynamic_buffer_stream->mem_ops
		&& dynamic_buffer_stream->mem_ops->calloc
		&& cfg
		&& cfg->default_cap > 0
	);

	if (dynamic_buffer_stream->state.dbuf.buf) {
		return DYNAMIC_BUFFER_STREAM_STATUS_OK;
	}

	dynamic_buffer_stream->state.dbuf.buf =
		dynamic_buffer_stream->mem_ops->calloc(
			1,
			cfg->default_cap * sizeof(char)
		);
	if (!dynamic_buffer_stream->state.dbuf.buf) {
		return DYNAMIC_BUFFER_STREAM_STATUS_OOM;
	}

	dynamic_buffer_stream->state.dbuf.cap = cfg->default_cap;
	dynamic_buffer_stream->state.dbuf.len = 0;
	dynamic_buffer_stream->state.dbuf.read_pos = 0;

	return DYNAMIC_BUFFER_STREAM_STATUS_OK;
}

static stream_status_t dynamic_buffer_stream_status_to_stream_status(
	dynamic_buffer_stream_status_t status
) {
	switch (status) {
		case DYNAMIC_BUFFER_STREAM_STATUS_OK: return STREAM_STATUS_OK;
		case DYNAMIC_BUFFER_STREAM_STATUS_OOM: return STREAM_STATUS_OOM;
		default: return STREAM_STATUS_IO_ERROR;
	}
}

static stream_status_t dynamic_buffer_stream_ctor(
	const void *ud,
	const void *args,
	void **out
) {
	(void)args;

	dynamic_buffer_stream_ctor_ud_t *dynamic_buffer_stream_ctor_ud =
		(dynamic_buffer_stream_ctor_ud_t *)ud;

	LEXLEO_ASSERT(dynamic_buffer_stream_ctor_ud && out);

	dynamic_buffer_stream_t *dynamic_buffer_stream = NULL;
	dynamic_buffer_stream_status_t dynamic_buffer_stream_status =
		dynamic_buffer_stream_create(
			&dynamic_buffer_stream,
			&dynamic_buffer_stream_ctor_ud->env
		);
	if (dynamic_buffer_stream_status != DYNAMIC_BUFFER_STREAM_STATUS_OK) {
		stream_status_t stream_status =
			dynamic_buffer_stream_status_to_stream_status(
				dynamic_buffer_stream_status
			);
		return stream_status;
	}

	dynamic_buffer_stream_status =
		dynamic_buffer_stream_complete_default_init(
			dynamic_buffer_stream,
			&dynamic_buffer_stream_ctor_ud->cfg
		);
	if (dynamic_buffer_stream_status != DYNAMIC_BUFFER_STREAM_STATUS_OK) {
		dynamic_buffer_stream_vtbl()->close(dynamic_buffer_stream);
		stream_status_t stream_status =
			dynamic_buffer_stream_status_to_stream_status(
				dynamic_buffer_stream_status
			);
		return stream_status;
	}

	*out = dynamic_buffer_stream;
	return STREAM_STATUS_OK;
}

static void dynamic_buffer_stream_ctor_ud_dtor(
	void *ud,
	const osal_mem_ops_t *mem
) {
	LEXLEO_ASSERT(mem && mem->free);
	mem->free(ud);
}

dynamic_buffer_stream_status_t dynamic_buffer_stream_create_adapter_provider(
	stream_adapter_provider_t **out,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env
) {
	LEXLEO_ASSERT(
		   out
		&& cfg
		&& cfg->default_cap > 0
		&& env
		&& env->mem_ops
		&& env->mem_ops->calloc
		&& env->mem_ops->free
	);

	dynamic_buffer_stream_ctor_ud_t *dynamic_buffer_stream_ctor_ud =
		env->mem_ops->calloc(1, sizeof(*dynamic_buffer_stream_ctor_ud));
	if (!dynamic_buffer_stream_ctor_ud) {
		return DYNAMIC_BUFFER_STREAM_STATUS_OOM;
	}

	dynamic_buffer_stream_ctor_ud->cfg = *cfg;
	dynamic_buffer_stream_ctor_ud->env = *env;

	stream_adapter_provider_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		dynamic_buffer_stream_ctor_ud_dtor(
			dynamic_buffer_stream_ctor_ud,
			env->mem_ops
		);
		return DYNAMIC_BUFFER_STREAM_STATUS_OOM;
	}

	tmp->backend_ctor = dynamic_buffer_stream_ctor;
	tmp->vtbl = &g_dynamic_buffer_stream_vtbl;
	tmp->ud = dynamic_buffer_stream_ctor_ud;
	tmp->ud_dtor = dynamic_buffer_stream_ctor_ud_dtor;
	tmp->mem = env->mem_ops;

	*out = tmp;
	return DYNAMIC_BUFFER_STREAM_STATUS_OK;
}
