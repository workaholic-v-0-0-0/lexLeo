// src/adapters/stream_legacy/fs_stream_legacy/include/internal/fs_stream_legacy_internal.h

#ifndef LEXLEO_FS_STREAM_INTERNAL_H
#define LEXLEO_FS_STREAM_INTERNAL_H

#include "fs_stream_legacy.h"
#include "internal/fs_stream_legacy_memory_allocator.h"

#include <stdio.h>
#include <stdbool.h>

stream_legacy *fs_stream_legacy_from_fp_for_tests(FILE *fp, bool autoclose);

#endif //LEXLEO_FS_STREAM_INTERNAL_H
