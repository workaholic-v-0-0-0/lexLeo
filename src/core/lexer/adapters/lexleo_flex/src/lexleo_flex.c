/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/core/lexer/adapters/lexleo_flex/src/
 * lexleo_flex.c
 *
 * Flex lexer backend implementation (adapter).
 * Produces tokens via yyextra; may buffer tokens using the pending mechanism.
 */

#include "lexleo_token.h"
#include "lexleo_assert.h"
#include "internal/lexleo_flex_internal.h"
#include "mem/osal_mem_ops.h"
#include "lexer.yy.h"

static lexer_next_rc_t flex_next(lexleo_flex_t *b, lexleo_token_t *out ) {
	LEXLEO_ASSERT(b && out && b->state.scanner);

	*out = (lexleo_token_t){0};

	for (;;) {
		if (b->state.extra.has_pending) {
			*out = b->state.extra.pending;
			b->state.extra.has_pending = 0;
			return LEXER_NEXT_TOKEN;
		}

		int rc = lexleo_flex_lex(b->state.scanner);

		if (rc < 0) {
			out->kind = LEX_TK_INVALID;
			return LEXER_NEXT_ERROR;
		}

		if (b->state.extra.has_pending) {
			*out = b->state.extra.pending;
			b->state.extra.has_pending = 0;
			return LEXER_NEXT_TOKEN;
		}

		if (rc == 0) {
			return LEXER_NEXT_EOF;
		}
	}
}

static lexer_next_rc_t next(
		void *backend,
		struct lexleo_token_t *out) {
	return flex_next((lexleo_flex_t *)backend, out);
}

// destroy assumes a fully initialized backend (scanner != NULL).
static void destroy(void *backend) {
	lexleo_flex_t *b = (lexleo_flex_t*)backend;
	if (!b) return;
	LEXLEO_ASSERT(b->state.scanner);
	int rc = lexleo_flex_lex_destroy(b->state.scanner);
	LEXLEO_ASSERT(rc == 0);
	b->state.scanner = NULL;
	LEXLEO_ASSERT(b->mem && b->mem->free);
	b->mem->free(b);
}

static const lexer_vtbl_t DEFAULT_VTBL = {
	.next    = next,
	.destroy = destroy,
};

// create_backend initializes the Flex scanner and wires the input_provider
// into yyextra. This function assumes a fully valid ctx.
static lexer_status_t create_backend (
		lexleo_flex_t **out,
		struct input_provider *prov,
		const lexleo_flex_ctx_t *ctx ) {
	LEXLEO_ASSERT(
		   out
		&& prov
		&& ctx
		&& ctx->deps.mem
		&& ctx->deps.mem->calloc
		&& ctx->deps.mem->free
		&& ctx->deps.prov_ops
		&& ctx->deps.prov_ops->read
	);
	lexleo_flex_t *backend = ctx->deps.mem->calloc(1, sizeof(*backend));
	if (!backend)
		return LEXER_STATUS_ERROR;
	backend->mem = ctx->deps.mem;
	backend->state.extra.provider = prov;
	backend->state.extra.read = ctx->deps.prov_ops->read;
	backend->state.extra.mem = ctx->deps.mem;
	backend->state.extra.has_pending = 0;
	backend->state.extra.pending.kind = LEX_TK_EOF;
	backend->state.extra.pending.as.string_value = NULL;
	if (
			lexleo_flex_lex_init_extra(
				&backend->state.extra,
				&backend->state.scanner )
			!= 0 ) {
		ctx->deps.mem->free(backend);
		return LEXER_STATUS_ERROR;
	}
	*out = backend;
	return LEXER_STATUS_OK;
}

lexer_status_t lexleo_flex_create_lexer(
		lexer_t **out,
		struct input_provider *prov,
		const lexleo_flex_ctx_t *ctx ) {
	lexleo_flex_t *backend = NULL;
	if (create_backend(&backend, prov, ctx) != LEXER_STATUS_OK)
		return LEXER_STATUS_ERROR;
	lexer_ctx_t lx_ctx = { .mem = ctx->deps.mem };
	if (lexer_create(out, &ctx->lexer_vtbl, backend, &lx_ctx) != LEXER_STATUS_OK) {
		if (ctx->lexer_vtbl.destroy) ctx->lexer_vtbl.destroy(backend); // rollback
		return LEXER_STATUS_ERROR;
	}
	return LEXER_STATUS_OK;
}

lexleo_flex_ctx_t lexleo_flex_default_ctx(
		const osal_mem_ops_t *mem_ops,
		const input_provider_legacy_ops_t *prov_ops ) {
	lexleo_flex_ctx_t ctx;
	ctx.deps.mem = mem_ops ? mem_ops : osal_mem_default_ops();
	ctx.deps.prov_ops = prov_ops ? prov_ops : input_provider_legacy_default_ops();
	ctx.lexer_vtbl = DEFAULT_VTBL;
	return ctx;
}
