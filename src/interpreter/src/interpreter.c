// src/interpreter/src/interpreter.c

#include "internal/interpreter_internal.h"

#include "runtime_env.h"
#include "ast.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h> //todebug

typedef bool (*ast_is_well_formed_fn_t)(const ast *node);

static bool ast_is_well_formed_one_child_node(
        const ast *node,
        ast_type type,
        ast_is_well_formed_fn_t child_is_well_formed_fn ) {
    return
        (  node
        && node->type == type
        && node->children
        && node->children->children
        && node->children->children_nb == 1
        && node->children->children[0]
        && child_is_well_formed_fn
        && child_is_well_formed_fn(node->children->children[0]) );
}

static bool ast_is_well_formed_two_children_node(
        const ast *node,
        ast_type type,
        ast_is_well_formed_fn_t first_child_is_well_formed_fn,
        ast_is_well_formed_fn_t second_child_is_well_formed_fn ) {
    return
        (  node
        && node->type == type
        && node->children
        && node->children->children
        && node->children->children_nb == 2
        && node->children->children[0]
        && first_child_is_well_formed_fn
        && first_child_is_well_formed_fn(node->children->children[0])
        && node->children->children[1]
        && second_child_is_well_formed_fn
        && second_child_is_well_formed_fn(node->children->children[1]) );
}

static bool ast_is_well_formed_uniform_list(
        const ast *node,
        ast_type type,
        ast_is_well_formed_fn_t child_is_well_formed_fn ) {
    if (
               !node
            || node->type != type
            || !node->children
            || !child_is_well_formed_fn )
        return false;
    size_t children_nb = node->children->children_nb;
    ast **children = node->children->children;
    if (children_nb == 0)
        return true;
    if (!children)
        return false;
    for (size_t i = 0; i < children_nb; i++)
        if (!children[i] || !child_is_well_formed_fn(children[i]))
            return false;
    return true;
}

static bool ast_is_well_formed_int_node(const ast *node) {
    return
        (  node
        && node->type == AST_TYPE_DATA_WRAPPER
        && node->data
        && node->data->type == TYPE_INT );
}

static bool ast_is_well_formed_string_node(const ast *node) {
    return
        (  node
        && node->type == AST_TYPE_DATA_WRAPPER
        && node->data
        && node->data->type == TYPE_STRING
        && node->data->data.string_value );
}

static bool ast_is_well_formed_symbol_node(const ast *node) {
    return
        (  node
        && node->type == AST_TYPE_DATA_WRAPPER
        && node->data
        && node->data->type == TYPE_SYMBOL
        && node->data->data.symbol_value );
}

static bool ast_is_well_formed_atom(const ast *node) {
    return
        (  ast_is_well_formed_int_node(node)
        || ast_is_well_formed_string_node(node)
        || ast_is_well_formed_symbol_node(node) );
}

static bool ast_is_well_formed_eval(const ast *node) {
    return
        ast_is_well_formed_one_child_node(
            node,
            AST_TYPE_EVAL,
            ast_is_well_formed_symbol_node );
}

// forward declaration
static bool ast_is_well_formed_computation(const ast *node);

static bool ast_is_well_formed_negation(const ast *node) {
    return
        ast_is_well_formed_one_child_node(
            node,
            AST_TYPE_NEGATION,
            ast_is_well_formed_computation );
}

static bool ast_is_well_formed_binary_computation(const ast *node, ast_type type) {
    return
        ast_is_well_formed_two_children_node(
            node,
            type,
            ast_is_well_formed_computation,
            ast_is_well_formed_computation
        );
}

static bool ast_is_well_formed_addition(const ast *node) {
    return ast_is_well_formed_binary_computation(node, AST_TYPE_ADDITION);
}

static bool ast_is_well_formed_subtraction(const ast *node) {
    return ast_is_well_formed_binary_computation(node, AST_TYPE_SUBTRACTION);
}

static bool ast_is_well_formed_multiplication(const ast *node) {
    return ast_is_well_formed_binary_computation(node, AST_TYPE_MULTIPLICATION);
}

