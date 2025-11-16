// src/adapters/stream/stdio_stream/include/internal/stdio_stream_internal.h

#ifndef LEXLEO_STDIO_STREAM_INTERNAL_H
#define LEXLEO_STDIO_STREAM_INTERNAL_H

#include "stdio_stream.h"
#include "internal/stdio_stream_memory_allocator.h"

#include <stdio.h>
#include <stdbool.h>

stream *stdio_stream_from_fp_for_tests(FILE *fp, bool autoclose);

#endif //LEXLEO_STDIO_STREAM_INTERNAL_H