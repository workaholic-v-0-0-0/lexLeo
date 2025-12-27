// src/frontend/ports/stream/include/stream.h

#ifndef LEXLEO_STREAM_H
#define LEXLEO_STREAM_H

#include <stddef.h>

typedef struct stream stream;

size_t stream_read(stream *s, void *buf, size_t n);
size_t stream_write(stream *s, const void *buf, size_t n);
int stream_flush(stream *s);
int stream_close(stream *s); // close, free s->ctx via adapter and then free s

#endif //LEXLEO_STREAM_H
