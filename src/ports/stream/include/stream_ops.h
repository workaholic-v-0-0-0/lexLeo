// src/ports/stream/include/stream_ops.h

#ifndef LEXLEO_STREAM_OPS_H
#define LEXLEO_STREAM_OPS_H

#include "stream_types.h"
#include <stddef.h>

typedef struct stream_ops_t {
	size_t (*read)(stream_t *s, void *buf, size_t n);
	size_t (*write)(stream_t *s, const void *buf, size_t n);
	int (*flush)(stream_t *s);
} stream_ops_t;

const stream_ops_t *stream_default_ops(void);

#endif //LEXLEO_STREAM_OPS_H