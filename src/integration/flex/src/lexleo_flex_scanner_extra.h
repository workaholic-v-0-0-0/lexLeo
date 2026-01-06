// src/integration/flex/lexleo_flex_scanner_extra.h

#ifndef LEXLEO_LEXLEO_FLEX_SCANNER_EXTRA_H
#define LEXLEO_LEXLEO_FLEX_SCANNER_EXTRA_H

#include "lexleo_token.h"

struct input_provider;

/*
 * Extra state carried by the reentrant Flex scanner (yyextra).
 * This type is private to the Flex integration layer.
 */
typedef struct lexleo_flex_scanner_extra_t {
	struct input_provider *provider;
	int has_pending;
	lexleo_token_t pending;
} lexleo_flex_scanner_extra_t;

#endif //LEXLEO_LEXLEO_FLEX_SCANNER_EXTRA_H
