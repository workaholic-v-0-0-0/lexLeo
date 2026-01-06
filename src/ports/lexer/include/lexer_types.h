// src/ports/lexer/include/lexer_types.h

#ifndef LEXLEO_LEXER_TYPES_H
#define LEXLEO_LEXER_TYPES_H

typedef struct lexer_t lexer_t;

typedef enum {
	LEXER_NEXT_ERROR = -1,
	LEXER_NEXT_EOF = 0,
	LEXER_NEXT_TOKEN = 1
} lexer_next_rc_t;

#endif //LEXLEO_LEXER_TYPES_H