// src/frontend/ports/stream_legacy/src/stream_legacy.c

#include "internal/stream_legacy_internal.h"

#include <stdlib.h>

struct stream_legacy {
	const stream_legacy_vtable *ops;
	void *ctx; // opaque adapter-specific
};

stream_legacy *stream_legacy_create(const stream_legacy_vtable* vtbl, void *ctx) {
	stream_legacy *s = STREAM_MALLOC(sizeof(stream_legacy));
	if (!s) return NULL;
	s->ops  = vtbl;
	s->ctx = ctx;
	return s;
}

size_t stream_legacy_read(stream_legacy *s, void *b, size_t n) {
	return (s && s->ops && s->ops->read) ? s->ops->read(s->ctx, b, n) : 0;
}

size_t stream_legacy_write(stream_legacy *s, const void *b, size_t n) {
	return (s && s->ops && s->ops->write) ? s->ops->write(s->ctx, b, n) : 0;
}

int stream_legacy_flush(stream_legacy *s) {
    return (s && s->ops && s->ops->flush) ? s->ops->flush(s->ctx) : 0;
}

int stream_legacy_close(stream_legacy *s) {
	if (!s) return 0;
	int rc = 0;
	if (s->ops && s->ops->close) rc = s->ops->close(s->ctx);
	STREAM_FREE(s);
	return rc;
}


#ifdef UNIT_TEST
void *stream_legacy_get_ctx(stream_legacy *s) {
	if (!s) return NULL;
	return s->ctx;
}
#endif