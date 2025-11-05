// src/include/parser_types.h

#ifndef PARSER_TYPES_H
#define PARSER_TYPES_H

typedef struct ast ast;
typedef struct symtab symtab;
typedef struct context { symtab *st; } *context;

typedef enum {
    PARSE_GOAL_TU,
    PARSE_GOAL_ONE_STATEMENT,
    PARSE_GOAL_READABLE,
} parse_goal_t;

typedef struct lexer_extra {
    parse_goal_t goal;
    int sent_mode_token;
    int reached_input_end;
} lexer_extra_t;

typedef enum {
    PARSE_STATUS_OK,
    PARSE_STATUS_EOF,
    PARSE_STATUS_INCOMPLETE,
    PARSE_STATUS_ERROR
} parse_status;

// forward declaration of yyscan_t for Bison (as Flex generates it)
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

#endif //PARSER_TYPES_H
