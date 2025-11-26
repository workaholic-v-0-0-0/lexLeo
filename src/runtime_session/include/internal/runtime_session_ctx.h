// src/runtime_session/include/internal/runtime_session_ctx.h

#ifndef LEXLEO_RUNTIME_SESSION_CTX_H
#define LEXLEO_RUNTIME_SESSION_CTX_H

#include "lexer.h"

typedef int (*lexer_init_fn_t)(yyscan_t *scanner);
typedef int (*lexer_destroy_fn_t)(yyscan_t scanner);
typedef struct lexer_ops_t {
	lexer_init_fn_t lexer_init_fn;
	lexer_destroy_fn_t lexer_destroy_fn;
} lexer_ops_t;

typedef struct runtime_session_ctx {
	const lexer_ops_t *lexer_ops;
} runtime_session_ctx;



// setters and getters

void runtime_session_set_lexer_ops(const lexer_ops_t *overrides);
void runtime_session_reset_lexer_ops(void);
void runtime_session_set_lexer_init_fn(lexer_init_fn_t lexer_init_fn);
void runtime_session_set_lexer_destroy_fn(lexer_destroy_fn_t lexer_destroy_fn);

lexer_init_fn_t runtime_session_get_lexer_init_fn(void);
lexer_destroy_fn_t runtime_session_get_lexer_destroy_fn(void);

#endif //LEXLEO_RUNTIME_SESSION_CTX_H
