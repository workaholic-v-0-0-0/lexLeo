// src/frontend/ports/stream/src/stream.c

#include "internal/stream_internal.h"

#include <stdlib.h>

struct stream {
	const stream_vtable *ops;
	void *ctx; // opaque adapter-specific
};

stream *stream_create(const stream_vtable* vtbl, void *ctx) {
	stream *s = STREAM_MALLOC(sizeof(stream));
	if (!s) return NULL;
	s->ops  = vtbl;
	s->ctx = ctx;
	return s;
}

size_t stream_read(stream *s, void *b, size_t n) {
	return (s && s->ops && s->ops->read) ? s->ops->read(s->ctx, b, n) : 0;
}

size_t stream_write(stream *s, const void *b, size_t n) {
	return (s && s->ops && s->ops->write) ? s->ops->write(s->ctx, b, n) : 0;
}

int stream_flush(stream *s) {
    return (s && s->ops && s->ops->flush) ? s->ops->flush(s->ctx) : 0;
}

int stream_close(stream *s) {
	if (!s) return 0;
	int rc = 0;
	if (s->ops && s->ops->close) rc = s->ops->close(s->ctx);
	STREAM_FREE(s);
	return rc;
}


#ifdef UNIT_TEST
void *stream_get_ctx(stream *s) {
	if (!s) return NULL;
	return s->ctx;
}
#endif