// src/adapters/stream/fs_stream/src/fs_stream.c

#include "fs_stream.h"

#include "internal/stream_internal.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	FILE *fp;
} fs_stream_ctx;

stream *fs_stream_from_file(const char *path) {
	return NULL; // placeholder
}
