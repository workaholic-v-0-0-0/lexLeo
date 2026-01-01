// src/lexer/include/internal/lexer_bison.h

#ifndef LEXLEO_LEXER_BISON_H
#define LEXLEO_LEXER_BISON_H

#include "lexer.h"
#include "parser.tab.h"

int lexer_scan(lexer_t *lx, YYSTYPE *yylval_param);

#endif //LEXLEO_LEXER_BISON_H