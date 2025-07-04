// src/include/parser_types.h

#ifndef PARSER_TYPES_H
#define PARSER_TYPES_H

typedef struct ast ast;
typedef struct symtab symtab;
typedef struct context { symtab *st; } *context;

// forward declaration of yyscan_t for Bison (as Flex generates it)
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
extern void* yyget_extra(yyscan_t scanner);
#endif

#endif //PARSER_TYPES_H
