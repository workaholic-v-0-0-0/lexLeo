// src/ast/src/internal/ast_test_utils.c

#include "internal/ast_test_utils.h"

#ifdef UNIT_TEST

ast_destroy_typed_data_int_fn ast_destroy_typed_data_int_mockable = real_ast_destroy_typed_data_int;
void real_ast_destroy_typed_data_int(typed_data *typed_data_int) {
    AST_FREE(typed_data_int);
}
void set_ast_destroy_typed_data_int(ast_destroy_typed_data_int_fn f) {
    ast_destroy_typed_data_int_mockable = f ? f : real_ast_destroy_typed_data_int;
}

ast_destroy_typed_data_string_fn ast_destroy_typed_data_string_mockable = real_ast_destroy_typed_data_string;
void real_ast_destroy_typed_data_string(typed_data *typed_data_string) {
    AST_FREE(typed_data_string->data.string_value);
    AST_FREE(typed_data_string);
}
void set_ast_destroy_typed_data_string(ast_destroy_typed_data_string_fn f) {
    ast_destroy_typed_data_string_mockable = f ? f : real_ast_destroy_typed_data_string;
}

ast_destroy_typed_data_symbol_fn ast_destroy_typed_data_symbol_mockable = real_ast_destroy_typed_data_symbol;
void real_ast_destroy_typed_data_symbol(typed_data *typed_data_symbol) {
    AST_FREE(typed_data_symbol);
}
void set_ast_destroy_typed_data_symbol(ast_destroy_typed_data_symbol_fn f) {
    ast_destroy_typed_data_symbol_mockable = f ? f : real_ast_destroy_typed_data_symbol;
}

ast_destroy_typed_data_wrapper_fn ast_destroy_typed_data_wrapper_mockable = real_ast_destroy_typed_data_wrapper;
void real_ast_destroy_typed_data_wrapper(ast *ast_data_wrapper) {
    if ((!ast_data_wrapper) ||(ast_data_wrapper->type != AST_TYPE_DATA_WRAPPER))
        return;

    typed_data *data = ast_data_wrapper->data;
    switch (data->type) {
        case TYPE_INT:
            ast_destroy_typed_data_int(data);
            break;
        case TYPE_STRING:
            ast_destroy_typed_data_string(data);
            break;
        case TYPE_SYMBOL:
            ast_destroy_typed_data_symbol(data);
    }
    AST_FREE(ast_data_wrapper);
}
void set_ast_destroy_typed_data_wrapper(ast_destroy_typed_data_wrapper_fn f) {
    ast_destroy_typed_data_wrapper_mockable = f ? f : real_ast_destroy_typed_data_wrapper;
}

ast_destroy_non_typed_data_wrapper_fn ast_destroy_non_typed_data_wrapper_mockable = real_ast_destroy_non_typed_data_wrapper;
void real_ast_destroy_non_typed_data_wrapper(ast *non_typed_data_wrapper) {
    // placeholder to be able to test ast_destroy_ast_children
}
void set_ast_destroy_non_typed_data_wrapper(ast_destroy_non_typed_data_wrapper_fn f) {
    ast_destroy_non_typed_data_wrapper_mockable = f ? f : real_ast_destroy_non_typed_data_wrapper;
}

ast_create_ast_children_arr_fn ast_create_ast_children_arr_mockable = real_ast_create_ast_children_arr;
ast_children_t *real_ast_create_ast_children_arr(size_t children_nb, ast **children) {
    if (children_nb == 0)
        return NULL;

    ast_children_t *ret = AST_MALLOC(sizeof(ast_children_t));
    if (!ret)
        return NULL;

    ret->children_nb = children_nb;
    ret->children = children;

    return ret;
}
void set_ast_create_ast_children_arr(ast_create_ast_children_arr_fn f) {
    ast_create_ast_children_arr_mockable = f ? f : real_ast_create_ast_children_arr;
}

#endif
