// src/adapters/stream/stdio_stream/src/stdio_stream.c

#include "stdio_stream.h"

#include "internal/stream_internal.h"

#include <stdio.h>
#include <stdbool.h>

typedef struct {
	FILE *fp;
	bool  autoclose;
} stdio_stream_ctx;

static stream *stdio_stream_from_fp(FILE *fp, bool autoclose) {
	return NULL; // placeholder
}

static stream *stdio_stream_from_fp_owning(FILE *fp) {
	return NULL; // placeholder
}

static stream *stdio_stream_from_fp_borrowing(FILE *fp) {
	return NULL; // placeholder
}

stream *stdio_stream_from_stdin(void) {
	return NULL; // placeholder
}

stream *stdio_stream_to_stdout(void) {
	return NULL; // placeholder
}

stream *stdio_stream_to_stderr(void) {
	return NULL; // placeholder
}
