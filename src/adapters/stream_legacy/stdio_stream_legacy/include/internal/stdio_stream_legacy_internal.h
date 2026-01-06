// src/adapters/stream_legacy/stdio_stream_legacy/include/internal/stdio_stream_legacy_internal.h

#ifndef LEXLEO_STDIO_STREAM_INTERNAL_H
#define LEXLEO_STDIO_STREAM_INTERNAL_H

#include "stdio_stream_legacy.h"
#include "internal/stdio_stream_legacy_memory_allocator.h"

#include <stdio.h>
#include <stdbool.h>

stream_legacy *stdio_stream_legacy_from_fp_for_tests(FILE *fp, bool autoclose);

#endif //LEXLEO_STDIO_STREAM_INTERNAL_H