// src/interpreter/src/interpreter.c

#include "interpreter.h"
#include "runtime_env.h"
#include "ast.h"

// client code owns out (but not deeply!)
// precondition: all is NULL or well-formed
interpreter_status interpreter_eval(
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out ) {
    if (!env || !root || !out
            || root->type < 0 || root->type >= AST_TYPE_NB_TYPES )
        return INTERPRETER_STATUS_ERROR;

    runtime_env_value *value = NULL;
    interpreter_status status = INTERPRETER_STATUS_OK;

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
            value = runtime_env_make_symbol(root->data->data.symbol_value);
            if (!value)
                return INTERPRETER_STATUS_OOM;
            *out = value;
            break;

        default:
            // should never append due precondition
            return INTERPRETER_STATUS_OOM;
        }
    break;


    case AST_TYPE_ERROR:
        value = runtime_env_make_error(root->error->code, root->error->message);
        if (!value)
            return INTERPRETER_STATUS_OOM;
        *out = value;
        break;

    case AST_TYPE_FUNCTION:
        value = runtime_env_make_function(root, env);
        if (!value)
            return INTERPRETER_STATUS_OOM;
        *out = value;
        break;

    case AST_TYPE_FUNCTION_DEFINITION:
        ast *function_node = root->children->children[0];
        runtime_env_value *evaluated_fn_value = NULL;
        status =
            interpreter_eval(env, function_node, &evaluated_fn_value);
        if (status != INTERPRETER_STATUS_OK)
            return status;
        bool binding = runtime_env_set_local(
            env,
            function_node->children->children[0]->data->data.symbol_value,
            evaluated_fn_value );
        if (!binding) {
            runtime_env_value_destroy(evaluated_fn_value);
            return INTERPRETER_STATUS_BINDING_ERROR;
        }
        *out = evaluated_fn_value;
        break;

    case AST_TYPE_NEGATION:
        if (
                !root->children
                || root->children->children_nb != 1
                || !root->children->children
                || !root->children->children[0] )
            return INTERPRETER_STATUS_INVALID_AST;

        ast *child = root->children->children[0];
        runtime_env_value *evaluated_child_value = NULL;
        status = interpreter_eval(env, child, &evaluated_child_value);

        if (status != INTERPRETER_STATUS_OK)
            return status;

        if (!evaluated_child_value || evaluated_child_value->type != RUNTIME_VALUE_NUMBER) {
            runtime_env_value_destroy(evaluated_child_value);
            return INTERPRETER_STATUS_TYPE_ERROR;
        }

        evaluated_child_value->as.i = - evaluated_child_value->as.i;
        *out = evaluated_child_value;
        break;

    case AST_TYPE_ADDITION:
        if (
                   !root->children
                || root->children->children_nb != 2
                || !root->children->children
                || !root->children->children[0]
                || !root->children->children[1] )
            return INTERPRETER_STATUS_INVALID_AST;

        ast *lhs = root->children->children[0];
        ast *rhs = root->children->children[1];

        runtime_env_value *evaluated_lhs = NULL;
        runtime_env_value *evaluated_rhs = NULL;

        status = interpreter_eval(env, lhs, &evaluated_lhs);
        if (status != INTERPRETER_STATUS_OK) {
            runtime_env_value_destroy(evaluated_lhs);
            return status;
        }
        status = interpreter_eval(env, rhs, &evaluated_rhs);
        if (status != INTERPRETER_STATUS_OK) {
            runtime_env_value_destroy(evaluated_lhs);
            runtime_env_value_destroy(evaluated_rhs);
            return status;
        }

        if (
                   evaluated_lhs->type != RUNTIME_VALUE_NUMBER
                || evaluated_rhs->type != RUNTIME_VALUE_NUMBER ) {
            runtime_env_value_destroy(evaluated_lhs);
            runtime_env_value_destroy(evaluated_rhs);
            return INTERPRETER_STATUS_TYPE_ERROR;
        }

        evaluated_lhs->as.i = evaluated_lhs->as.i + evaluated_rhs->as.i;
        *out = evaluated_lhs;

        runtime_env_value_destroy(evaluated_rhs);
        break;

    default:
        return INTERPRETER_STATUS_INVALID_AST;
    }

    return INTERPRETER_STATUS_OK;
}
