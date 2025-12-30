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
    AST_TYPE_BLOCK_ITEMS,
    AST_TYPE_BLOCK,
    AST_TYPE_PARAMETERS,
    AST_TYPE_LIST_OF_PARAMETERS,
    AST_TYPE_FUNCTION,
    AST_TYPE_FUNCTION_DEFINITION,
    AST_TYPE_ARGUMENTS,
    AST_TYPE_LIST_OF_ARGUMENTS,
    AST_TYPE_FUNCTION_CALL,
    AST_TYPE_QUOTE,
    AST_TYPE_EVAL,
    AST_TYPE_NEGATION,
    AST_TYPE_ADDITION,
    AST_TYPE_SUBTRACTION,
    AST_TYPE_MULTIPLICATION,
    AST_TYPE_DIVISION,
	AST_TYPE_SYMBOL,
	AST_TYPE_SET,
	AST_TYPE_CONDITIONAL_BLOCK,
	AST_TYPE_WHILE_BLOCK,

	// leaf type
    AST_TYPE_DATA_WRAPPER,
	AST_TYPE_ERROR,
    AST_TYPE_NB_TYPES,
} ast_type;

#define AST_MAX_ARITY 16

typedef enum {
    TYPE_INT,
    TYPE_STRING, // can be code snippet
    TYPE_SYMBOL_NAME,
    TYPE_SYMBOL, // can be unbound or bound to another symbol
} data_type;

typedef enum {
	AST_ERROR_CODE_INT_NODE_CREATION_FAILED,
    AST_ERROR_CODE_STRING_NODE_CREATION_FAILED,
    AST_ERROR_CODE_SYMBOL_NAME_NODE_CREATION_FAILED,
    AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED,
    AST_ERROR_CODE_READING_NODE_CREATION_FAILED,
    AST_ERROR_CODE_WRITING_NODE_CREATION_FAILED,
    AST_ERROR_CODE_TRANSLATION_UNIT_NODE_CREATION_FAILED,
    AST_ERROR_CODE_TRANSLATION_UNIT_APPEND_FAILED,
    AST_ERROR_CODE_BLOCK_ITEMS_NODE_CREATION_FAILED,
    AST_ERROR_CODE_BLOCK_ITEMS_APPEND_FAILED,
    AST_ERROR_CODE_BLOCK_NODE_CREATION_FAILED,
    AST_ERROR_CODE_PARAMETERS_NODE_CREATION_FAILED,
    AST_ERROR_CODE_PARAMETERS_APPEND_FAILED,
    AST_ERROR_CODE_LIST_OF_PARAMETERS_NODE_CREATION_FAILED,
    AST_ERROR_CODE_FUNCTION_NODE_CREATION_FAILED,
    AST_ERROR_CODE_FUNCTION_DEFINITION_NODE_CREATION_FAILED,
    AST_ERROR_CODE_ARGUMENTS_NODE_CREATION_FAILED,
    AST_ERROR_CODE_ARGUMENTS_APPEND_FAILED,
    AST_ERROR_CODE_LIST_OF_ARGUMENTS_NODE_CREATION_FAILED,
    AST_ERROR_CODE_FUNCTION_CALL_NODE_CREATION_FAILED,
    AST_ERROR_CODE_QUOTE_NODE_CREATION_FAILED,
    AST_ERROR_CODE_EVAL_NODE_CREATION_FAILED,
	AST_ERROR_CODE_NEGATION_NODE_CREATION_FAILED,
	AST_ERROR_CODE_ADDITION_NODE_CREATION_FAILED,
	AST_ERROR_CODE_SUBTRACTION_NODE_CREATION_FAILED,
	AST_ERROR_CODE_MULTIPLICATION_NODE_CREATION_FAILED,
	AST_ERROR_CODE_DIVISION_NODE_CREATION_FAILED,
	AST_ERROR_CODE_SYMBOL_NODE_CREATION_FAILED,
	AST_ERROR_CODE_SET_NODE_CREATION_FAILED,
	AST_ERROR_CODE_CONDITIONAL_BLOCK_NODE_CREATION_FAILED,
	AST_ERROR_CODE_WHILE_BLOCK_NODE_CREATION_FAILED,
	// ...,
	AST_UNRETRIEVABLE_ERROR_CODE,
} ast_error_type;

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
    ast_error_type code;
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

