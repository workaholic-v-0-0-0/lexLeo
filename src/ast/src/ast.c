// src/ast/src/ast.c

#include "internal/ast_test_utils.h"

#include "internal/ast_memory_allocator.h"
#include "internal/ast_string_utils.h"

#include <stdarg.h>
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
#ifdef UNIT_TEST
    ast_destroy_typed_data_wrapper_mockable(ast_data_wrapper);
#else
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
#endif
}

ast_children_t *ast_create_ast_children_arr(size_t children_nb, ast **children) {
#ifdef UNIT_TEST
    return ast_create_ast_children_arr_mockable(children_nb, children);
#else
    ast_children_t *ret = AST_MALLOC(sizeof(ast_children_t));
    if (!ret)
        return NULL;

    if (children_nb == 0) {
        ret->children_nb = 0;
        ret->children = NULL;
    } else {

        ret->children_nb = children_nb;
        ret->children = children;
    }
    return ret;
#endif
}

ast_children_t *ast_create_ast_children_var(size_t children_nb,...) {
    ast_children_t *ret = AST_MALLOC(sizeof(ast_children_t));
    if (!ret)
        return NULL;

    if (children_nb == 0) {
        ret->children_nb = 0;
        ret->children = NULL;

    } else {
        ast **children = AST_MALLOC(children_nb * sizeof(ast *));
            if (!children) {
            AST_FREE(ret);
            return NULL;
        }

        va_list args;
        va_start(args, children_nb);
        for (size_t i = 0; i < children_nb; i++) {
            children[i] = va_arg(args, ast *);
        }
        va_end(args);

        ret->children_nb = children_nb;
        ret->children = children;
    }

    return ret;
}

void ast_destroy_ast_children(ast_children_t *ast_children) {
#ifdef UNIT_TEST
    ast_destroy_ast_children_mockable(ast_children);
#else
    if ((!ast_children) || (ast_children->children_nb == 0))
        return;

    for (size_t i = 0; i < ast_children->children_nb; i++) {
        if (ast_children->children[i]->type != AST_TYPE_DATA_WRAPPER)
            ast_destroy_non_typed_data_wrapper(ast_children->children[i]);
        else
            ast_destroy_typed_data_wrapper(ast_children->children[i]);
    }
    AST_FREE(ast_children->children);
    AST_FREE(ast_children);
#endif
}

ast *ast_create_non_typed_data_wrapper(ast_type type, ast_children_t *ast_children) {
    if ((type == AST_TYPE_DATA_WRAPPER) || (type < 0) || (type >= AST_TYPE_NB_TYPES) || (!ast_children))
        return NULL;

    ast *ret = AST_MALLOC(sizeof(ast));
    if (!ret)
        return NULL;

    ret->type = type;
    ret->children = ast_children;

    return ret;
}

ast *ast_create_non_typed_data_wrapper_arr(ast_type type, size_t children_nb, ast **children) {
    if ((type == AST_TYPE_DATA_WRAPPER) || (type < 0) || (type >= AST_TYPE_NB_TYPES))
        return NULL;

    ast_children_t * ast_children = ast_create_ast_children_arr(children_nb, children);
    if (!ast_children)
        return NULL;

    ast *ret = AST_MALLOC(sizeof(ast));
    if (!ret) {
        AST_FREE(ast_children);
        return NULL;
    }

    ret->type = type;
    ret->children = ast_children;

    return ret;
}

ast *ast_create_non_typed_data_wrapper_var(ast_type type, size_t children_nb,...) {
    if ((type == AST_TYPE_DATA_WRAPPER) || (type < 0) || (type >= AST_TYPE_NB_TYPES))
        return NULL;

    ast *ret = AST_MALLOC(sizeof(ast));
    if (!ret)
        return NULL;

    if (children_nb == 0) {
        ast_children_t *children_info = ast_create_ast_children_arr(0, NULL);
        if (!children_info) {
            AST_FREE(ret);
            return NULL;
        }

        ret->type = type;
        ret->children = children_info;

    } else {
        ast **ast_p_arr = AST_MALLOC(children_nb * sizeof(ast *));
        if (!ast_p_arr) {
            AST_FREE(ret);
            return NULL;
        }

        va_list args;
        va_start(args, children_nb);
        for (size_t i = 0; i < children_nb; i++) {
            ast_p_arr[i] = va_arg(args, ast *);
        }
        va_end(args);

        ast_children_t *children_info = ast_create_ast_children_arr(children_nb, ast_p_arr);
        if (!children_info) {
            AST_FREE(ast_p_arr);
            AST_FREE(ret);
            return NULL;
        }

        ret->type = type;
        ret->children = children_info;
        AST_FREE(ast_p_arr);
    }
    return ret;
}

void ast_destroy_non_typed_data_wrapper(ast *non_typed_data_wrapper) {
#ifdef UNIT_TEST
    ast_destroy_non_typed_data_wrapper_mockable(non_typed_data_wrapper);
#else
    if ((non_typed_data_wrapper) && (non_typed_data_wrapper->type != AST_TYPE_DATA_WRAPPER)) {
        ast_destroy_ast_children(non_typed_data_wrapper->children);
        AST_FREE(non_typed_data_wrapper);
    }
#endif
}

void ast_destroy(ast *root) {
#ifdef UNIT_TEST
    ast_destroy_mockable(root);
#else
    if (!root)
        return;

    if (root->type == AST_TYPE_DATA_WRAPPER)
        ast_destroy_typed_data_wrapper(root);
    else
        ast_destroy_non_typed_data_wrapper(root);
#endif
}
