// src/adapters/stream/fs_stream/src/fs_stream.c

#include "internal/fs_stream_internal.h"

#include "internal/stream_internal.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	FILE *fp;
	bool autoclose;
} fs_stream_ctx;

static size_t fs_stream_read(void *ctx, void* buf, size_t n) {
	return fread(buf, 1, n, ((fs_stream_ctx*)ctx)->fp);
}

static size_t fs_stream_write(void *ctx, const void* buf, size_t n) {
	return fwrite(buf, 1, n, ((fs_stream_ctx*)ctx)->fp);
}

static int fs_stream_flush(void *ctx) {
	return fflush(((fs_stream_ctx*)ctx)->fp);
}

static int fs_stream_close(void *ctx) {
	fs_stream_ctx *c = ctx;
	if (!c) return EOF;
	int ret = 0;
	if (c->autoclose && c->fp) ret = fclose(c->fp);
	FS_STREAM_FREE(c);
	return ret;
}

static const stream_vtable FS_STREAM_VTABLE = {
	.read = fs_stream_read,
	.write = fs_stream_write,
	.flush = fs_stream_flush,
	.close = fs_stream_close
};

static stream *fs_stream_from_fp(FILE *fp, bool autoclose) {
	if (!fp) return NULL;
	fs_stream_ctx *ctx = FS_STREAM_MALLOC(sizeof(fs_stream_ctx));
	if (!ctx) return NULL;
	ctx->fp = fp;
	ctx->autoclose = autoclose;
	stream *ret = stream_create(&FS_STREAM_VTABLE, ctx);
	if (!ret) FS_STREAM_FREE(ctx);
	return ret;
}

stream *fs_stream_from_file(const char *path) {
	FILE *fp = fopen(path, "r");
	if (!fp) return NULL;
	stream *ret = fs_stream_from_fp(fp, true);
	if (!ret) {
		fclose(fp);
		return NULL;
	}
	return ret;
}



#ifdef UNIT_TEST
stream *fs_stream_from_fp_for_tests(FILE *fp, bool autoclose) {
	return fs_stream_from_fp(fp, autoclose);
}
#endif