ast *ast_error_sentinel(void);

typed_data *ast_create_typed_data_int(int i);

/**
 * Frees the typed_data container holding an int.
 */
void ast_destroy_typed_data_int(typed_data *typed_data_int);

typed_data *ast_create_typed_data_string(const char *s);

/**
 * Frees the typed_data container and its owned string.
 */
void ast_destroy_typed_data_string(typed_data *typed_data_string);

typed_data *ast_create_typed_data_symbol_name(const char *s);

/**
 * Frees the typed_data container and its owned string.
 */
void ast_destroy_typed_data_symbol_name(typed_data *typed_data_symbol_name);

// borrowed symbol*: not freed by the caller nor by the AST
// lifetime managed by the runtime_session
typed_data *ast_create_typed_data_symbol(symbol *s);

/**
 * Frees the typed_data container.
 * Does not free the underlying symbol* (owned by the symtab).
 */
void ast_destroy_typed_data_symbol(typed_data *typed_data_symbol);

// the caller must not free data
ast *ast_create_typed_data_wrapper(typed_data *data);

/**
 * Frees the ast typed_data container by freeing it typed_data
 * with the appropriate ast_destroy_typed_data_*
 */
void ast_destroy_typed_data_wrapper(ast *ast_data_wrapper);

ast *ast_create_int_node(int i);

// Copies `str` internally (strdup). The AST owns the copy.
// The caller remains responsible for the original pointer (if any).
ast *ast_create_string_node(const char *str);

// Copies `str` internally (strdup). The AST owns the copy.
ast *ast_create_symbol_name_node(const char *str);

// Borrowed symbol pointer (NOT owned by the AST).
// lifetime of symbol is managed by runtime_session.
ast *ast_create_symbol_node(symbol *sym);

ast *ast_create_error_node(ast_error_type code, const char *message); // client code is responsible for message
void ast_destroy_error_node(ast *ast_error_node); // client code is responsible for providing either NULL or a correctly formed ast of type AST_TYPE_ERROR
ast *ast_create_error_node_or_sentinel(ast_error_type code, const char *message); // client code is responsible for message

// caller is responsible for children_nb correctness
ast_children_t *ast_create_ast_children_arr(size_t children_nb, ast **children);

// client code is responsible for the argument number correctness
ast_children_t *ast_create_ast_children_var(size_t children_nb,...);

// client code is responsible for children_nb correctness
void ast_destroy_ast_children(ast_children_t *ast_children);

// return false on reallocation error or true otherwise ; client code is responsible for children_nb and capacity field values correctness
bool ast_children_reserve(ast_children_t *ast_children, size_t capacity);

// returns true on succees and false on error ; client code is responsible for passing NULL or well-formed asts with children_nb and capacity correctness
bool ast_children_append_take(ast *parent, ast *child);

// client code is responsible for providing a correctly formed ast_children
ast *ast_create_children_node(ast_type type, ast_children_t *ast_children);

// client code is responsible for children_nb and capacity values correctness
ast *ast_create_children_node_arr(ast_type type, size_t children_nb, ast **children);

// client code is responsible for children_nb and capacity values correctness
ast *ast_create_children_node_var(ast_type type, size_t children_nb,...);

void ast_destroy_children_node(ast *children_node);

// the caller is responsible for passing either NULL or a well-formed ast pointer
void ast_destroy(ast *root);

bool ast_type_has_children(ast_type type);
bool ast_can_have_children(ast *a);
bool ast_has_any_child(ast *a);
bool ast_is_data_of(const ast *a, data_type dt);

char *ast_serialize(ast *root);
ast *ast_deserialize(char *);

// DEBUG TOOLS
const char *ast_type_to_string(ast_type t);
void ast_print(const ast *root);
void ast_print_limited(const ast *root, int max_depth);

#endif //AST_H
