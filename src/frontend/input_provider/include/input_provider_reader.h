// src/frontend/input_provider/include/input_provider_reader.h

#ifndef LEXLEO_INPUT_PROVIDER_READER_H
#define LEXLEO_INPUT_PROVIDER_READER_H

typedef struct input_provider input_provider;

size_t input_provider_read(input_provider *p, void *buf, size_t n);

#endif //LEXLEO_INPUT_PROVIDER_READER_H