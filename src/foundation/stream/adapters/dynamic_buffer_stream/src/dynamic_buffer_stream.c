/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/dynamic_buffer_stream/src/
 * dynamic_buffer_stream.c
 *
 * Dynamic buffer stream backend implementation (adapter).
 */

#include "internal/dynamic_buffer_stream_handle.h"
#include "internal/dynamic_buffer_stream_state.h"
#include "internal/dynamic_buffer_stream_ctor_ud.h"
#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_api.h"
#include "policy/lexleo_assert.h"
#include "osal/mem/osal_mem_ops.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static size_t read(void *b, void* buf, size_t n, stream_status_t *st) {
	if (st) *st = STREAM_STATUS_OK;

    if (!b || (!buf && n)) {
        if (st) *st = STREAM_STATUS_INVALID;
        return 0;
    }

    if (n == 0) {
        return 0;
    }

    dynamic_buffer_stream_t *dbs =
        (dynamic_buffer_stream_t *)b;

    LEXLEO_ASSERT(dbs->mem && dbs->mem->memcpy);

    dynamic_buffer_t *dbuf = &dbs->state.dbuf;
    LEXLEO_ASSERT(dbuf->read_pos <= dbuf->len);

    if (dbuf->read_pos >= dbuf->len) {
        if (st) *st = STREAM_STATUS_EOF;
        return 0;
    }

    size_t avail = dbuf->len - dbuf->read_pos;
    size_t ret = (avail < n) ? avail : n;

    dbs->mem->memcpy(buf,
                    dbuf->buf + dbuf->read_pos,
                    ret);

    dbuf->read_pos += ret;

    return ret;
}

static stream_status_t buffer_reserve(
	dynamic_buffer_stream_t *dbs,
	size_t cap)
{
	if (!dbs || !dbs->mem || !dbs->mem->realloc)
		return STREAM_STATUS_INVALID;

	dynamic_buffer_t *dbuf = &dbs->state.dbuf;

	if (dbuf->cap >= cap)
		return STREAM_STATUS_OK;

	void *new_buf = dbs->mem->realloc(dbuf->buf, cap);
	if (!new_buf)
		return STREAM_STATUS_OOM;

	dbuf->cap = cap;
	dbuf->buf = new_buf;

	return STREAM_STATUS_OK;
}

static size_t next_cap(size_t cap) {
	if (cap > SIZE_MAX / 2) return 0;
	return 2 * cap;
}

static size_t write(
    void *b,
    const void *buf,
    size_t n,
    stream_status_t *st)
{
    if (st) *st = STREAM_STATUS_OK;

    if (!b || (!buf && n)) {
        if (st) *st = STREAM_STATUS_INVALID;
        return 0;
    }

    if (n == 0) {
        return 0;
    }

    dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)b;
    dynamic_buffer_t *dbuf = &dbs->state.dbuf;

    LEXLEO_ASSERT(dbs->mem && dbs->mem->memcpy);
    LEXLEO_ASSERT(dbuf->cap > 0);

    if (n > SIZE_MAX - dbuf->len) {
        if (st) *st = STREAM_STATUS_INVALID;
        return 0;
    }

    size_t need = dbuf->len + n;

    size_t new_cap = dbuf->cap;
    if (new_cap == 0) {
        if (st) *st = STREAM_STATUS_INVALID;
        return 0;
    }

    while (need > new_cap) {
        size_t grown = next_cap(new_cap);
        if (grown == 0 || grown <= new_cap) {
            if (st) *st = STREAM_STATUS_OOM;
            return 0;
        }
        new_cap = grown;
    }

	stream_status_t rst = buffer_reserve(dbs, new_cap);
	if (rst != STREAM_STATUS_OK) {
		if (st) *st = rst;
		return 0;
	}

    dbs->mem->memcpy(dbuf->buf + dbuf->len, buf, n);
    dbuf->len += n;

    return n;
}

static stream_status_t flush(void *b)
{
    (void)b;
    return STREAM_STATUS_OK;
}

static stream_status_t close(void *b)
{
    if (!b) return STREAM_STATUS_OK;

    dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)b;
    dynamic_buffer_t *dbuf = &dbs->state.dbuf;

    LEXLEO_ASSERT(dbs->mem && dbs->mem->free);

    if (dbuf->autoclose && dbuf->buf) {
        dbs->mem->free(dbuf->buf);
        dbuf->buf = NULL;
    }

    dbs->mem->free(dbs);
    return STREAM_STATUS_OK;
}

