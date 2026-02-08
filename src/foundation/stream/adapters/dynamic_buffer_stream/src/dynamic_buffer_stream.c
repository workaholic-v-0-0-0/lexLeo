/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/dynamic_buffer_stream/src/
 * dynamic_buffer_stream.c
 *
 * Dynamic buffer stream backend implementation (adapter).
 */

#include "internal/dynamic_buffer_stream_ctx.h"
#include "internal/dynamic_buffer_stream_internal.h"
#include "dynamic_buffer_stream_factory.h"
#include "lexleo_assert.h"
#include "mem/osal_mem_ops.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static size_t read(void *b, void* buf, size_t n, stream_status_t *st) {
	    if (st) *st = STREAM_STATUS_OK;

    if (!b || !buf) {
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

static bool buffer_reserve(dynamic_buffer_stream_t *dbs, size_t cap) {
	if (!dbs) return false;
	LEXLEO_ASSERT(dbs->mem && dbs->mem->realloc);
	dynamic_buffer_t *dbuf = &dbs->state.dbuf;
	if (dbuf->cap >= cap) return true;
	void *new_buf = dbs->mem->realloc(dbuf->buf, sizeof(char) * cap);
	if (!new_buf) return false;
	dbuf->cap = cap;
	dbuf->buf = new_buf;
	return true;
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

    if (!buffer_reserve(dbs, new_cap)) {
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
    const dynamic_buffer_stream_ctx_t *ctx)
{
    if (out) *out = NULL;

    if (!out || !ctx || !ctx->deps.mem ||
        !ctx->deps.mem->calloc || !ctx->deps.mem->free) {
        return STREAM_STATUS_INVALID;
    }

    dynamic_buffer_stream_t *backend =
        (dynamic_buffer_stream_t *)ctx->deps.mem->calloc(1, sizeof(*backend));
    if (!backend) {
        return STREAM_STATUS_OOM;
    }

    backend->mem = ctx->deps.mem;

    backend->state.dbuf.cap = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY;
    backend->state.dbuf.buf = (char *)ctx->deps.mem->calloc(
        backend->state.dbuf.cap, sizeof(char));

    if (!backend->state.dbuf.buf) {
        ctx->deps.mem->free(backend);
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
    const dynamic_buffer_stream_ctx_t *ctx)
{
    if (out) *out = NULL;

    if (!out || !ctx) {
        return STREAM_STATUS_INVALID;
    }

    dynamic_buffer_stream_t *backend = NULL;

    stream_status_t st = create_backend(&backend, ctx);
    if (st != STREAM_STATUS_OK) {
        return st;
    }

    st = stream_create(out, &VTBL, backend, ctx->port_ctx);
    if (st != STREAM_STATUS_OK) {
        VTBL.close(backend);
        return st;
    }

    return STREAM_STATUS_OK;
}

dynamic_buffer_stream_ctx_t dynamic_buffer_stream_default_ctx(
	const osal_mem_ops_t *mem_ops,
	const stream_ctx_t *port_ctx )
{
	dynamic_buffer_stream_ctx_t ctx;
	ctx.deps.mem = mem_ops ? mem_ops : osal_mem_default_ops();
	ctx.port_ctx = port_ctx;
	return ctx;
}

typedef struct {
	const osal_mem_ops_t *mem;
	const stream_vtbl_t *vtbl;
} dynamic_buffer_stream_factory_userdata_t;

static stream_status_t factory_create_adapter(void *userdata, stream_t **out) {
	const dynamic_buffer_stream_factory_userdata_t *ud =
		(const dynamic_buffer_stream_factory_userdata_t *) userdata;

	if (out) *out = NULL;
	if (!ud || !ud->vtbl || !ud->mem || !out) return STREAM_STATUS_INVALID;

    dynamic_buffer_stream_ctx_t tmp = {
        .deps = { .mem = ud->mem },
		.port_ctx = NULL // is depreciated anyway
    };

    return dynamic_buffer_stream_create_stream(out, &tmp);
}

stream_status_t dynamic_buffer_stream_create_factory(
		dynamic_buffer_stream_factory_t **out,
		const dynamic_buffer_stream_ctx_t *ctx ) {
	if (out) *out = NULL;
	if (
			   !out
			|| !ctx
			|| !ctx->deps.mem
			|| !ctx->deps.mem->calloc
			|| !ctx->deps.mem->free )
		return STREAM_STATUS_INVALID;

	const osal_mem_ops_t *mem = ctx->deps.mem;

	dynamic_buffer_stream_factory_t *fact = mem->calloc(1, sizeof(*fact));

	if (!fact) return STREAM_STATUS_INVALID;

	dynamic_buffer_stream_factory_userdata_t *ud = mem->calloc(1, sizeof(*ud));

	if (!ud) {
		mem->free(fact);
		return STREAM_STATUS_INVALID;
	}

	ud->mem  = mem;
	ud->vtbl = NULL;

	fact->userdata = ud;
	fact->create   = factory_create_adapter;
	fact->mem = mem;

	*out = fact;

	return STREAM_STATUS_OK;
}

stream_status_t dynamic_buffer_stream_destroy_factory(
		dynamic_buffer_stream_factory_t **fact ) {
	if (!fact || !*fact) return STREAM_STATUS_OK;

	const osal_mem_ops_t *mem = (*fact)->mem;

    if (!mem || !mem->free) {
		// log
		return STREAM_STATUS_INVALID;
	}

	if ((*fact)->userdata) {
		mem->free((*fact)->userdata);
	}
	mem->free(*fact);
	*fact = NULL;

	return STREAM_STATUS_OK;
}
