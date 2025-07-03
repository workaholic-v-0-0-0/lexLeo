/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 8 "src/parser/bison/parser.y"

   #include "ast.h"
   typedef struct symtab symtab;
   typedef struct context { symtab *st; } *context;

   // forward declaration of yyscan_t for Bison (as Flex generates it)
   #ifndef YY_TYPEDEF_YY_SCANNER_T
   #define YY_TYPEDEF_YY_SCANNER_T
   typedef void* yyscan_t;
   extern void* yyget_extra(yyscan_t scanner);
   #endif

#line 62 "parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INTEGER = 258,                 /* INTEGER  */
    STRING = 259,                  /* STRING  */
    SYMBOL = 260,                  /* SYMBOL  */
    PARAMETERS = 261,              /* PARAMETERS  */
    EQUAL = 262,                   /* EQUAL  */
    EVALUATE = 263,                /* EVALUATE  */
    EXECUTE = 264,                 /* EXECUTE  */
    COMPUTE = 265,                 /* COMPUTE  */
    READ = 266,                    /* READ  */
    WRITE = 267,                   /* WRITE  */
    SEMICOLON = 268,               /* SEMICOLON  */
    LEFT_PARENTHESIS = 269,        /* LEFT_PARENTHESIS  */
    RIGHT_PARENTHESIS = 270,       /* RIGHT_PARENTHESIS  */
    PLUS = 271,                    /* PLUS  */
    MINUS = 272,                   /* MINUS  */
    MULTIPLY = 273,                /* MULTIPLY  */
    DIVIDE = 274                   /* DIVIDE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 36 "src/parser/bison/parser.y"

    int int_value;
    char *string_value;
    char *symbol_name_value;
    ast *ast;

#line 105 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (yyscan_t scanner);

/* "%code provides" blocks.  */
#line 31 "src/parser/bison/parser.y"

    int yyerror(yyscan_t scanner, const char *s);
    int yylex(YYSTYPE *yylval, yyscan_t scanner);

#line 124 "parser.tab.h"

#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
