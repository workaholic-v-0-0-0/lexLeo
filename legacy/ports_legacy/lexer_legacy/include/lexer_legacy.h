// src/ports/lexer_legacy/include/lexer_legacy.h

// port

#ifndef LEXLEO_LEXER_H
#define LEXLEO_LEXER_H

#ifdef __cplusplus
extern "C" {
#endif

struct lexleo_token_t;

// ----------------------------------------------------------------------------
// OPAQUE HANDLE
// ----------------------------------------------------------------------------

typedef struct lexer_t lexer_t;

// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------

typedef enum {
	LEXER_NEXT_ERROR = -1,
	LEXER_NEXT_EOF = 0,
	LEXER_NEXT_TOKEN = 1
} lexer_next_rc_t;

lexer_next_rc_t lexer_next(lexer_t *lx, struct lexleo_token_t *out);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_LEXER_H
