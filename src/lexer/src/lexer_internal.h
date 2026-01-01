// src/lexer/src/lexer_internal.h

#ifndef LEXLEO_LEXER_INTERNAL_H
#define LEXLEO_LEXER_INTERNAL_H

// for Flex and Bison
#include "lexer.yy.h" // yyscan_t, yylex_init, yylex_destroy, yyset_extra and yyget_extra
#include "parser.tab.h" // so that lexer set yylval properly (type YYSTYPE *)

#include <stdbool.h>

typedef int (*create_fn_t) (yyscan_t* scanner);
typedef int (*destroy_fn_t) (yyscan_t yyscanner);
typedef int (*scan_fn_t) (YYSTYPE *yylval_param, yyscan_t yyscanner);

typedef struct lexer_ops_t {
	create_fn_t create_fn;
	destroy_fn_t destroy_fn;
	scan_fn_t scan_fn;
} lexer_ops_t;

typedef struct lexer_ctx_t {
	const lexer_ops_t *ops;
} lexer_ctx_t;

typedef struct lexer_state_t {
	int first_token;
	bool first_token_is_pending;
	bool reached_input_end;
} lexer_state_t;

struct input_provider;

// make Flex knows lexer_scanner_extra_t via:
// %option extra-type="lexer_scanner_extra_t *"
typedef struct lexer_scanner_extra_t {
	lexer_state_t *state;
	struct input_provider *provider;
} lexer_scanner_extra_t;

struct lexer_t {
	lexer_ctx_t ctx;
	yyscan_t scanner; // own
	lexer_state_t state; // passed to Flex through extra->state
	struct input_provider *provider; // borrow from runtime_session ; // passed to Flex through extra->provider
	lexer_scanner_extra_t scanner_extra;
};

#endif //LEXLEO_LEXER_INTERNAL_H
