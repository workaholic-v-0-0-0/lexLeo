// src/ast/src/ast.c

#include "ast.h"

#include "internal/ast_memory_allocator.h"

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
