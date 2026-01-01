// src/lexer/src/lexer.c

#include "lexer.h"
#include "lexer_bison.h"
#include "lexer_internal.h"
#include "lexer_memory_allocator.h"

#include <string.h>

//#include "logger.h" // very soon

static const lexer_ops_t LEXER_OPS_FROM_FLEX = {
	.create_fn = yylex_init,
	.destroy_fn = yylex_destroy,
	.scan_fn = yylex
};

static const lexer_ctx_t LEXER_CTX_FROM_FLEX = {
	.ops = &LEXER_OPS_FROM_FLEX
};

lexer_t *lexer_create(struct input_provider *ip) {
	if (!ip) return NULL;

	lexer_t *lx = LEXER_MALLOC(sizeof(lexer_t));
	if (!lx) return NULL;
	memset(lx, 0, sizeof(lexer_t));

	lx->ctx = LEXER_CTX_FROM_FLEX;

	if (lx->ctx.ops->create_fn(&lx->scanner) != 0) {
		LEXER_FREE(lx);
		return NULL;
	}

	lx->state.first_token = ONE_STATEMENT_MODE;
	lx->state.first_token_is_pending = true;
	lx->state.reached_input_end = false;

	lx->provider = ip;

	lx->scanner_extra.state = &lx->state;
	lx->scanner_extra.provider = lx->provider;

	yyset_extra(&lx->scanner_extra, lx->scanner);

	return lx;
}

void lexer_destroy(lexer_t *lx) {
	if (!lx) return;
	if (lx->scanner) lx->ctx.ops->destroy_fn(lx->scanner);
	LEXER_FREE(lx);
}

void lexer_inject_first_token(lexer_t *lx, int tk) {
	if (!lx) return;
	lx->state.first_token = tk;
	lx->state.first_token_is_pending = true;
	lx->state.reached_input_end = false;
}

int lexer_scan(lexer_t *lx, YYSTYPE *yylval_param) {
	return lx->ctx.ops->scan_fn(yylval_param, lx->scanner);
}

// why not make Bison wrapper use Flex wrapper so that Flex stays agnostic of lexer state?
// -> very bad idea! Cause this is not only a param but a real grammar entry point
