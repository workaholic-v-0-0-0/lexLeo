// src/core/ast/include/internal/ast_test_utils.h

#ifndef AST_INTERNAL_H
#define AST_INTERNAL_H

#include "ast.h"

#include "internal/ast_memory_allocator.h"
#include "internal/ast_string_utils.h"

#ifdef UNIT_TEST

typedef void (*ast_destroy_typed_data_int_fn)(typed_data *typed_data_int);
void real_ast_destroy_typed_data_int(typed_data *typed_data_int);
extern ast_destroy_typed_data_int_fn ast_destroy_typed_data_int_mockable;
void set_ast_destroy_typed_data_int(ast_destroy_typed_data_int_fn f);

typedef void (*ast_destroy_typed_data_string_fn)(typed_data *typed_data_string);
void real_ast_destroy_typed_data_string(typed_data *typed_data_string);
extern ast_destroy_typed_data_string_fn ast_destroy_typed_data_string_mockable;
void set_ast_destroy_typed_data_string(ast_destroy_typed_data_string_fn f);

typedef void (*ast_destroy_typed_data_symbol_fn)(typed_data *typed_data_symbol);
void real_ast_destroy_typed_data_symbol(typed_data *typed_data_symbol);
extern ast_destroy_typed_data_symbol_fn ast_destroy_typed_data_symbol_mockable;
void set_ast_destroy_typed_data_symbol(ast_destroy_typed_data_symbol_fn f);

typedef void (*ast_destroy_typed_data_wrapper_fn)(ast *ast_data_wrapper);
void real_ast_destroy_typed_data_wrapper(ast *ast_data_wrapper);
extern ast_destroy_typed_data_wrapper_fn ast_destroy_typed_data_wrapper_mockable;
void set_ast_destroy_typed_data_wrapper(ast_destroy_typed_data_wrapper_fn f);

typedef void (*ast_destroy_children_node_fn)(ast *children_node);
void real_ast_destroy_children_node(ast *children_node);
extern ast_destroy_children_node_fn ast_destroy_children_node_mockable;
void set_ast_destroy_children_node(ast_destroy_children_node_fn f);

typedef ast_children_t * (*ast_create_ast_children_arr_fn)(size_t children_nb, ast **children);
ast_children_t *real_ast_create_ast_children_arr(size_t children_nb, ast **children);
extern ast_create_ast_children_arr_fn ast_create_ast_children_arr_mockable;
void set_ast_create_ast_children_arr(ast_create_ast_children_arr_fn f);

typedef void (*ast_destroy_ast_children_fn)(ast_children_t *ast_children);
void real_ast_destroy_ast_children(ast_children_t *ast_children);
extern ast_destroy_ast_children_fn ast_destroy_ast_children_mockable;
void set_ast_destroy_ast_children(ast_destroy_ast_children_fn f);

typedef void (*ast_destroy_fn)(ast *root);
void real_ast_destroy(ast *root);
extern ast_destroy_fn ast_destroy_mockable;
void set_ast_destroy(ast_destroy_fn f);

#endif

#endif //AST_INTERNAL_H
