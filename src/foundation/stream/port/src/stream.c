// src/foundation/stream/port/src/stream.c

#include "stream/borrowers/stream.h"
#include "stream/adapters/stream_adapters_api.h"
#include "stream/cr/stream_cr_api.h"
#include "internal/stream_handle.h"
#include "lexleo_assert.h"

size_t stream_read(stream_t *s, void *buf, size_t n, stream_status_t *st) {
    if (st) *st = STREAM_STATUS_OK;

    if (n == 0) return (size_t)0;

    if (!s || (n && !buf)) {
        if (st) *st = STREAM_STATUS_INVALID;
        return (size_t)0;
    }

    if (!s->vtbl.read) {
        if (st) *st = STREAM_STATUS_INVALID;
        return (size_t)0;
    }

    if (!s->backend) {
        if (st) *st = STREAM_STATUS_NO_BACKEND;
        return (size_t)0;
    }

    return s->vtbl.read(s->backend, buf, n, st);
}

size_t stream_write(
	stream_t *s,
	const void *buf,
	size_t n,
	stream_status_t *st )
{
    if (st) *st = STREAM_STATUS_OK;

    if (n == 0) return (size_t)0;

    if (!s || (n && !buf)) {
        if (st) *st = STREAM_STATUS_INVALID;
        return (size_t)0;
    }

    if (!s->vtbl.write) {
        if (st) *st = STREAM_STATUS_INVALID;
        return (size_t)0;
    }

    if (!s->backend) {
        if (st) *st = STREAM_STATUS_NO_BACKEND;
        return (size_t)0;
    }

    return s->vtbl.write(s->backend, buf, n, st);
}

stream_status_t stream_flush(stream_t *s) {
    if (!s || !s->vtbl.flush) return STREAM_STATUS_INVALID;
    if (!s->backend) return STREAM_STATUS_NO_BACKEND;

    return s->vtbl.flush(s->backend);
}

stream_status_t stream_create(
		stream_t **out,
		const stream_vtbl_t *vtbl,
		void *backend,
		const stream_ctx_t *ctx ) {
	if (out) *out = NULL;
	if (
			   !out
			|| !vtbl
			|| !vtbl->read
			|| !vtbl->write
			|| !ctx
			|| !ctx->mem
			|| !ctx->mem->calloc
			|| !ctx->mem->free )
		return STREAM_STATUS_INVALID;

	stream_t *s = ctx->mem->calloc(1, sizeof(*s));
	if (!s) return STREAM_STATUS_OOM;

	s->vtbl = *vtbl;
	s->backend = backend;
	s->mem = ctx->mem;

	*out = s;
	return STREAM_STATUS_OK;
}

void stream_destroy(stream_t *s)
{
	if (!s) return;

	const osal_mem_ops_t *mem = s->mem;
	stream_close_fn_t close_fn = s->vtbl.close;
	void *backend = s->backend;

	if (close_fn) {
		stream_status_t st = close_fn(backend);
		LEXLEO_ASSERT(st == STREAM_STATUS_OK || st == STREAM_STATUS_IO_ERROR);
	}
	LEXLEO_ASSERT(mem && mem->free);
	mem->free(s);
}

const stream_ops_t *stream_default_ops(void)
{
	static const stream_ops_t OPS = {
		.read = stream_read,
		.write = stream_write,
		.flush = stream_flush
	};
	return &OPS;
}
