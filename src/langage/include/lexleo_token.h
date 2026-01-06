// src/langage/include/lexleo_token.h

#ifndef LEXLEO_LEXLEO_TOKEN_H
#define LEXLEO_LEXLEO_TOKEN_H

#include <stdint.h>

/**
 * Kind of lexical token produced by a lexer backend (Flex, etc.).
 *
 * This enum is backend-agnostic and parser-agnostic.
 * It represents the lexical contract of the language.
 */
typedef enum lexleo_token_kind_t {
	LEX_TK_INVALID = 0,

	/* punctuation */
	LEX_TK_LBRACE,
	LEX_TK_RBRACE,
	LEX_TK_LPAREN,
	LEX_TK_RPAREN,

	/* operators */
	LEX_TK_ADD,
	LEX_TK_SUBTRACT,
	LEX_TK_MULTIPLY,
	LEX_TK_DIVIDE,

	/* separators / assignment */
	LEX_TK_SEMICOLON,
	LEX_TK_COMMA,
	LEX_TK_EQUAL,

	/* keywords */
	LEX_TK_READ,
	LEX_TK_WRITE,
	LEX_TK_DEFINE,
	LEX_TK_CALL,
	LEX_TK_QUOTE,
	LEX_TK_EVAL,
	LEX_TK_SYMBOL,
	LEX_TK_SET,
	LEX_TK_IF,
	LEX_TK_THEN,
	LEX_TK_ELSE,
	LEX_TK_WHILE,
	LEX_TK_DO,

	/* literals / identifiers */
	LEX_TK_INTEGER,
	LEX_TK_STRING,
	LEX_TK_SYMBOL_NAME,

	/* end of input */
	LEX_TK_EOF

} lexleo_token_kind_t;

/**
 * Lexical token.
 *
 * Ownership rules:
 * - For LEX_TK_STRING and LEX_TK_SYMBOL_NAME, string_value is heap-allocated
 *   and ownership is transferred to the token consumer.
 * - For other token kinds, the union content is unspecified.
 */
typedef struct lexleo_token_t {
	lexleo_token_kind_t kind;
	union {
		int int_value;
		char *string_value;
		char *symbol_value;
	} as;
} lexleo_token_t;

#endif //LEXLEO_LEXLEO_TOKEN_H
