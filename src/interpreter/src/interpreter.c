// src/interpreter/src/interpreter.c

#include "interpreter.h"

#include "ast.h"
//#include "symtab.h"





// precondition: root->type == AST_TYPE_DATA_WRAPPER
#ifndef UNIT_TEST
static
#endif
interpreter_status eval_atom(
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out ) {

    return INTERPRETER_STATUS_ERROR;
}
