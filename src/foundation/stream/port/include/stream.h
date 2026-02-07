// src/foundation/stream/port/include/stream.h

#ifndef LEXLEO_STREAM_H
#define LEXLEO_STREAM_H

#include "stream_types.h"
#include <stddef.h>

size_t stream_read(stream_t *s, void *buf, size_t n);
size_t stream_write(stream_t *s, const void *buf, size_t n);
int stream_flush(stream_t *s);

#endif //LEXLEO_STREAM_H