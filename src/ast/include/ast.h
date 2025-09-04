// src/ast/include/ast.h

#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    AST_TYPE_BINDING,
	AST_TYPE_READING,
    AST_TYPE_WRITING,
    AST_TYPE_TRANSLATION_UNIT,
    //AST_TYPE_STATEMENT, useless?
    //AST_TYPE_LIST_OF_PARAMETERS,
    //AST_TYPE_PARAMETER,
    //AST_TYPE_EVALUATION,
    //AST_TYPE_EXECUTION,
    AST_TYPE_COMPUTATION,
    AST_TYPE_ADDITION,
    // other elementary operations
    AST_TYPE_DATA_WRAPPER, // leaf
	AST_TYPE_ERROR, // leaf
    AST_TYPE_NB_TYPES,
} ast_type;

typedef enum {
    TYPE_INT,
    TYPE_STRING, // can be code snippet
    TYPE_SYMBOL_NAME,
    TYPE_SYMBOL, // can be unbound or bound to another symbol
    //TYPE_FUNCTION,
} data_type;

typedef enum {
	AST_ERROR_CODE_INT_NODE_CREATION_FAILED,
    AST_ERROR_CODE_STRING_NODE_CREATION_FAILED,
    AST_ERROR_CODE_SYMBOL_NAME_NODE_CREATION_FAILED,
    AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED,
    AST_ERROR_CODE_READING_NODE_CREATION_FAILED,
    AST_ERROR_CODE_WRITING_NODE_CREATION_FAILED,
	// ...,
	UNRETRIEVABLE_ERROR_CODE,
} error_type;

// forward declaration to handle cross-dependency
typedef struct symbol symbol;

typedef struct ast_children_t {
    size_t children_nb;
    size_t capacity;
    struct ast **children;
} ast_children_t;

typedef struct {
    data_type type;
    union {
        int int_value;
        char *string_value;
        symbol *symbol_value;
        //function *function_value;
    } data;
} typed_data;

#define MAXIMUM_ERROR_MESSAGE_LENGTH 255
typedef struct {
    error_type code;
    char *message;
    bool is_sentinel; // true for the static fallback node
    // YYLTYPE loc; // later?
} error_info;

typedef struct ast {
    ast_type type;
    union {
        ast_children_t *children;
        typed_data *data;
        error_info *error;
    };
} ast;

ast * ast_error_sentinel(void);

typed_data *ast_create_typed_data_int(int i);
void ast_destroy_typed_data_int(typed_data *typed_data_int);

typed_data *ast_create_typed_data_string(char *s); // s can be NULL
void ast_destroy_typed_data_string(typed_data *typed_data_string);

typed_data *ast_create_typed_data_symbol_name(char *s);
void ast_destroy_typed_data_symbol_name(typed_data *typed_data_symbol_name);

typed_data *ast_create_typed_data_symbol(symbol *s); // client code is responsible for s
void ast_destroy_typed_data_symbol(typed_data *typed_data_symbol); // does not destroy the symbol* (owned by the symtab)

ast *ast_create_typed_data_wrapper(typed_data *data); // client code is responsible for data
void ast_destroy_typed_data_wrapper(ast *ast_data_wrapper);

ast *ast_create_int_node(int i);
ast *ast_create_string_node(char *str); // client code is responsible for str
ast *ast_create_symbol_name_node(char *str); // client code is responsible for str
ast *ast_create_symbol_node(symbol *sym); // client code is responsible for s

ast *ast_create_error_node(error_type code, char *message); // client code is responsible for message
void ast_destroy_error_node(ast *ast_error_node); // client code is responsible for providing either NULL or a correctly formed ast of type AST_TYPE_ERROR
ast *ast_create_error_node_or_sentinel(error_type code, char *message); // client code is responsible for message

ast_children_t *ast_create_ast_children_arr(size_t children_nb, ast **children); // client code is responsible for children_nb and capacity values correctness and for destroying chidren array (but not the ast * it contains)
ast_children_t *ast_create_ast_children_var(size_t children_nb,...); // client code is responsible for the argument number correctness ; a double pointer of ast can be malloced (eg when no child)
void ast_destroy_ast_children(ast_children_t *ast_children); // client code is responsible for children_nb and capacity values correctness
bool ast_children_reserve(ast_children_t *ast_children, size_t capacity); // return false on reallocation error or true otherwise ; client code is responsible for children_nb and capacity field values correctness
bool ast_children_append_take(ast *parent, ast *child); // returns true on succees and false on error ; client code is responsible for passing NULL or well-formed asts with children_nb and capacity correctness

ast *ast_create_children_node(ast_type type, ast_children_t *ast_children); // client code is responsible for providing a correctly formed ast_children
ast *ast_create_children_node_arr(ast_type type, size_t children_nb, ast **children); // client code is responsible for children_nb and capacity values correctness and for destroying children array (but not the ast * it contains)
ast *ast_create_children_node_var(ast_type type, size_t children_nb,...); // client code is responsible for children_nb and capacity values correctness
void ast_destroy_children_node(ast *children_node);

void ast_destroy(ast *root); // the caller is responsible for passing either NULL or a well-formed ast pointer

char *ast_serialize(ast *root);
ast *ast_deserialize(char *);

#endif //AST_H
