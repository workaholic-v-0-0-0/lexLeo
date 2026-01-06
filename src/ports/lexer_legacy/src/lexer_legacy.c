// src/ports/lexer/src/lexer.c

#include "lexer.h"

#include "lexer_internal.h"

lexer_next_rc_t lexer_next(lexer_t *lx, struct lexleo_token_t *out) {
	if (!lx || !lx->vtbl || !lx->vtbl->next) return LEXER_NEXT_ERROR;
	return lx->vtbl->next(lx->backend, out);
}
