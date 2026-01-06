// src/ports/lexer/include/lexer_ops.h

#ifndef LEXLEO_LEXER_OPS_H
#define LEXLEO_LEXER_OPS_H

#include "lexer_types.h"

struct lexleo_token_t;

typedef struct lexer_ops_t {
	lexer_next_rc_t (*next)(lexer_t *lx, struct lexleo_token_t *out);
} lexer_ops_t;

const lexer_ops_t *lexer_default_ops(void);

#endif //LEXLEO_LEXER_OPS_H