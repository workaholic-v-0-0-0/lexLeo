// src/frontend/input_provider/include/internal/input_provider_internal.h

#ifndef LEXLEO_INPUT_PROVIDER_INTERNAL_H
#define LEXLEO_INPUT_PROVIDER_INTERNAL_H

#include "input_provider.h"
#include "internal/input_provider_memory_allocator.h"

struct stream;

struct input_provider {
    input_provider_mode mode;
    struct stream *borrowed_stream; // borrowed from client code
    struct stream *chunks_stream; // owned
    yyscan_t lexer_scanner; // borrowed from lexer
};

size_t input_provider_read(input_provider *p, void *buf, size_t maxlen);

#endif //LEXLEO_INPUT_PROVIDER_INTERNAL_H
