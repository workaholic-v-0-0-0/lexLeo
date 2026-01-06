// src/foundation/input_provider_legacy/include/internal/input_provider_legacy_internal.h

#ifndef LEXLEO_INPUT_PROVIDER_INTERNAL_H
#define LEXLEO_INPUT_PROVIDER_INTERNAL_H

#include "input_provider_legacy.h"
#include "internal/input_provider_legacy_memory_allocator.h"

struct stream_legacy;

struct input_provider {
    input_provider_legacy_mode mode;
    struct stream_legacy *borrowed_stream; // borrowed from client code
    struct stream_legacy *chunks_stream; // owned
    yyscan_t lexer_scanner; // borrowed from lexer
};

size_t input_provider_legacy_read(input_provider *p, void *buf, size_t maxlen);

#endif //LEXLEO_INPUT_PROVIDER_INTERNAL_H