static bool ast_is_well_formed_division(const ast *node) {
    return ast_is_well_formed_binary_computation(node, AST_TYPE_DIVISION);
}

static bool ast_is_well_formed_computation(const ast *node) {
    return
        (  ast_is_well_formed_negation(node)
        || ast_is_well_formed_addition(node)
        || ast_is_well_formed_subtraction(node)
        || ast_is_well_formed_multiplication(node)
        || ast_is_well_formed_division(node)
        || ast_is_well_formed_int_node(node)
        || ast_is_well_formed_symbol_node(node) );
}

static bool ast_is_well_formed_numbers(const ast *node) {
    return
        ast_is_well_formed_uniform_list(
            node,
            AST_TYPE_NUMBERS,
            ast_is_well_formed_int_node );
}

static bool ast_is_well_formed_parameters(const ast *node) {
    return
        ast_is_well_formed_uniform_list(
            node,
            AST_TYPE_PARAMETERS,
            ast_is_well_formed_symbol_node );
}

static bool ast_is_well_formed_list_of_numbers(const ast *node) {
    return
        ast_is_well_formed_one_child_node(
            node,
            AST_TYPE_LIST_OF_NUMBERS,
            ast_is_well_formed_numbers );
}

static bool ast_is_well_formed_list_of_parameters(const ast *node) {
    return
        ast_is_well_formed_one_child_node(
            node,
            AST_TYPE_LIST_OF_PARAMETERS,
            ast_is_well_formed_parameters );
}

static bool ast_is_well_formed_function_call(const ast *node) {
    return
        ast_is_well_formed_two_children_node(
            node,
            AST_TYPE_FUNCTION_CALL,
            ast_is_well_formed_symbol_node,
            ast_is_well_formed_list_of_numbers
        );
}

// forward declaration
static bool ast_is_well_formed_quote(const ast *node);

static bool ast_is_well_formed_evaluable(const ast *node) {
    return
        (  ast_is_well_formed_function_call(node)
        || ast_is_well_formed_atom(node)
        || ast_is_well_formed_computation(node)
        || ast_is_well_formed_quote(node) );
}

static bool ast_is_well_formed_writing(const ast *node) {
    return
        ast_is_well_formed_one_child_node(
            node,
            AST_TYPE_WRITING,
            ast_is_well_formed_evaluable );
}

static bool ast_is_well_formed_quote(const ast *node) {
    return
        ast_is_well_formed_one_child_node(
            node,
            AST_TYPE_QUOTE,
            ast_is_well_formed_evaluable );
}

static bool ast_is_well_formed_binding(const ast *node) {
    return
        ast_is_well_formed_two_children_node(
            node,
            AST_TYPE_BINDING,
            ast_is_well_formed_symbol_node,
            ast_is_well_formed_evaluable
        );
}

static bool ast_is_well_formed_reading(const ast *node) {
    return
        ast_is_well_formed_one_child_node(
            node,
            AST_TYPE_READING,
            ast_is_well_formed_symbol_node );
}

static bool ast_is_well_formed_function_definition(const ast *node);

static bool ast_is_well_formed_statement(const ast *node) {
    return
        (  ast_is_well_formed_binding(node)
        || ast_is_well_formed_writing(node)
        || ast_is_well_formed_reading(node)
        || ast_is_well_formed_function_definition(node)
        || ast_is_well_formed_function_call(node) );
}

static bool ast_is_well_formed_block_items(const ast *node) {
    return
        ast_is_well_formed_uniform_list(
            node,
            AST_TYPE_BLOCK_ITEMS,
            ast_is_well_formed_statement );
}

static bool ast_is_well_formed_block(const ast *node) {
    return
        ast_is_well_formed_one_child_node(
            node,
            AST_TYPE_BLOCK,
            ast_is_well_formed_block_items );
}

static bool ast_is_well_formed_function(const ast *node) {
    return
        (  node
        && node->type == AST_TYPE_FUNCTION
        && node->children
        && node->children->children
        && node->children->children_nb == 3
        && ast_is_well_formed_symbol_node(node->children->children[0])
        && ast_is_well_formed_list_of_parameters(node->children->children[1])
        && ast_is_well_formed_block(node->children->children[2]) );
}

