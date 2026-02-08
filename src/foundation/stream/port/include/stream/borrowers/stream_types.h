// src/foundation/stream/port/include/borrowers/stream_types.h

#ifndef LEXLEO_STREAM_TYPES_H
#define LEXLEO_STREAM_TYPES_H

#include <stddef.h>

typedef struct stream_t stream_t;

typedef enum {
	STREAM_STATUS_OK = 0,
	STREAM_STATUS_EOF,
	STREAM_STATUS_INVALID,
	STREAM_STATUS_IO_ERROR,
	STREAM_STATUS_NO_BACKEND,
	STREAM_STATUS_OOM
} stream_status_t;

typedef struct stream_ops_t {
	size_t (*read)(stream_t *s, void *buf, size_t n, stream_status_t *st);
	size_t (*write)(stream_t *s, const void *buf, size_t n, stream_status_t *st);
	stream_status_t (*flush)(stream_t *s);
} stream_ops_t;

#endif //LEXLEO_STREAM_TYPES_H
