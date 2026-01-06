/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/adapters/stream/dynamic_buffer_stream/src/
 * dynamic_buffer_stream.c
 *
 * Dynamic buffer stream backend implementation (adapter).
 */

#include "internal/dynamic_buffer_stream_ctx.h"
#include "internal/dynamic_buffer_stream_internal.h"
#include "internal/stream_ctx.h"
#include "dynamic_buffer_stream_factory.h"
#include "lexleo_assert.h"
#include "osal_mem_ops.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static size_t read(void *b, void* buf, size_t n) {
	if (!b || n==0 || !buf) return 0;

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t*)b;
	LEXLEO_ASSERT(dbs->mem && dbs->mem->memcpy);
	dynamic_buffer_t *dbuf = &dbs->state.dbuf;
	LEXLEO_ASSERT(dbuf->read_pos <= dbuf->len);

	if (dbuf->len <= dbuf->read_pos) return 0;

	size_t ret = (dbuf->len - dbuf->read_pos < n) ? dbuf->len - dbuf->read_pos : n;
	dbs->mem->memcpy(buf, dbuf->buf + dbuf->read_pos, ret);
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

static size_t write(void *b, const void* buf, size_t n) {
	if (!b || (!buf && n)) return 0;

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t*)b;
	dynamic_buffer_t *dbuf = &dbs->state.dbuf;

	LEXLEO_ASSERT(dbs->mem && dbs->mem->memcpy);
	LEXLEO_ASSERT(dbuf->cap > 0);

	if (n > SIZE_MAX - dbuf->len) return 0;
	size_t need = dbuf->len + n;

	size_t new_cap = dbuf->cap;
	if (new_cap == 0) return 0;

	while (need > new_cap) {
		size_t grown = next_cap(new_cap);
		if (grown == 0 || grown <= new_cap) return 0; // overflow/stuck guard
		new_cap = grown;
	}

	if (!buffer_reserve(dbs, new_cap)) return 0;

	dbs->mem->memcpy(dbuf->buf + dbuf->len, buf, n);
	dbuf->len += n;

	return n;
}

static int flush(void *b) {
	(void)b;
	return 0;
}

static int close(void *b) {
	if (!b) return 0;

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t*)b;
	dynamic_buffer_t *dbuf = &dbs->state.dbuf;

	LEXLEO_ASSERT(dbs->mem && dbs->mem->free);

	if (dbuf->autoclose) {
		dbs->mem->free(dbuf->buf);
		dbuf->buf = NULL;
	}

	dbs->mem->free(b);

	return 0;
}

static const stream_vtbl_t DEFAULT_VTBL = {
	.read = read,
	.write = write,
	.flush = flush,
	.close = close,
};

static stream_status_t create_backend (
		dynamic_buffer_stream_t **out,
		const dynamic_buffer_stream_ctx_t *ctx ) {
	if (out) *out = NULL;
	if (
			   !out
			|| !ctx
			|| !ctx->deps.mem
			|| !ctx->deps.mem->calloc
			|| !ctx->deps.mem->free )
		return STREAM_STATUS_ERROR;

	dynamic_buffer_stream_t *backend = ctx->deps.mem->calloc(1, sizeof(*backend));
	if (!backend)
		return STREAM_STATUS_ERROR;
	backend->mem = ctx->deps.mem;
	backend->state.dbuf.cap = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY;
	backend->state.dbuf.buf =
		ctx->deps.mem->calloc(backend->state.dbuf.cap, sizeof(char));
	if (!backend->state.dbuf.buf) {
		ctx->deps.mem->free(backend);
		return STREAM_STATUS_ERROR;
	}
	backend->state.dbuf.len = 0;
	backend->state.dbuf.read_pos = 0;
	backend->state.dbuf.autoclose = true;

	*out = backend;
	return STREAM_STATUS_OK;
}

stream_status_t dynamic_buffer_stream_create_stream(
		stream_t **out,
		const dynamic_buffer_stream_ctx_t *ctx ) {
	if (out) *out = NULL;
	if (!out || !ctx) return STREAM_STATUS_ERROR;

	dynamic_buffer_stream_t *backend = NULL;

	if (create_backend(&backend, ctx) != STREAM_STATUS_OK)
		return STREAM_STATUS_ERROR;

	stream_ctx_t stream_ctx = { .mem = ctx->deps.mem };

	if (stream_create(out, ctx->stream_vtbl, backend, &stream_ctx) != STREAM_STATUS_OK) {
		if (ctx->stream_vtbl->close) ctx->stream_vtbl->close(backend); // rollback
		return STREAM_STATUS_ERROR;
	}
	return STREAM_STATUS_OK;
}

dynamic_buffer_stream_ctx_t dynamic_buffer_stream_default_ctx(
		const osal_mem_ops_t *mem_ops ) {
	dynamic_buffer_stream_ctx_t ctx;
	ctx.deps.mem = mem_ops ? mem_ops : osal_mem_default_ops();
	ctx.stream_vtbl = &DEFAULT_VTBL;
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
	if (!ud || !ud->vtbl || !ud->mem || !out) return STREAM_STATUS_ERROR;

    dynamic_buffer_stream_ctx_t tmp = {
        .deps = { .mem = ud->mem },
        .stream_vtbl = ud->vtbl,
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
			|| !ctx->deps.mem->free
			|| !ctx->stream_vtbl )
		return STREAM_STATUS_ERROR;

	const osal_mem_ops_t *mem = ctx->deps.mem;

	dynamic_buffer_stream_factory_t *fact = mem->calloc(1, sizeof(*fact));

	if (!fact) return STREAM_STATUS_ERROR;

	dynamic_buffer_stream_factory_userdata_t *ud = mem->calloc(1, sizeof(*ud));

	if (!ud) {
		mem->free(fact);
		return STREAM_STATUS_ERROR;
	}

	ud->mem  = mem;
	ud->vtbl = ctx->stream_vtbl;

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
		return STREAM_STATUS_ERROR;
	}

	if ((*fact)->userdata) {
		mem->free((*fact)->userdata);
	}
	mem->free(*fact);
	*fact = NULL;

	return STREAM_STATUS_OK;
}
