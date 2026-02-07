/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/core/lexer/adapters/lexleo_flex/src/internal/
 * lexleo_flex_state.h
 *
 * Internal Flex scanner state.
 *
 * This header defines the runtime state carried by the reentrant
 * Flex scanner (yyextra) and the associated buffering logic.
 */

#ifndef LEXLEO_FLEX_STATE_H
#define LEXLEO_FLEX_STATE_H

#include "lexleo_token.h"
#include "input_provider_legacy_types.h"
#include "osal_mem_ops.h"

typedef void *lexleo_flex_yyscan_t;

// Extra state carried by the reentrant Flex scanner (yyextra).
typedef struct lexleo_flex_scanner_extra_t {
	struct input_provider *provider;
	input_provider_legacy_read_fn_t read;
	const osal_mem_ops_t *mem;

	/**
	 * Indicates whether a token has been produced by the scanner
	 * but not yet consumed by the lexer frontend.
	 *
	 * This is used to buffer a token when Flex rules need to
	 * return control before the token can be emitted.
	 */
	int has_pending;

	/**
	 * Buffered token returned on the next call when has_pending is set.
	 */
	lexleo_token_t pending;
} lexleo_flex_scanner_extra_t;

typedef struct lexleo_flex_state_t {
	lexleo_flex_yyscan_t scanner;
	lexleo_flex_scanner_extra_t extra;
} lexleo_flex_state_t;

#endif //LEXLEO_FLEX_STATE_H