static const stream_vtbl_t VTBL = {
	.read = read,
	.write = write,
	.flush = flush,
	.close = close,
};

static stream_status_t create_backend(
    dynamic_buffer_stream_t **out,
    const dynamic_buffer_stream_cfg_t *cfg,
    const dynamic_buffer_stream_env_t *env)
{
    if (out) *out = NULL;

    if (
    		   !out
    		|| !cfg
    		|| !cfg->default_cap
    		|| !env
    		|| !env->mem
    		|| !env->mem->calloc
    		|| !env->mem->realloc
    		|| !env->mem->free
    		|| !env->mem->memcpy ) {
        return STREAM_STATUS_INVALID;
    }

    dynamic_buffer_stream_t *backend =
        (dynamic_buffer_stream_t *)env->mem->calloc(1, sizeof(*backend));
    if (!backend) {
        return STREAM_STATUS_OOM;
    }

    backend->mem = env->mem;

    backend->state.dbuf.cap = cfg->default_cap;
    backend->state.dbuf.buf = (char *)env->mem->calloc(
        backend->state.dbuf.cap, sizeof(char));

    if (!backend->state.dbuf.buf) {
        env->mem->free(backend);
        return STREAM_STATUS_OOM;
    }

    backend->state.dbuf.len = 0;
    backend->state.dbuf.read_pos = 0;
    backend->state.dbuf.autoclose = true;

    *out = backend;
    return STREAM_STATUS_OK;
}

stream_status_t dynamic_buffer_stream_create_stream(
	stream_t **out,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env )
{
    if (out) *out = NULL;

    if (!out || !env || !cfg) {
        return STREAM_STATUS_INVALID;
    }

    dynamic_buffer_stream_t *backend = NULL;

    stream_status_t st = create_backend(&backend, cfg, env);
    if (st != STREAM_STATUS_OK) {
        return st;
    }

    st = stream_create(out, &VTBL, backend, &env->port_env);
    if (st != STREAM_STATUS_OK) {
        VTBL.close(backend);
        return st;
    }

    return STREAM_STATUS_OK;
}

stream_status_t dynamic_buffer_stream_ctor(
	void *ud,
	const void *args,
	stream_t **out )
{
	if (args) return STREAM_STATUS_INVALID;

	if (out) *out = NULL;

	dynamic_buffer_stream_ctor_ud_t *ctor_ud =
		(dynamic_buffer_stream_ctor_ud_t *) ud;

	if (!out || !ctor_ud) {
		return STREAM_STATUS_INVALID;
	}

	return
		dynamic_buffer_stream_create_stream(
			out,
			&ctor_ud->cfg,
			&ctor_ud->env );
}

void dynamic_buffer_stream_destroy_ud_ctor(
	void *ud,
	const osal_mem_ops_t *mem ) // MUST BE THE FACTORY'S
{
	if (!ud) return;
	LEXLEO_ASSERT(mem && mem->free);
	mem->free(ud);
}

stream_status_t dynamic_buffer_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env,
	const osal_mem_ops_t *mem ) // MUST BE THE FACTORY'S
{
	if (out) *out = (stream_adapter_desc_t){0};

	if (
			   !out
			|| !key
			|| *key == '\0'
			|| !cfg
			|| !cfg->default_cap
			|| !env
			|| !mem
			|| !mem->calloc
			|| !mem->free
			|| !mem->memcpy )
		return STREAM_STATUS_INVALID;

	out->key = key;
	out->ctor = dynamic_buffer_stream_ctor;

	dynamic_buffer_stream_ctor_ud_t *ud = mem->calloc(1, sizeof(*ud));
	if (!ud)
		return STREAM_STATUS_OOM;

	mem->memcpy(&ud->cfg, cfg, sizeof(*cfg));
	mem->memcpy(&ud->env, env, sizeof(*env));
	out->ud = ud;
	out->ud_dtor = dynamic_buffer_stream_destroy_ud_ctor;

	return STREAM_STATUS_OK;
}

dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void) {
	return
		(dynamic_buffer_stream_cfg_t)
			{ .default_cap = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY };
}

dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
	const osal_mem_ops_t *mem,
	const stream_env_t *port_env)
{
	dynamic_buffer_stream_env_t env;
	env.mem = mem ? mem : osal_mem_default_ops();
	LEXLEO_ASSERT(port_env);
	env.port_env = *port_env;
	return env;
}
