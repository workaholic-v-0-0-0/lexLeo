// src/adapters/stream_legacy/stdio_stream_legacy/src/stdio_stream_legacy.c

#include "internal/stdio_stream_legacy_internal.h"

#include "internal/stream_legacy_internal.h"

#include <stdio.h>
#include <stdbool.h>

typedef struct {
	FILE *fp;
	bool autoclose; // usefull for tests
} stdio_stream_legacy_ctx;

static size_t stdio_stream_legacy_read(void *ctx, void* buf, size_t n) {
	return fread(buf, 1, n, ((stdio_stream_legacy_ctx*)ctx)->fp);
}

static size_t stdio_stream_legacy_write(void *ctx, const void* buf, size_t n) {
	return fwrite(buf, 1, n, ((stdio_stream_legacy_ctx*)ctx)->fp);
}

static int stdio_stream_legacy_flush(void *ctx) {
	return fflush(((stdio_stream_legacy_ctx*)ctx)->fp);
}

static int stdio_stream_legacy_close(void *ctx) {
	stdio_stream_legacy_ctx *c = ctx;
	if (!c) return EOF;
	int ret = 0;
	if (c->autoclose && c->fp) ret = fclose(c->fp);
	STDIO_STREAM_FREE(c);
	return ret;
}

static const stream_legacy_vtable STDIO_STREAM_VTABLE = {
	.read = stdio_stream_legacy_read,
	.write = stdio_stream_legacy_write,
	.flush = stdio_stream_legacy_flush,
	.close = stdio_stream_legacy_close
};

static stream_legacy *stdio_stream_legacy_from_fp(FILE *fp, bool autoclose) {
	if (!fp) return NULL;
	stdio_stream_legacy_ctx *ctx = STDIO_STREAM_MALLOC(sizeof(stdio_stream_legacy_ctx));
	if (!ctx) return NULL;
	ctx->fp = fp;
	ctx->autoclose = autoclose;
	stream_legacy *ret = stream_legacy_create(&STDIO_STREAM_VTABLE, ctx);
	if (!ret) {
		STDIO_STREAM_FREE(ctx);
		return NULL;
	}
	return ret;
}

stream_legacy *stdio_stream_legacy_from_stdin(void) {
	return stdio_stream_legacy_from_fp(stdin, false);
}

stream_legacy *stdio_stream_legacy_to_stdout(void) {
	return stdio_stream_legacy_from_fp(stdout, false);
}

stream_legacy *stdio_stream_legacy_to_stderr(void) {
	return stdio_stream_legacy_from_fp(stderr, false);
}



#ifdef UNIT_TEST
stream_legacy *stdio_stream_legacy_from_fp_for_tests(FILE *fp, bool autoclose) {
	return stdio_stream_legacy_from_fp(fp, autoclose);
}
#endif
