// src/ports/lexer/include/internal/lexer_ctx.h

#ifndef LEXLEO_LEXER_CTX_H
#define LEXLEO_LEXER_CTX_H

#include "lexer.h"

#include "mem/internal/osal_mem_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// CTX
// ----------------------------------------------------------------------------

typedef lexer_next_rc_t (*lexer_next_fn_t)(
		void *backend,
		struct lexleo_token_t *out);

typedef void (*lexer_destroy_fn_t) (void *backend);

typedef struct lexer_ctx_t {
	const osal_mem_ops_t *mem;
} lexer_ctx_t;

typedef struct lexer_vtbl_t {
	lexer_next_fn_t next;
	lexer_destroy_fn_t destroy;
} lexer_vtbl_t;

typedef enum {
	LEXER_STATUS_OK = 0,
	LEXER_STATUS_ERROR = 1
} lexer_status_t;

lexer_status_t lexer_create(
	lexer_t **out,
	const lexer_vtbl_t *vtbl,
	void *backend,
	const lexer_ctx_t *ctx );

void lexer_destroy(lexer_t *lx);

/*
lexer_status_t lexer_create(
	lexer_t **out,
	const lexer_vtbl_t *vtbl,
	void *backend,
	const lexer_ctx_t *ctx)
{
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
*/

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_LEXER_CTX_H