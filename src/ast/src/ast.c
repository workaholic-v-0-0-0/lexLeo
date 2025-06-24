// src/ast/src/ast.c

#include "internal/ast_test_utils.h"

#include "internal/ast_memory_allocator.h"
#include "internal/ast_string_utils.h"

#include <stdlib.h>

typed_data *ast_create_typed_data_int(int i) {
    typed_data *ret = AST_MALLOC(sizeof(typed_data));

    if (!ret)
        return NULL;

    ret->type = TYPE_INT;
    (ret->data).int_value = i;

    return ret;
}

void ast_destroy_typed_data_int(typed_data *typed_data_int) {
#ifdef UNIT_TEST
    ast_destroy_typed_data_int_mockable(typed_data_int);
#else
    AST_FREE(typed_data_int);
#endif
}

typed_data *ast_create_typed_data_string(char *s) {
    typed_data *ret = AST_MALLOC(sizeof(typed_data));
    if (!ret)
        return NULL;

    char *string_value = AST_STRING_DUPLICATE(s);
    if (!string_value) {
        AST_FREE(ret);
        return NULL;
    }

    ret->type = TYPE_STRING;
    ret->data.string_value = string_value;

    return ret;
}

void ast_destroy_typed_data_string(typed_data *typed_data_string) {
#ifdef UNIT_TEST
    ast_destroy_typed_data_string_mockable(typed_data_string);
#else
    AST_FREE(typed_data_string->data.string_value);
    AST_FREE(typed_data_string);
#endif
}

typed_data *ast_create_typed_data_symbol(symbol *s) {
    typed_data *ret = AST_MALLOC(sizeof(typed_data));

    if (!ret)
        return NULL;

    ret->type = TYPE_SYMBOL;
    (ret->data).symbol_value = s;

    return ret;
}

void ast_destroy_typed_data_symbol(typed_data *typed_data_symbol) {
#ifdef UNIT_TEST
    ast_destroy_typed_data_symbol_mockable(typed_data_symbol);
#else
    AST_FREE(typed_data_symbol);
#endif
}

ast *ast_create_typed_data_wrapper(typed_data *data) {
    ast * ret = AST_MALLOC(sizeof(ast));

    if (!ret)
        return NULL;

    ret->type = AST_TYPE_DATA_WRAPPER;
    ret->data = data;

    return ret;
}

void ast_destroy_typed_data_wrapper(ast *ast_data_wrapper) {
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
