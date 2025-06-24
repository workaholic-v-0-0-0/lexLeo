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

#endif