static bool ast_is_well_formed_function_definition(const ast *node) {
    return
        (  node
        && node->type == AST_TYPE_FUNCTION_DEFINITION
        && node->children
        && node->children->children
        && node->children->children_nb == 1
        && ast_is_well_formed_function(node->children->children[0]) );
}

static bool ast_is_well_formed_translation_unit(const ast *node) {
    return
        ast_is_well_formed_uniform_list(
            node,
            AST_TYPE_TRANSLATION_UNIT,
            ast_is_well_formed_statement );
}

// precondition: ast is a well-formed ast of type AST_TYPE_LIST_OF_PARAMETERS
// it must be call after a call of ast_is_well_formed_list_of_parameters which returned true
static bool params_are_unique(const ast *list_of_params) {
	const ast_children_t *params_info = list_of_params->children->children[0]->children;
	ast **params = params_info->children;
	const size_t params_nb = params_info->children_nb;
	for (size_t i = 0; i < params_nb; i++) {
		const symbol *symbol_i = params[i]->data->data.symbol_value;
		for (size_t j = 0; j < i; j++) {
			const symbol *symbol_j = params[j]->data->data.symbol_value;
			if (symbol_j == symbol_i)
				return false;
		}
	}
	return true;
}

// to debug begin
typedef struct symbol {
    char *name; // owned ; must be not NULL and not exceeding MAXIMUM_SYMBOL_NAME_LENGTH characters
} symbol;
// to debug end

