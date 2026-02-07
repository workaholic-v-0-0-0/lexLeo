// src/foundation/stream/port/src/stream.c

#include "stream.h"
#include "stream_ops.h"
#include "internal/stream_ctx.h"
#include "internal/stream_internal.h"
#include "lexleo_assert.h"

stream_status_t stream_create(
		stream_t **out,
		const stream_vtbl_t *vtbl,
		void *backend,
		const stream_ctx_t *ctx ) {
	if (out) *out = NULL;
	if (!out || !vtbl || !vtbl->read || !vtbl->write || !ctx || !ctx->mem)
		return STREAM_STATUS_ERROR;

	stream_t *s = ctx->mem->malloc(sizeof(*s));
	if (!s) return STREAM_STATUS_ERROR;

	s->vtbl = *vtbl;
	s->backend = backend;
	s->mem = ctx->mem;

	*out = s;
	return STREAM_STATUS_OK;
}

void stream_destroy(stream_t *s)
{
	if (!s) return;
	if (s->vtbl.close) s->vtbl.close(s->backend);
	LEXLEO_ASSERT(s->mem && s->mem->free);
	s->mem->free(s);
}

size_t stream_read(stream_t *s, void *buf, size_t n) {
	if (!s || !s->vtbl.read || (n && !buf)) return 0;
	return s->vtbl.read(s->backend, buf, n);
}

size_t stream_write(stream_t *s, const void *buf, size_t n) {
	if (!s || !s->vtbl.write || (n && !buf)) return 0;
	return s->vtbl.write(s->backend, buf, n);
}

int stream_flush(stream_t *s) {
	if (!s || !s->vtbl.flush) return 0;
	return s->vtbl.flush(s->backend);
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
