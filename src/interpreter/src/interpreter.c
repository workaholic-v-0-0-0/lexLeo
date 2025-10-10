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

    switch (root->type) {

    case AST_TYPE_DATA_WRAPPER:

        switch (root->data->type) {

        case TYPE_INT:
            runtime_env_value *value = runtime_env_make_number(root->data->data.int_value);
            if (!value)
                return INTERPRETER_STATUS_OOM;
            *out = value;
            break;

        default:
            //
        }

    break;

    default:
        //
    }

    return INTERPRETER_STATUS_OK;
}
