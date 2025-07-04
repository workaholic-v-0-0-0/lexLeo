// src/ast/include/ast.h

#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    AST_TYPE_PROGRAM,
    AST_TYPE_STATEMENT,
    //AST_TYPE_LIST_OF_PARAMETERS,
    //AST_TYPE_PARAMETER,
    AST_TYPE_BINDING,
    AST_TYPE_EVALUATION,
    AST_TYPE_EXECUTION,
    AST_TYPE_COMPUTATION,
    //AST_TYPE_READING,
    //AST_TYPE_WRITING,
    AST_TYPE_ADDITION,
    // other elementary operations
    AST_TYPE_DATA_WRAPPER, // leaf
    AST_TYPE_NB_TYPES,
} ast_type;

typedef enum {
    TYPE_INT,
    TYPE_STRING, // can be code snippet
    TYPE_SYMBOL, // can be unbound or bound to another symbol
    //TYPE_FUNCTION,
} data_type;

// forward declaration to handle cross-dependency
typedef struct symbol symbol;

typedef struct {
    data_type type;
    union {
        int int_value;
        char *string_value;
        symbol *symbol_value;
        //function *function_value;
    } data;
} typed_data;

typedef struct ast_children_t {
    size_t children_nb;
    struct ast **children;
} ast_children_t;

typedef struct ast {
    ast_type type;
    union {
        ast_children_t *children;
        typed_data *data;
    };
} ast;

typed_data *ast_create_typed_data_int(int i);
void ast_destroy_typed_data_int(typed_data *typed_data_int);

typed_data *ast_create_typed_data_string(char *s);
void ast_destroy_typed_data_string(typed_data *typed_data_string);

typed_data *ast_create_typed_data_symbol(symbol *s); // client code is responsible of s
void ast_destroy_typed_data_symbol(typed_data *typed_data_symbol); // note: will call ast_destroy because symbol has field of type ast

ast *ast_create_typed_data_wrapper(typed_data *data); // client code is responsible of data
void ast_destroy_typed_data_wrapper(ast *ast_data_wrapper);

ast_children_t *ast_create_ast_children_arr(size_t children_nb, ast **children); // client code is responsible for children_nb value correctness and for destroying chidren array (but not the ast * it contains)
ast_children_t *ast_create_ast_children_var(size_t children_nb,...); // client code is responsible for the argument number correctness ; a double pointer of ast can be malloced (eg when no child)
void ast_destroy_ast_children(ast_children_t *ast_children); // client code is responsible for children_nb value correctness

ast *ast_create_non_typed_data_wrapper(ast_type type, ast_children_t *ast_children); // client code is responsible for providing a correctly formed ast_children
ast *ast_create_non_typed_data_wrapper_arr(ast_type type, size_t children_nb, ast **children); // client code is responsible for children_nb value correctness and for destroying children array (but not the ast * it contains)
ast *ast_create_non_typed_data_wrapper_var(ast_type type, size_t children_nb,...); // client code is responsible for children_nb value correctness
void ast_destroy_non_typed_data_wrapper(ast *non_typed_data_wrapper);

void ast_destroy(ast *root); // the caller is responsible for passing either NULL or a well-formed ast pointer

char *ast_serialize(ast *root);
ast *ast_deserialize(char *);

#endif //AST_H
