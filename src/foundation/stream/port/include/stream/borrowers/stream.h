// src/foundation/stream/port/include/borrowers/stream.h

#ifndef LEXLEO_STREAM_H
#define LEXLEO_STREAM_H

#include "stream/borrowers/stream_types.h"

#include <stddef.h>

size_t stream_read(stream_t *s, void *buf, size_t n, stream_status_t *st);
size_t stream_write(stream_t *s, const void *buf, size_t n, stream_status_t *st);
stream_status_t stream_flush(stream_t *s);

#endif //LEXLEO_STREAM_H