// src/adapters/stream_legacy/stdio_stream_legacy/include/stdio_stream_legacy.h

#ifndef LEXLEO_STDIO_STREAM_H
#define LEXLEO_STDIO_STREAM_H

#include "stream_legacy.h"

stream_legacy *stdio_stream_legacy_from_stdin(void);
stream_legacy *stdio_stream_legacy_to_stdout(void);
stream_legacy *stdio_stream_legacy_to_stderr(void);

#endif //LEXLEO_STDIO_STREAM_H
