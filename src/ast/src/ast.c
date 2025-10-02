// src/ast/src/ast.c

#ifdef UNIT_TEST
#include "internal/ast_test_utils.h"
#endif

#include "internal/ast_memory_allocator.h"
#include "internal/ast_string_utils.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static error_info error_sentinel_payload = {
    .code = AST_UNRETRIEVABLE_ERROR_CODE,
    .message = "AST error sentinel: original cause lost due to allocation \
failure while constructing error node",
    .is_sentinel = true
};
static ast error_sentinel = {
    .type = AST_TYPE_ERROR,
    .error = &error_sentinel_payload
};
static ast *const AST_ERROR_SENTINEL = &error_sentinel;

ast *ast_error_sentinel(void) {
    return AST_ERROR_SENTINEL;
}

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

typed_data *ast_create_typed_data_string(const char *s) {
    typed_data *ret = AST_MALLOC(sizeof(typed_data));
    if (!ret)
        return NULL;

    char *string_value = (s) ? AST_STRING_DUPLICATE(s) : NULL;
    if ((!string_value) && (s)) {
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

typed_data *ast_create_typed_data_symbol_name(const char *s) {
	if (!s)
		return NULL;

    typed_data *ret = AST_MALLOC(sizeof(typed_data));
    if (!ret)
        return NULL;

    char *string_value = AST_STRING_DUPLICATE(s);
    if (!string_value) {
        AST_FREE(ret);
        return NULL;
    }

    ret->type = TYPE_SYMBOL_NAME;
    ret->data.string_value = string_value;

    return ret;
}

void ast_destroy_typed_data_symbol_name(typed_data *typed_data_symbol_name) {
	ast_destroy_typed_data_string(typed_data_symbol_name);
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
	// Only frees the typed_data container itself.
	// Does not free the underlying symbol*, which is owned and freed
	// by the global symbol_pool via symtab_cleanup_pool().
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
    if ((!ast_data_wrapper) || (ast_data_wrapper->type != AST_TYPE_DATA_WRAPPER))
        return;

    typed_data *data = ast_data_wrapper->data;
    switch (data->type) {
        case TYPE_INT:
            ast_destroy_typed_data_int(data);
            break;
        case TYPE_STRING:
            ast_destroy_typed_data_string(data);
            break;
        case TYPE_SYMBOL_NAME:
            ast_destroy_typed_data_symbol_name(data);
            break;
        case TYPE_SYMBOL:
            ast_destroy_typed_data_symbol(data);
    }
    AST_FREE(ast_data_wrapper);
#endif
}

ast *ast_create_error_node(ast_error_type code, const char *message) {
	if ((!message) || (strlen(message) > MAXIMUM_ERROR_MESSAGE_LENGTH))
		return NULL;

	ast *ret = AST_MALLOC(sizeof(ast));
	if (!ret)
		return NULL;

	char *error_message = AST_STRING_DUPLICATE(message);

	if (!error_message) {
		AST_FREE(ret);
		return NULL;
	}

	error_info *error = AST_MALLOC(sizeof(error_info));
	if (!error) {
		AST_FREE(error_message);
		AST_FREE(ret);
		return NULL;
	}

	error->code = code;
	error->message = error_message;
	error->is_sentinel = false;

	ret->type = AST_TYPE_ERROR;
	ret->error = error;

	return ret;
}

ast *ast_create_error_node_or_sentinel(ast_error_type code, const char *message) {
	ast *error_node = ast_create_error_node(code, message);
	return (error_node) ? error_node : ast_error_sentinel();
}

void ast_destroy_error_node(ast *ast_error_node) {
    if ((ast_error_node == AST_ERROR_SENTINEL) || (!ast_error_node))
		return;

	AST_FREE(ast_error_node->error->message);
	AST_FREE(ast_error_node->error);
	AST_FREE(ast_error_node);
}

ast_children_t *ast_create_ast_children_arr(size_t children_nb, ast **children) {
#ifdef UNIT_TEST
    return ast_create_ast_children_arr_mockable(children_nb, children);
#else
    ast_children_t *ret = AST_MALLOC(sizeof(ast_children_t));
    if (!ret)
        return NULL;

    ret->children = children;
    ret->children_nb = children_nb;
    ret->capacity = children_nb;
    return ret;
#endif
}

ast_children_t *ast_create_ast_children_var(size_t children_nb,...) {
    ast_children_t *ret = AST_MALLOC(sizeof(ast_children_t));
    if (!ret)
        return NULL;

    if (children_nb > 0) {
        ret->children = AST_MALLOC(children_nb * sizeof(ast *));
        if (!ret->children) {
            AST_FREE(ret);
            return NULL;
        }

        va_list args;
        va_start(args, children_nb);
        for (size_t i = 0; i < children_nb; i++) {
            ret->children[i] = va_arg(args, ast *);
        }
        va_end(args);

    } else {
        ret->children = NULL;
    }

    ret->children_nb = children_nb;
    ret->capacity = children_nb;

    return ret;
}

void ast_destroy_ast_children(ast_children_t *ast_children) {
#ifdef UNIT_TEST
    ast_destroy_ast_children_mockable(ast_children);
#else
    if (!ast_children)
        return;

    for (size_t i = 0; i < ast_children->children_nb; i++) {
        ast_destroy(ast_children->children[i]);
    }
    AST_FREE(ast_children->children);
    AST_FREE(ast_children);
#endif
}

bool ast_children_reserve(ast_children_t *ast_children, size_t capacity) {
    if ((!ast_children) || (capacity < ast_children->children_nb))
        return false;

    if (capacity <= ast_children->capacity)
        return true;

    void *new_address =
        AST_REALLOC(
            ast_children->children,
            capacity * sizeof(ast *) );
    if (!new_address)
        return false;

    ast_children->children = new_address;
    ast_children->capacity = capacity;
    return true;
}

static size_t next_capacity(size_t capacity) {
    return 1 + 2 * capacity;
}

bool ast_children_append_take(ast *parent, ast *child) {
    if (
               (!parent)
            || (!child)
            || (parent->type == AST_TYPE_DATA_WRAPPER)
            || (parent->type == AST_TYPE_ERROR))
        return false;

    ast_children_t *children = parent->children;
    if (children->capacity == children->children_nb) {
        if (!ast_children_reserve(
                parent->children,
                next_capacity(children->capacity) ) )
            return false;
    }

    (children->children)[children->children_nb++] = child;
    return true;
}

ast *ast_create_children_node(ast_type type, ast_children_t *ast_children) {
    if ((type == AST_TYPE_DATA_WRAPPER)
            || (type < 0) || (type >= AST_TYPE_NB_TYPES) || (!ast_children) )
        return NULL;

    ast *ret = AST_MALLOC(sizeof(ast));
    if (!ret)
        return NULL;

    ret->type = type;
    ret->children = ast_children;

    return ret;
}

ast *ast_create_children_node_arr(ast_type type, size_t children_nb, ast **children) {
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

ast *ast_create_children_node_var(ast_type type, size_t children_nb,...) {
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
    }
    return ret;
}

void ast_destroy_children_node(ast *children_node) {
#ifdef UNIT_TEST
    ast_destroy_children_node_mockable(children_node);
#else
    if ((children_node) && (children_node->type != AST_TYPE_DATA_WRAPPER)) {
        ast_destroy_ast_children(children_node->children);
        AST_FREE(children_node);
    }
#endif
}

void ast_destroy(ast *root) {
#ifdef UNIT_TEST
    ast_destroy_mockable(root);
#else
    if (!root)
        return;

	switch (root->type) {
		case AST_TYPE_DATA_WRAPPER:
			ast_destroy_typed_data_wrapper(root);
			break;
		case AST_TYPE_ERROR:
			ast_destroy_error_node(root);
        	break;
		default:
	        ast_destroy_children_node(root);
	}
#endif
}

bool ast_type_has_children(ast_type type) {
    switch (type) {
        case AST_TYPE_BINDING:
        case AST_TYPE_READING:
        case AST_TYPE_WRITING:
        case AST_TYPE_TRANSLATION_UNIT:
        case AST_TYPE_BLOCK_ITEMS:
        case AST_TYPE_BLOCK:
        case AST_TYPE_PARAMETERS:
        case AST_TYPE_LIST_OF_PARAMETERS:
        case AST_TYPE_FUNCTION:
        case AST_TYPE_FUNCTION_DEFINITION:
        case AST_TYPE_NUMBERS:
        case AST_TYPE_LIST_OF_NUMBERS:
        case AST_TYPE_FUNCTION_CALL:
        case AST_TYPE_QUOTE:
        case AST_TYPE_NEGATION:
        case AST_TYPE_COMPUTATION:
        case AST_TYPE_ADDITION:
            return true;
        case AST_TYPE_DATA_WRAPPER:
        case AST_TYPE_ERROR:
            return false;
        default:
            return false;
    }
}

bool ast_can_have_children(ast *a) {
    return (a) && (ast_type_has_children(a->type));
}

bool ast_has_any_child(ast *a) {
    return (ast_can_have_children(a)) && (a->children->children_nb > 0);
}

bool ast_is_data_of(const ast *a, data_type dt) {
    return (a) && (a->type == AST_TYPE_DATA_WRAPPER) && (a->data) && (a->data->type == dt);
}

ast *ast_create_int_node(int i) {
    typed_data *td = ast_create_typed_data_int(i);
    if (!td)
        return NULL;

    ast *ret = ast_create_typed_data_wrapper(td);
	if (!ret) {
        ast_destroy_typed_data_int(td);
		return NULL;
    }

    return ret;
}

ast *ast_create_string_node(const char *str) {
    typed_data *td = ast_create_typed_data_string(str);
    if (!td)
        return NULL;

    ast *ret = ast_create_typed_data_wrapper(td);
	if (!ret) {
        ast_destroy_typed_data_string(td);
		return NULL;
    }

    return ret;
}

ast *ast_create_symbol_name_node(const char *str) {
	if (!str)
		return NULL;

	typed_data *td = ast_create_typed_data_symbol_name(str);
    if (!td)
        return NULL;

    ast *ret = ast_create_typed_data_wrapper(td);
	if (!ret) {
        ast_destroy_typed_data_symbol_name(td);
		return NULL;
    }

    return ret;
}

ast *ast_create_symbol_node(symbol *sym) {
	if (!sym)
		return NULL;

	typed_data *td = ast_create_typed_data_symbol(sym);
    if (!td)
        return NULL;

    ast *ret = ast_create_typed_data_wrapper(td);
	if (!ret) {
        ast_destroy_typed_data_symbol(td);
		return NULL;
    }

	return ret;
}


// DEBUG TOOLS

const char *ast_type_to_string(ast_type t) {
    switch (t) {
        case AST_TYPE_BINDING:             return "BINDING";
        case AST_TYPE_READING:             return "READING";
        case AST_TYPE_WRITING:             return "WRITING";
        case AST_TYPE_TRANSLATION_UNIT:    return "TRANSLATION_UNIT";
        case AST_TYPE_BLOCK_ITEMS:         return "BLOCK_ITEMS";
        case AST_TYPE_BLOCK:               return "BLOCK";
        case AST_TYPE_PARAMETERS:          return "PARAMETERS";
        case AST_TYPE_LIST_OF_PARAMETERS:  return "LIST_OF_PARAMETERS";
        case AST_TYPE_FUNCTION:            return "FUNCTION";
        case AST_TYPE_FUNCTION_DEFINITION: return "FUNCTION_DEFINITION";
        case AST_TYPE_NUMBERS:             return "NUMBERS";
        case AST_TYPE_LIST_OF_NUMBERS:     return "LIST_OF_NUMBERS";
        case AST_TYPE_FUNCTION_CALL:       return "FUNCTION_CALL";
        case AST_TYPE_QUOTE:               return "QUOTE";
        case AST_TYPE_NEGATION:            return "NEGATION";
        case AST_TYPE_COMPUTATION:         return "COMPUTATION";
        case AST_TYPE_ADDITION:            return "ADDITION";
        case AST_TYPE_DATA_WRAPPER:        return "DATA_WRAPPER";
        case AST_TYPE_ERROR:               return "ERROR";
        case AST_TYPE_NB_TYPES:            return "NB_TYPES";
        default:                           return "<UNKNOWN_AST_TYPE>";
    }
}
