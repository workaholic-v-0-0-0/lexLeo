// src/adapters/stream/stdio_stream/include/stdio_stream.h

#ifndef LEXLEO_STDIO_STREAM_H
#define LEXLEO_STDIO_STREAM_H

#include "stream.h"

stream *stdio_stream_from_stdin(void);
stream *stdio_stream_to_stdout(void);
stream *stdio_stream_to_stderr(void);

#endif //LEXLEO_STDIO_STREAM_H
