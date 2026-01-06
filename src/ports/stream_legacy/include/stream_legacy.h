// src/ports/stream_legacy/include/stream_legacy.h

#ifndef LEXLEO_STREAM_H
#define LEXLEO_STREAM_H

#include <stddef.h>

typedef struct stream_legacy stream_legacy;

size_t stream_legacy_read(stream_legacy *s, void *buf, size_t n);
size_t stream_legacy_write(stream_legacy *s, const void *buf, size_t n);
int stream_legacy_flush(stream_legacy *s);
int stream_legacy_close(stream_legacy *s); // close, free s->ctx via adapter and then free s

#endif //LEXLEO_STREAM_H
