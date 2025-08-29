// src/parser/include/parser_ctx.h

#ifndef LEXLEO_PARSER_CTX_H
#define LEXLEO_PARSER_CTX_H

#include "ast.h"

typedef struct parser_ops {
    ast* (*create_int_node)(int);
	ast* (*create_error_node)(error_type code, char *message);
	ast* (*error_sentinel)(void);
} parser_ops;

typedef struct parser_ctx {
    parser_ops ops;
} parser_ctx;

#endif //LEXLEO_PARSER_CTX_H
