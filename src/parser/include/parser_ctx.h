// src/parser/include/parser_ctx.h

#ifndef LEXLEO_PARSER_CTX_H
#define LEXLEO_PARSER_CTX_H

#include "ast.h"

typedef struct parser_ops {
    ast* (*create_int_node)(int);
    ast* (*create_string_node)(char *);
    ast *(*create_symbol_name_node)(char *);
	ast* (*create_error_node_or_sentinel)(error_type code, char *message);
    ast *(*create_children_node_var)(ast_type type, size_t children_nb,...);
    void (*destroy)(ast *root);
    bool (*children_append_take)(ast *parent, ast *child);
} parser_ops;

typedef struct parser_ctx {
    parser_ops ops;
} parser_ctx;

#endif //LEXLEO_PARSER_CTX_H
