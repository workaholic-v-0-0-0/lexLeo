// src/adapters/stream/dynamic_buffer_stream/src/dynamic_buffer_stream.c

#include "internal/dynamic_buffer_stream_internal.h"
#include "internal/stream_internal.h"

#include <string.h>

static size_t dynamic_buffer_stream_read(void *ctx, void* buf, size_t n) {
	if (!ctx || !buf) return 0;

	dynamic_buffer_stream_ctx *dbs = (dynamic_buffer_stream_ctx*)ctx;
	if (dbs->len <= dbs->read_pos) return 0;

	size_t ret = (dbs->len - dbs->read_pos < n) ? dbs->len - dbs->read_pos : n;
	memcpy(buf, dbs->buf + dbs->read_pos, ret);
	dbs->read_pos += ret;

	return ret;
}

static bool buffer_reserve(dynamic_buffer_stream_ctx *dbs, size_t cap) {
	if (!dbs) return false;
	if (dbs->cap >= cap) return true;
	void *new_buf = DYNAMIC_BUFFER_STREAM_REALLOC(dbs->buf, sizeof(char) * cap);
	if (!new_buf) return false;
	dbs->cap = cap;
	dbs->buf = new_buf;
	return true;
}

static size_t next_cap(size_t cap) {
	return 2 * cap;
}

static size_t dynamic_buffer_stream_write(void *ctx, const void* buf, size_t n) {
	if (!ctx || !buf) return 0;

	dynamic_buffer_stream_ctx *dbs = (dynamic_buffer_stream_ctx*)ctx;

	size_t new_cap = dbs->cap;
	while (dbs->len + n > new_cap) new_cap = next_cap(new_cap);
	if (!buffer_reserve(dbs, new_cap)) return false;

	memcpy(dbs->buf + dbs->len, buf, n);
	dbs->len += n;

	return n;
}

// do it but be aware this is not a kind of stdio flush
static int dynamic_buffer_stream_flush(void *ctx) {
	if (!ctx) return -1;

	dynamic_buffer_stream_ctx *dbs = (dynamic_buffer_stream_ctx*)ctx;

	if (dbs->read_pos == 0) return 0;

	if (dbs->read_pos >= dbs->len) {
		dbs->len = 0;
		dbs->read_pos = 0;
		return 0;
	}

	size_t remaining = dbs->len - dbs->read_pos;
	memmove(dbs->buf, dbs->buf + dbs->read_pos, remaining);
	dbs->len = remaining;
	dbs->read_pos = 0;

	return 0;
}

static int dynamic_buffer_stream_close(void *ctx) {
	if (!ctx) return -1;
	if (((dynamic_buffer_stream_ctx*)ctx)->autoclose)
		DYNAMIC_BUFFER_STREAM_FREE(((dynamic_buffer_stream_ctx*)ctx)->buf);
	DYNAMIC_BUFFER_STREAM_FREE(ctx);
	return 0;
}

static const stream_vtable DYNAMIC_BUFFER_STREAM_VTABLE = {
	.read = dynamic_buffer_stream_read,
	.write = dynamic_buffer_stream_write,
	.flush = dynamic_buffer_stream_flush,
	.close = dynamic_buffer_stream_close
};

stream *dynamic_buffer_stream_create() {
	dynamic_buffer_stream_ctx *ctx = DYNAMIC_BUFFER_STREAM_MALLOC(sizeof(dynamic_buffer_stream_ctx));
	if (!ctx) return NULL;

	ctx->buf = DYNAMIC_BUFFER_STREAM_MALLOC(sizeof(char) * DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	if (!ctx->buf) {
		DYNAMIC_BUFFER_STREAM_FREE(ctx);
		return NULL;
	}

	ctx->cap = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY;
	ctx->len = 0;
	ctx->read_pos = 0;
	ctx->autoclose = true;

	return stream_create(&DYNAMIC_BUFFER_STREAM_VTABLE, ctx);
}