// Ownership & contract
// - On success (return == INTERPRETER_STATUS_OK):
//   *out receives an OWNED reference to the returned value
//   (refcount == 1, unless otherwise documented).
//   The caller MUST call runtime_env_value_release(*out) when done.
// - On error (return != INTERPRETER_STATUS_OK):
//   *out is LEFT UNCHANGED (the caller must not release *out).
//
// Specific notes:
// - TYPE_SYMBOL: runtime_env_get() returns a BORROWED pointer.
//   interpreter_eval() calls runtime_env_value_retain() before storing it in *out.
// - TYPE_FUNCTION: the value is created via runtime_env_make_function() (refcount == 1).
//   No extra retain is performed. The closure (env) is retained inside make_function().
//
// Preconditions (ensured by the caller or validated by this function):
// - env, root, and out are non-NULL
// - root->type ∈ [0 .. AST_TYPE_NB_TYPES)
// - The required substructures for the given node kind are well-formed;
//   otherwise, an appropriate error status is returned.
interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
        struct runtime_env *env,
        //struct runtime_session *session,
        const struct ast *root,
        const struct runtime_env_value **out ) {

    if (
               !env
            || !root
            || !out
            || root->type < 0
            || root->type >= AST_TYPE_NB_TYPES )
        return INTERPRETER_STATUS_ERROR;

    const runtime_env_value *value = NULL;
    interpreter_status status = INTERPRETER_STATUS_OK;

    ast *lhs = NULL;
    ast *rhs = NULL;
    ast *child = NULL;
    const runtime_env_value *evaluated_lhs = NULL;
    const runtime_env_value *evaluated_rhs = NULL;
    const runtime_env_value *evaluated_child_value = NULL;
    const runtime_env_value *evaluated_quoted_ast = NULL;
    bool binding = false;
    bool write_runtime_value_ret = false;
    interpreter_status rhs_eval_status;

    switch (root->type) {

    case AST_TYPE_DATA_WRAPPER:

        switch (root->data->type) {

        case TYPE_INT:
            value = runtime_env_make_number(root->data->data.int_value);
            if (!value)
                return INTERPRETER_STATUS_OOM;

            *out = value;
            break;

        case TYPE_STRING:
            value = runtime_env_make_string(root->data->data.string_value);
            if (!value)
                return INTERPRETER_STATUS_OOM;

            *out = value;
            break;

        case TYPE_SYMBOL:
            value =
                runtime_env_get(
                    env,
                    root->data->data.symbol_value );

            if (!value)
                return INTERPRETER_STATUS_LOOKUP_FAILED;

            runtime_env_value_retain(value);
            *out = value;
            break;

        default:
            // should never happen due precondition
            return INTERPRETER_STATUS_INVALID_AST;
        }
    break;

    case AST_TYPE_ERROR:
        value = runtime_env_make_error(root->error->code, root->error->message);
        if (!value)
            return INTERPRETER_STATUS_OOM;
        *out = value;
        break;

    case AST_TYPE_FUNCTION:
        if (!ast_is_well_formed_function(root))
            return INTERPRETER_STATUS_INVALID_AST;

		if (!params_are_unique(root->children->children[1]))
			return INTERPRETER_STATUS_DUPLICATE_PARAMETER; // should be better in resolver

        value = runtime_env_make_function(root, env);

        if (!value)
            return INTERPRETER_STATUS_OOM;

        *out = value;
        break;

    case AST_TYPE_FUNCTION_DEFINITION:
        if (!ast_is_well_formed_function_definition(root))
            return INTERPRETER_STATUS_INVALID_AST;

        ast *function_node = root->children->children[0];
        const runtime_env_value *evaluated_fn_value = NULL;
        status =
            interpreter_eval(ctx, env, function_node, &evaluated_fn_value);
        if (status != INTERPRETER_STATUS_OK)
            return status;
        binding =
            runtime_env_set_local(
                env,
                function_node->children->children[0]->data->data.symbol_value,
                evaluated_fn_value );
        if (!binding) {
            runtime_env_value_release(evaluated_fn_value);
            return INTERPRETER_STATUS_BINDING_ERROR;
        }

        *out = evaluated_fn_value;
        break;

    case AST_TYPE_NEGATION:
        if (!ast_is_well_formed_negation(root))
            return INTERPRETER_STATUS_INVALID_AST;

        child = root->children->children[0];
        evaluated_child_value = NULL;
        status = interpreter_eval(ctx, env, child, &evaluated_child_value);

        if (status != INTERPRETER_STATUS_OK)
            return status;

        if (evaluated_child_value->type != RUNTIME_VALUE_NUMBER) {
            runtime_env_value_release(evaluated_child_value);
            return INTERPRETER_STATUS_TYPE_ERROR;
        }

        value = runtime_env_make_number(- evaluated_child_value->as.i);
        runtime_env_value_release(evaluated_child_value);
        if (!value)
            return INTERPRETER_STATUS_OOM;

        *out = value;
        break;

    case AST_TYPE_ADDITION:
        if (!ast_is_well_formed_addition(root))
            return INTERPRETER_STATUS_INVALID_AST;

        lhs = root->children->children[0];
        rhs = root->children->children[1];

        evaluated_lhs = NULL;
        evaluated_rhs = NULL;

        status = interpreter_eval(ctx, env, lhs, &evaluated_lhs);
        if (status != INTERPRETER_STATUS_OK)
            return status;

        if (evaluated_lhs->type != RUNTIME_VALUE_NUMBER)
            return INTERPRETER_STATUS_TYPE_ERROR;

        status = interpreter_eval(ctx, env, rhs, &evaluated_rhs);
        if (status != INTERPRETER_STATUS_OK) {
            runtime_env_value_release(evaluated_lhs);
            return status;
        }

        if (evaluated_rhs->type != RUNTIME_VALUE_NUMBER) {
            runtime_env_value_release(evaluated_rhs);
            runtime_env_value_release(evaluated_lhs);
            return INTERPRETER_STATUS_TYPE_ERROR;
        }

        value =
            runtime_env_make_number(
                evaluated_lhs->as.i
                +
                evaluated_rhs->as.i );

        runtime_env_value_release(evaluated_lhs);
        runtime_env_value_release(evaluated_rhs);
        *out = value;
        break;

    case AST_TYPE_SUBTRACTION:
        if (!ast_is_well_formed_subtraction(root))
            return INTERPRETER_STATUS_INVALID_AST;

        lhs = root->children->children[0];
        rhs = root->children->children[1];

        evaluated_lhs = NULL;
        evaluated_rhs = NULL;

        status = interpreter_eval(ctx, env, lhs, &evaluated_lhs);
        if (status != INTERPRETER_STATUS_OK)
            return status;

        if (evaluated_lhs->type != RUNTIME_VALUE_NUMBER)
            return INTERPRETER_STATUS_TYPE_ERROR;

        status = interpreter_eval(ctx, env, rhs, &evaluated_rhs);
        if (status != INTERPRETER_STATUS_OK) {
            runtime_env_value_release(evaluated_lhs);
            return status;
        }

        if (evaluated_rhs->type != RUNTIME_VALUE_NUMBER) {
            runtime_env_value_release(evaluated_rhs);
            runtime_env_value_release(evaluated_lhs);
            return INTERPRETER_STATUS_TYPE_ERROR;
        }

        value =
            runtime_env_make_number(
                evaluated_lhs->as.i
                -
                evaluated_rhs->as.i );

        runtime_env_value_release(evaluated_lhs);
        runtime_env_value_release(evaluated_rhs);
        *out = value;
        break;

    case AST_TYPE_MULTIPLICATION:
        if (!ast_is_well_formed_multiplication(root))
            return INTERPRETER_STATUS_INVALID_AST;

        lhs = root->children->children[0];
        rhs = root->children->children[1];

        evaluated_lhs = NULL;
        evaluated_rhs = NULL;

        status = interpreter_eval(ctx, env, lhs, &evaluated_lhs);
        if (status != INTERPRETER_STATUS_OK)
            return status;

        if (evaluated_lhs->type != RUNTIME_VALUE_NUMBER)
            return INTERPRETER_STATUS_TYPE_ERROR;

        status = interpreter_eval(ctx, env, rhs, &evaluated_rhs);
        if (status != INTERPRETER_STATUS_OK) {
            runtime_env_value_release(evaluated_lhs);
            return status;
        }

        if (evaluated_rhs->type != RUNTIME_VALUE_NUMBER) {
            runtime_env_value_release(evaluated_rhs);
            runtime_env_value_release(evaluated_lhs);
            return INTERPRETER_STATUS_TYPE_ERROR;
        }

        value =
            runtime_env_make_number(
                evaluated_lhs->as.i
                *
                evaluated_rhs->as.i );

        runtime_env_value_release(evaluated_lhs);
        runtime_env_value_release(evaluated_rhs);
        *out = value;
        break;

    case AST_TYPE_DIVISION:
        if (!ast_is_well_formed_division(root))
            return INTERPRETER_STATUS_INVALID_AST;

        lhs = root->children->children[0];
        rhs = root->children->children[1];

        evaluated_lhs = NULL;
        evaluated_rhs = NULL;

        status = interpreter_eval(ctx, env, lhs, &evaluated_lhs);
        if (status != INTERPRETER_STATUS_OK)
            return status;

        if (evaluated_lhs->type != RUNTIME_VALUE_NUMBER)
            return INTERPRETER_STATUS_TYPE_ERROR;

        status = interpreter_eval(ctx, env, rhs, &evaluated_rhs);
        if (status != INTERPRETER_STATUS_OK) {
            runtime_env_value_release(evaluated_lhs);
            return status;
        }

        if (evaluated_rhs->type != RUNTIME_VALUE_NUMBER) {
            runtime_env_value_release(evaluated_lhs);
            runtime_env_value_release(evaluated_rhs);
            return INTERPRETER_STATUS_TYPE_ERROR;
        }

        if (evaluated_rhs->as.i == 0) {
            runtime_env_value_release(evaluated_lhs);
            runtime_env_value_release(evaluated_rhs);
            return INTERPRETER_STATUS_DIVISION_BY_ZERO;
        }

        value =
            runtime_env_make_number(
                evaluated_lhs->as.i
                /
                evaluated_rhs->as.i );

        runtime_env_value_release(evaluated_lhs);
        runtime_env_value_release(evaluated_rhs);
        *out = value;
        break;

    case AST_TYPE_QUOTE:
        if (!ast_is_well_formed_quote(root))
            return INTERPRETER_STATUS_INVALID_AST;

        value = runtime_env_make_quoted(root->children->children[0]);
        if (!value)
            return INTERPRETER_STATUS_OOM;

        *out = value;
        break;

    case AST_TYPE_READING:
        if (!ast_is_well_formed_reading(root))
            return INTERPRETER_STATUS_INVALID_AST;

        if (!ctx || !ctx->ops || !ctx->ops->read_ast_fn)
            return INTERPRETER_STATUS_INTERNAL_ERROR;

        rhs = ctx->ops->read_ast_fn(ctx);
        if (!rhs)
            return INTERPRETER_STATUS_READ_AST_ERROR;

        evaluated_rhs = NULL;
        rhs_eval_status = interpreter_eval(ctx, env, rhs, &evaluated_rhs);
        if (rhs_eval_status != INTERPRETER_STATUS_OK)
            return rhs_eval_status;

        binding =
            runtime_env_set_local(
                env,
                root->children->children[0]->data->data.symbol_value,
                evaluated_rhs );

        if (!binding) {
            runtime_env_value_release(evaluated_rhs);
            return INTERPRETER_STATUS_BINDING_ERROR;
        }

        *out = evaluated_rhs;

        break;

    case AST_TYPE_BINDING:
        if (!ast_is_well_formed_binding(root))
            return INTERPRETER_STATUS_INVALID_AST;

        rhs = root->children->children[1];
        evaluated_rhs = NULL;
        status = interpreter_eval(ctx, env, rhs, &evaluated_rhs);
        if (status != INTERPRETER_STATUS_OK)
            return status;

        binding =
            runtime_env_set_local(
                env,
                root->children->children[0]->data->data.symbol_value,
                evaluated_rhs );
        if (!binding) {
            runtime_env_value_release(evaluated_rhs);
            return INTERPRETER_STATUS_BINDING_ERROR;
        }

        *out = evaluated_rhs;
        break;

    case AST_TYPE_EVAL:
        if (!ast_is_well_formed_eval(root))
            return INTERPRETER_STATUS_INVALID_AST;

        child = root->children->children[0];
        evaluated_child_value = NULL;
        status = interpreter_eval(ctx, env, child, &evaluated_child_value);

        if (status != INTERPRETER_STATUS_OK)
            return status;

        if (evaluated_child_value->type != RUNTIME_VALUE_QUOTED) {
            *out = evaluated_child_value;
        }
        else {
            status =
                interpreter_eval(
                    ctx,
                    env,
                    evaluated_child_value->as.quoted,
                    &evaluated_quoted_ast );
            runtime_env_value_release(evaluated_child_value);
            if (status != INTERPRETER_STATUS_OK) {
                return status;
            }
            *out = evaluated_quoted_ast;
        }

        break;

    case AST_TYPE_WRITING:
        if (!ast_is_well_formed_writing(root))
            return INTERPRETER_STATUS_INVALID_AST;

        if (!ctx || !ctx->ops || !ctx->ops->write_runtime_value_fn)
            return INTERPRETER_STATUS_INTERNAL_ERROR;

        evaluated_child_value = NULL;
        status = interpreter_eval(ctx, env, root->children->children[0], &evaluated_child_value);
        if (status != INTERPRETER_STATUS_OK)
            return status;

        write_runtime_value_ret = ctx->ops->write_runtime_value_fn(ctx, evaluated_child_value);
        runtime_env_value_release(evaluated_child_value);
        return
            (write_runtime_value_ret) ?
                INTERPRETER_STATUS_OK
                :
                INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR;


// <here>


    default:
        return INTERPRETER_STATUS_UNSUPPORTED_AST;
    }

    return INTERPRETER_STATUS_OK;
}

void interpreter_ctx_init(
        interpreter_ctx *ctx,
        const interpreter_ops_t *ops,
        void *host_ctx ) {
    ctx->ops = ops;
    ctx->host_ctx = host_ctx;
}