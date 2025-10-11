// src/interpreter/src/interpreter.c

#include "interpreter.h"
#include "runtime_env.h"
#include "ast.h"

//#include "symtab.h"




// client code owns out (but not deeply!)
interpreter_status interpreter_eval(
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out ) {
    if (!env || !root || !out
            || root->type < 0 || root->type >= AST_TYPE_NB_TYPES )
        return INTERPRETER_STATUS_ERROR;

    runtime_env_value *value = NULL;

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

    default:
        //
    }

    return INTERPRETER_STATUS_OK;
}
