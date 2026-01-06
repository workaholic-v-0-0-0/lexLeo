// src/integration/flex/src/flex_backend.c

#include "lexleo_flex_backend.h"

#include "lexleo_flex_scanner_extra.h"

#include "osal_mem.h"
#include "lexleo_assert.h"

typedef void* yyscan_t;

#define YY_EXTRA_TYPE lexleo_flex_scanner_extra_t *

int lexleo_flex_yylex_init_extra(YY_EXTRA_TYPE user_defined, yyscan_t *scanner);
int lexleo_flex_yylex_destroy(yyscan_t scanner);
int lexleo_flex_yylex(yyscan_t scanner);

struct lexleo_flex_backend {
	yyscan_t scanner;
	lexleo_flex_scanner_extra_t extra;
};

lexleo_flex_status_t lexleo_flex_backend_create(
		lexleo_flex_backend **out,
		struct input_provider *provider ) {
	if (!out || !provider)
		return LEXLEO_FLEX_STATUS_ERROR;

	lexleo_flex_backend *b = osal_calloc(1, sizeof(*b));
	if (!b)
		return LEXLEO_FLEX_STATUS_ERROR;

	b->extra.provider = provider;
	b->extra.has_pending = 0;
	/* b->extra.pending is zeroed by osal_calloc */

	if (lexleo_flex_yylex_init_extra(&b->extra, &b->scanner) != 0) {
		osal_free(b);
		return LEXLEO_FLEX_STATUS_ERROR;
	}

	*out = b;
	return LEXLEO_FLEX_STATUS_OK;
}

void lexleo_flex_backend_destroy(lexleo_flex_backend *b) {
	if (!b) return;
	if (b->scanner) {
		int rc = lexleo_flex_yylex_destroy(b->scanner);
		LEXLEO_ASSERT(rc == 0);
		(void)rc;
		b->scanner = NULL;
	}
	osal_free(b);
}

lexleo_flex_next_rc_t lexleo_flex_backend_next(
			lexleo_flex_backend *b,
			lexleo_token_t *out ) {
	if (!b || !out || !b->scanner)
		return LEXLEO_FLEX_NEXT_ERROR;

	*out = (lexleo_token_t){0};

	for (;;) {
		if (b->extra.has_pending) {
			*out = b->extra.pending;
			b->extra.has_pending = 0;
			return LEXLEO_FLEX_NEXT_TOKEN;
		}

		int rc = lexleo_flex_yylex(b->scanner);

		if (rc < 0) {
			return LEXLEO_FLEX_NEXT_ERROR;
		}

		if (b->extra.has_pending) {
			*out = b->extra.pending;
			b->extra.has_pending = 0;
			return LEXLEO_FLEX_NEXT_TOKEN;
		}

		if (rc == 0) {
			return LEXLEO_FLEX_NEXT_EOF;
		}
	}
}


