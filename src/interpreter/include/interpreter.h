// src/interpreter/include/interpreter.h

#ifndef LEXLEO_INTERPRETER_H
#define LEXLEO_INTERPRETER_H

typedef enum {
    VAL_NUMBER,
    VAL_STRING,
    VAL_SYMBOL, // just the name ???
    // VAL_ERROR, ??? no -> interpreter fails -> VAL_ERROR = code/message
    // VAL_FUNCTION,
    VAL_TYPE_NB_TYPES,
} value_type;

typedef struct value {
    value_type type;
    union {
        int i;
        char *s;
        struct symbol *sym;
        struct {
            int code;
            char *msg;
        } err;
        /*
        struct {
            const struct ast *lambda;
            struct env *closure;
        } fn;
        */
    }
} value;

// env
typedef struct interpreter_ctx {
    symtab *symbol_table;
} interpreter_ctx;

int interpreter_init_exec_env(symtab **symbol_table);//???

/*
typedef struct env env;
env *env_new(env *parent);
void env_destroy(env *e);
bool env_set(env *e, const char *name, value v);
bool env_get(env *e, const char *name, value *out);
*/

int interpreter_eval(interpreter_ctx*, const ast *root, value *out);

#endif //LEXLEO_INTERPRETER_H