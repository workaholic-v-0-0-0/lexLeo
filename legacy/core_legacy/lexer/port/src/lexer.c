// src/core/lexer/port/src/lexer.c

#include "lexer.h"
#include "lexer_ops.h"
#include "internal/lexer_internal.h"

lexer_status_t lexer_create(
		lexer_t **out,
		const lexer_vtbl_t *vtbl,
		void *backend,
		const lexer_ctx_t *ctx ) {
	if (!out || !vtbl || !vtbl->next || !ctx || !ctx->mem)
		return LEXER_STATUS_ERROR;

	lexer_t *lx = ctx->mem->malloc(sizeof(*lx));
	if (!lx) return LEXER_STATUS_ERROR;

	lx->vtbl = *vtbl;
	lx->backend = backend;
	lx->mem = ctx->mem;

	*out = lx;
	return LEXER_STATUS_OK;
}

void lexer_destroy(lexer_t *lx)
{
	if (!lx) return;
	if (lx->vtbl.destroy) lx->vtbl.destroy(lx->backend);
	lx->mem->free(lx);
}

lexer_next_rc_t lexer_next(lexer_t *lx, struct lexleo_token_t *out) {
	if (!lx || !lx->vtbl.next) return LEXER_NEXT_ERROR;
	return lx->vtbl.next(lx->backend, out);
}

const lexer_ops_t *lexer_default_ops(void)
{
	static const lexer_ops_t OPS = {
		.next = lexer_next
	};
	return &OPS;
}

