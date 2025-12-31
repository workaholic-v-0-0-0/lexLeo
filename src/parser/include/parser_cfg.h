// src/parser/include/parser_cfg.h

#ifndef LEXLEO_PARSER_CFG_H
#define LEXLEO_PARSER_CFG_H

#include "parser_types.h"
#include "ast.h"

typedef struct parser_ops {
    ast* (*create_int_node)(int);
    ast* (*create_string_node)(const char *);
    ast *(*create_symbol_name_node)(const char *);
	ast* (*create_error_node_or_sentinel)(ast_error_type code, const char *message);
    ast *(*create_children_node_var)(ast_type type, size_t children_nb,...);
    void (*destroy)(ast *root);
    bool (*children_append_take)(ast *parent, ast *child);
} parser_ops;

typedef struct parser_cfg {
    parser_ops ops;
    parse_goal_t goal;
	int syntax_errors;
} parser_cfg;

#endif //LEXLEO_PARSER_CFG_H
