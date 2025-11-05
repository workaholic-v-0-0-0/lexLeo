// src/io/include/input_provider.h

#ifndef LEXLEO_INPUT_PROVIDER_H
#define LEXLEO_INPUT_PROVIDER_H

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "lexer.yy.h"

typedef enum {
    INPUT_FROM_CSTRING,
    INPUT_FROM_FILE
} input_source_kind_t;

typedef struct input_provider input_provider;

input_provider *input_provider_from_cstring(const char *src, bool make_copy);
input_provider *input_provider_from_file(FILE *f, bool take_ownership);
bool input_provider_bind_to_scanner(input_provider *p, yyscan_t scanner);
bool input_provider_rewind(input_provider *p);
FILE *input_provider_file(input_provider *p);
input_source_kind_t input_provider_kind(const input_provider *p);
void input_provider_destroy(input_provider *p);

#endif //LEXLEO_INPUT_PROVIDER_H