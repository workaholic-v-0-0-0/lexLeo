// src/adapters/stream/fs_stream/include/internal/fs_stream_internal.h

#ifndef LEXLEO_FS_STREAM_INTERNAL_H
#define LEXLEO_FS_STREAM_INTERNAL_H

#include "fs_stream.h"
#include "internal/fs_stream_memory_allocator.h"

#include <stdio.h>
#include <stdbool.h>

stream *fs_stream_from_fp_for_tests(FILE *fp, bool autoclose);

#endif //LEXLEO_FS_STREAM_INTERNAL_H
