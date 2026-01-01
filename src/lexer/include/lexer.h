// src/lexer/include/lexer.h

#ifndef LEXLEO_LEXER_H
#define LEXLEO_LEXER_H

struct input_provider;

typedef struct lexer_t lexer_t; // opaque

lexer_t *lexer_create(struct input_provider *ip);
void lexer_destroy(lexer_t *lx);

// for semantic mode ; called only by parser
void lexer_inject_first_token(lexer_t *lx, int tk);

#endif //LEXLEO_LEXER_H
