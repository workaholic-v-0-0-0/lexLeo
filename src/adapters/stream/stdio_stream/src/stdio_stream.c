// src/adapters/stream/stdio_stream/src/stdio_stream.c

#include "internal/stdio_stream_internal.h"

#include "internal/stream_internal.h"

#include <stdio.h>
#include <stdbool.h>

typedef struct {
	FILE *fp;
	bool autoclose; // usefull for tests
} stdio_stream_ctx;

static size_t stdio_stream_read(void *ctx, void* buf, size_t n) {
	return fread(buf, 1, n, ((stdio_stream_ctx*)ctx)->fp);
}

static size_t stdio_stream_write(void *ctx, const void* buf, size_t n) {
	return fwrite(buf, 1, n, ((stdio_stream_ctx*)ctx)->fp);
}

static int stdio_stream_flush(void *ctx) {
	return fflush(((stdio_stream_ctx*)ctx)->fp);
}

static int stdio_stream_close(void *ctx) {
	stdio_stream_ctx *c = ctx;
	if (!c) return EOF;
	int ret = 0;
	if (c->autoclose && c->fp) ret = fclose(c->fp);
	STDIO_STREAM_FREE(c);
	return ret;
}

const stream_vtable STDIO_STREAM_VTABLE = {
	.read = stdio_stream_read,
	.write = stdio_stream_write,
	.flush = stdio_stream_flush,
	.close = stdio_stream_close
};

static stream *stdio_stream_from_fp(FILE *fp, bool autoclose) {
	if (!fp) return NULL;
	stdio_stream_ctx *ctx = STDIO_STREAM_MALLOC(sizeof(stdio_stream_ctx));
	if (!ctx) return NULL;
	ctx->fp = fp;
	ctx->autoclose = autoclose;
	stream *ret = stream_create(&STDIO_STREAM_VTABLE, ctx);
	if (!ret) {
		STDIO_STREAM_FREE(ctx);
		return NULL;
	}
	return ret;
}

stream *stdio_stream_from_stdin(void) {
	return stdio_stream_from_fp(stdin, false);
}

stream *stdio_stream_to_stdout(void) {
	return stdio_stream_from_fp(stdout, false);
}

stream *stdio_stream_to_stderr(void) {
	return stdio_stream_from_fp(stderr, false);
}



#ifdef UNIT_TEST
stream *stdio_stream_from_fp_for_tests(FILE *fp, bool autoclose) {
	return stdio_stream_from_fp(fp, autoclose);
}
#endif
