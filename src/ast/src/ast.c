// src/ast/src/ast.c

#include "ast.h"

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
    AST_FREE(typed_data_int);
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
    AST_FREE(typed_data_string->data.string_value);
    AST_FREE(typed_data_string);
}
