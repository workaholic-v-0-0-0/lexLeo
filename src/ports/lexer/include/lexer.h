// src/ports/lexer/include/lexer.h

// port

#ifndef LEXLEO_LEXER_H
#define LEXLEO_LEXER_H

#include "lexer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct lexleo_token_t;

lexer_next_rc_t lexer_next(lexer_t *lx, struct lexleo_token_t *out);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_LEXER_H
