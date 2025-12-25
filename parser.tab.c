/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 23 "build/src/parser/bison/parser.y"

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include "ast.h"
    #include "parser_ctx.h"

#line 79 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 14 "build/src/parser/bison/parser.y"

    #include "parser_types.h"
    struct parser_ctx;

    // forward declaration
    typedef void* yyscan_t;
    void *yyget_extra(yyscan_t scanner);

#line 120 "parser.tab.c"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    END = 0,                       /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INTEGER = 258,                 /* INTEGER  */
    STRING = 259,                  /* STRING  */
    SYMBOL_NAME = 260,             /* SYMBOL_NAME  */
    LBRACE = 261,                  /* LBRACE  */
    RBRACE = 262,                  /* RBRACE  */
    LPAREN = 263,                  /* LPAREN  */
    RPAREN = 264,                  /* RPAREN  */
    ADD = 265,                     /* ADD  */
    SUBTRACT = 266,                /* SUBTRACT  */
    MULTIPLY = 267,                /* MULTIPLY  */
    DIVIDE = 268,                  /* DIVIDE  */
    SEMICOLON = 269,               /* SEMICOLON  */
<<<<<<< HEAD
    COMMA = 270,                   /* COMMA  */
    EQUAL = 271,                   /* EQUAL  */
    READ = 272,                    /* READ  */
    WRITE = 273,                   /* WRITE  */
    DEFINE = 274,                  /* DEFINE  */
    CALL = 275,                    /* CALL  */
    QUOTE = 276,                   /* QUOTE  */
    EVAL = 277,                    /* EVAL  */
    INVALID = 278,                 /* INVALID  */
    UMINUS = 279,                  /* UMINUS  */
    TU_MODE = 280,                 /* TU_MODE  */
    ONE_STATEMENT_MODE = 281,      /* ONE_STATEMENT_MODE  */
    READABLE_MODE = 282            /* READABLE_MODE  */
=======
    EQUAL = 270,                   /* EQUAL  */
    READ = 271,                    /* READ  */
    WRITE = 272,                   /* WRITE  */
    DEFINE = 273,                  /* DEFINE  */
    CALL = 274,                    /* CALL  */
    QUOTE = 275,                   /* QUOTE  */
    EVAL = 276,                    /* EVAL  */
    INVALID = 277,                 /* INVALID  */
    UMINUS = 278,                  /* UMINUS  */
    TU_MODE = 279,                 /* TU_MODE  */
    ONE_STATEMENT_MODE = 280,      /* ONE_STATEMENT_MODE  */
    READABLE_MODE = 281            /* READABLE_MODE  */
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 36 "build/src/parser/bison/parser.y"

    int int_value;
    char *string_value;
    char *symbol_name_value;
ast *ast;

<<<<<<< HEAD
#line 171 "parser.tab.c"
=======
#line 170 "parser.tab.c"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (yyscan_t scanner, ast **result, struct parser_ctx *ctx);

/* "%code provides" blocks.  */
#line 31 "build/src/parser/bison/parser.y"

    int yyerror(yyscan_t scanner, ast **result, struct parser_ctx *ctx, const char *s);
    int yylex(YYSTYPE *yylval, yyscan_t scanner);

<<<<<<< HEAD
#line 190 "parser.tab.c"
=======
#line 189 "parser.tab.c"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD


/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INTEGER = 3,                    /* INTEGER  */
  YYSYMBOL_STRING = 4,                     /* STRING  */
  YYSYMBOL_SYMBOL_NAME = 5,                /* SYMBOL_NAME  */
  YYSYMBOL_LBRACE = 6,                     /* LBRACE  */
  YYSYMBOL_RBRACE = 7,                     /* RBRACE  */
  YYSYMBOL_LPAREN = 8,                     /* LPAREN  */
  YYSYMBOL_RPAREN = 9,                     /* RPAREN  */
  YYSYMBOL_ADD = 10,                       /* ADD  */
  YYSYMBOL_SUBTRACT = 11,                  /* SUBTRACT  */
  YYSYMBOL_MULTIPLY = 12,                  /* MULTIPLY  */
  YYSYMBOL_DIVIDE = 13,                    /* DIVIDE  */
  YYSYMBOL_SEMICOLON = 14,                 /* SEMICOLON  */
<<<<<<< HEAD
  YYSYMBOL_COMMA = 15,                     /* COMMA  */
  YYSYMBOL_EQUAL = 16,                     /* EQUAL  */
  YYSYMBOL_READ = 17,                      /* READ  */
  YYSYMBOL_WRITE = 18,                     /* WRITE  */
  YYSYMBOL_DEFINE = 19,                    /* DEFINE  */
  YYSYMBOL_CALL = 20,                      /* CALL  */
  YYSYMBOL_QUOTE = 21,                     /* QUOTE  */
  YYSYMBOL_EVAL = 22,                      /* EVAL  */
  YYSYMBOL_INVALID = 23,                   /* INVALID  */
  YYSYMBOL_UMINUS = 24,                    /* UMINUS  */
  YYSYMBOL_TU_MODE = 25,                   /* TU_MODE  */
  YYSYMBOL_ONE_STATEMENT_MODE = 26,        /* ONE_STATEMENT_MODE  */
  YYSYMBOL_READABLE_MODE = 27,             /* READABLE_MODE  */
  YYSYMBOL_YYACCEPT = 28,                  /* $accept  */
  YYSYMBOL_start = 29,                     /* start  */
  YYSYMBOL_readable = 30,                  /* readable  */
  YYSYMBOL_arguments = 31,                 /* arguments  */
  YYSYMBOL_argument_list = 32,             /* argument_list  */
  YYSYMBOL_atom = 33,                      /* atom  */
  YYSYMBOL_binding = 34,                   /* binding  */
  YYSYMBOL_block_items = 35,               /* block_items  */
  YYSYMBOL_block = 36,                     /* block  */
  YYSYMBOL_computable = 37,                /* computable  */
  YYSYMBOL_eval = 38,                      /* eval  */
  YYSYMBOL_evaluable = 39,                 /* evaluable  */
  YYSYMBOL_function_call = 40,             /* function_call  */
  YYSYMBOL_function_definition = 41,       /* function_definition  */
  YYSYMBOL_function = 42,                  /* function  */
  YYSYMBOL_list_of_arguments = 43,         /* list_of_arguments  */
  YYSYMBOL_list_of_parameters = 44,        /* list_of_parameters  */
  YYSYMBOL_number_atom = 45,               /* number_atom  */
  YYSYMBOL_parameters = 46,                /* parameters  */
  YYSYMBOL_reading = 47,                   /* reading  */
  YYSYMBOL_statement = 48,                 /* statement  */
  YYSYMBOL_string_atom = 49,               /* string_atom  */
  YYSYMBOL_symbol_name_atom = 50,          /* symbol_name_atom  */
  YYSYMBOL_translation_unit = 51,          /* translation_unit  */
  YYSYMBOL_writing = 52                    /* writing  */
=======
  YYSYMBOL_EQUAL = 15,                     /* EQUAL  */
  YYSYMBOL_READ = 16,                      /* READ  */
  YYSYMBOL_WRITE = 17,                     /* WRITE  */
  YYSYMBOL_DEFINE = 18,                    /* DEFINE  */
  YYSYMBOL_CALL = 19,                      /* CALL  */
  YYSYMBOL_QUOTE = 20,                     /* QUOTE  */
  YYSYMBOL_EVAL = 21,                      /* EVAL  */
  YYSYMBOL_INVALID = 22,                   /* INVALID  */
  YYSYMBOL_UMINUS = 23,                    /* UMINUS  */
  YYSYMBOL_TU_MODE = 24,                   /* TU_MODE  */
  YYSYMBOL_ONE_STATEMENT_MODE = 25,        /* ONE_STATEMENT_MODE  */
  YYSYMBOL_READABLE_MODE = 26,             /* READABLE_MODE  */
  YYSYMBOL_YYACCEPT = 27,                  /* $accept  */
  YYSYMBOL_start = 28,                     /* start  */
  YYSYMBOL_readable = 29,                  /* readable  */
  YYSYMBOL_atom = 30,                      /* atom  */
  YYSYMBOL_binding = 31,                   /* binding  */
  YYSYMBOL_block_items = 32,               /* block_items  */
  YYSYMBOL_block = 33,                     /* block  */
  YYSYMBOL_computable_core = 34,           /* computable_core  */
  YYSYMBOL_computable = 35,                /* computable  */
  YYSYMBOL_eval = 36,                      /* eval  */
  YYSYMBOL_evaluable = 37,                 /* evaluable  */
  YYSYMBOL_function_call = 38,             /* function_call  */
  YYSYMBOL_function_definition = 39,       /* function_definition  */
  YYSYMBOL_function = 40,                  /* function  */
  YYSYMBOL_list_of_numbers = 41,           /* list_of_numbers  */
  YYSYMBOL_list_of_parameters = 42,        /* list_of_parameters  */
  YYSYMBOL_number_atom = 43,               /* number_atom  */
  YYSYMBOL_numbers = 44,                   /* numbers  */
  YYSYMBOL_parameters = 45,                /* parameters  */
  YYSYMBOL_reading = 46,                   /* reading  */
  YYSYMBOL_statement = 47,                 /* statement  */
  YYSYMBOL_string_atom = 48,               /* string_atom  */
  YYSYMBOL_symbol_name_atom = 49,          /* symbol_name_atom  */
  YYSYMBOL_translation_unit = 50,          /* translation_unit  */
  YYSYMBOL_writing = 51                    /* writing  */
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
<<<<<<< HEAD
#define YYFINAL  35
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   131

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  28
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  25
/* YYNRULES -- Number of rules.  */
#define YYNRULES  49
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  80

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   282
=======
#define YYFINAL  36
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   158

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  27
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  25
/* YYNRULES -- Number of rules.  */
#define YYNRULES  55
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  92

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   281
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
<<<<<<< HEAD
      25,    26,    27
=======
      25,    26
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
<<<<<<< HEAD
       0,    87,    87,    91,    96,   101,   109,   110,   116,   123,
     127,   134,   147,   148,   149,   155,   176,   188,   209,   228,
     242,   258,   274,   290,   306,   309,   318,   337,   338,   339,
     358,   379,   398,   421,   440,   459,   472,   484,   505,   524,
     525,   526,   527,   528,   529,   535,   549,   563,   575,   596
=======
       0,    86,    86,    90,    95,   100,   108,   109,   115,   116,
     117,   123,   144,   156,   177,   196,   210,   226,   242,   258,
     274,   277,   280,   286,   300,   316,   332,   348,   364,   372,
     391,   392,   393,   394,   413,   434,   453,   476,   495,   514,
     527,   539,   593,   605,   626,   645,   646,   647,   648,   649,
     650,   656,   670,   684,   696,   717
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INTEGER", "STRING",
  "SYMBOL_NAME", "LBRACE", "RBRACE", "LPAREN", "RPAREN", "ADD", "SUBTRACT",
<<<<<<< HEAD
  "MULTIPLY", "DIVIDE", "SEMICOLON", "COMMA", "EQUAL", "READ", "WRITE",
  "DEFINE", "CALL", "QUOTE", "EVAL", "INVALID", "UMINUS", "TU_MODE",
  "ONE_STATEMENT_MODE", "READABLE_MODE", "$accept", "start", "readable",
  "arguments", "argument_list", "atom", "binding", "block_items", "block",
  "computable", "eval", "evaluable", "function_call",
  "function_definition", "function", "list_of_arguments",
  "list_of_parameters", "number_atom", "parameters", "reading",
=======
  "MULTIPLY", "DIVIDE", "SEMICOLON", "EQUAL", "READ", "WRITE", "DEFINE",
  "CALL", "QUOTE", "EVAL", "INVALID", "UMINUS", "TU_MODE",
  "ONE_STATEMENT_MODE", "READABLE_MODE", "$accept", "start", "readable",
  "atom", "binding", "block_items", "block", "computable_core",
  "computable", "eval", "evaluable", "function_call",
  "function_definition", "function", "list_of_numbers",
  "list_of_parameters", "number_atom", "numbers", "parameters", "reading",
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
  "statement", "string_atom", "symbol_name_atom", "translation_unit",
  "writing", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

<<<<<<< HEAD
#define YYPACT_NINF (-10)
=======
#define YYPACT_NINF (-5)
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

<<<<<<< HEAD
#define YYTABLE_NINF (-1)
=======
#define YYTABLE_NINF (-29)
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
<<<<<<< HEAD
static const yytype_int8 yypact[] =
{
      31,   -10,    25,   100,     7,    60,   -10,   -10,   -10,   -10,
     120,   120,     8,   109,     8,   109,     8,   -10,   -10,    11,
     -10,     3,   -10,   -10,   -10,   -10,   -10,   -10,    -5,   -10,
     -10,   -10,   -10,   -10,    23,   -10,   -10,   -10,    83,   -10,
     -10,     5,    18,   -10,    26,   -10,    21,   120,   120,   120,
     120,   -10,   109,   109,   -10,    80,   -10,   -10,   -10,   -10,
      39,   -10,    14,    14,   -10,   -10,    28,    33,   -10,    47,
     -10,   -10,    -1,   -10,   -10,   109,   -10,   -10,   -10,   -10
=======
static const yytype_int16 yypact[] =
{
     122,    -5,    20,   119,    19,    81,    -5,    -5,    -5,    -5,
      85,    85,    22,   129,    22,   129,    22,    -5,    -5,   145,
      -5,    -5,    15,    -5,    -5,    34,    -5,    -5,    -5,    98,
      -5,    -5,    -5,    -5,    -5,    58,    -5,    -5,    -5,    85,
      85,   132,    -5,    -5,    35,    38,    55,    -5,    71,    -5,
      56,    85,    85,    85,    85,    -5,    -5,   129,    -5,   100,
     141,    -5,    39,    85,    85,    85,    85,    -5,    -5,    -5,
      74,    -5,    30,    33,    54,    73,    23,    68,    -5,    -5,
      -5,    -4,    -4,    -5,    -5,    62,    -5,    -5,    -5,    -5,
      -5,    -5
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
<<<<<<< HEAD
       0,    47,     0,     0,     0,     0,     4,    35,    45,    46,
       0,     0,     0,     0,     0,     0,     0,    25,    39,    28,
      44,     0,    27,    42,    12,    41,     3,    13,    14,    40,
      16,     5,     7,     6,    14,     1,     2,    48,     0,    14,
      19,     0,     0,    31,     0,    29,     0,     0,     0,     0,
       0,    43,     8,     0,    30,     0,    24,    38,    49,    36,
       0,    26,    20,    21,    22,    23,     0,     9,    10,     0,
      18,    17,     0,    32,    33,     0,    15,    34,    37,    11
=======
       0,    53,     0,     0,     0,     0,     4,    39,    51,    52,
       0,     0,     0,     0,     0,     0,     0,    31,    45,     0,
      32,    50,     0,    30,    48,    21,    47,     3,     9,    22,
      46,    12,     5,     7,     6,    22,     1,     2,    54,     0,
       0,     0,    21,    22,    15,     0,     0,    35,     0,    33,
       0,     0,     0,     0,     0,    49,    40,     0,    34,     0,
       0,    15,    20,     0,     0,     0,     0,    44,    55,    42,
       0,    29,    16,    17,    18,    19,     0,     0,    14,    13,
      20,    16,    17,    18,    19,     0,    36,    37,    41,    11,
      38,    43
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
<<<<<<< HEAD
     -10,   -10,   -10,   -10,   -10,   -10,   -10,   -10,     2,    -9,
     -10,    -3,   -10,   -10,   -10,   -10,   -10,   -10,   -10,   -10,
       0,   -10,     4,   -10,   -10
=======
      -5,    -5,    -5,    -5,    -5,    -5,    21,    11,    -5,    -5,
       3,    -5,    -5,    -5,    -5,    -5,     2,    -5,    -5,    -5,
      -3,    -5,    -2,    -5,    -5
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
<<<<<<< HEAD
       0,     4,    31,    66,    67,    17,    18,    55,    32,    19,
      20,    21,    22,    23,    43,    54,    60,    24,    72,    25,
      26,    27,    34,     5,    29
=======
       0,     4,    32,    17,    18,    59,    33,    19,    20,    21,
      22,    23,    24,    47,    58,    70,    42,    76,    85,    26,
      27,    28,    43,     5,    30
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
<<<<<<< HEAD
      33,    38,    40,    52,     9,    37,    28,    35,    77,    28,
      42,    53,    45,     9,    39,    39,    41,    51,    44,    57,
      46,    47,    48,    49,    50,     6,    49,    50,     7,     8,
       9,    52,    58,    10,    59,    61,    11,    74,    62,    63,
      64,    65,    12,    13,    14,    30,    15,    16,    75,    68,
      69,    39,    39,    39,    39,    71,     1,     2,     3,    28,
      36,    76,    73,     7,     8,     9,     0,     0,    10,     0,
       0,    11,    79,     0,     0,     0,    78,    12,    13,    14,
       0,    15,    16,     7,     8,     9,     0,    70,    10,     0,
       0,    11,    56,    47,    48,    49,    50,    12,    13,    14,
       0,    15,    16,     7,     8,     9,    30,     0,    10,     0,
       0,    11,     7,     8,     9,     0,     0,    10,     0,     0,
      11,    15,     0,     7,     8,     9,     0,     0,    10,     0,
      15,    11
=======
      29,    35,    38,    29,    25,    25,    34,    25,    65,    66,
      45,    35,    48,    35,    50,    25,    46,    25,    49,    36,
       6,    41,    44,     7,     8,     9,     7,     9,    10,    55,
     -24,    11,    87,   -25,    -8,   -23,    12,    13,    14,   -28,
      15,    16,    65,    66,   -24,    65,    66,   -25,    -8,   -23,
      60,    61,    67,   -28,   -26,    35,    79,    29,   -10,    25,
      77,    25,    72,    73,    74,    75,    56,     9,   -26,    68,
      71,    90,   -10,   -27,    81,    82,    83,    84,    88,    69,
      31,    37,    89,    91,     7,     8,     9,   -27,     7,    10,
       9,    86,    11,    39,     0,     0,    40,    12,    13,    14,
       0,    15,    16,     7,     8,     9,    56,    78,    10,     0,
       0,    11,   -10,    57,     0,     0,    12,    13,    14,     0,
      15,    16,     7,     8,     9,    31,     0,    10,     0,     0,
      11,     0,     7,     8,     9,     0,     0,    10,     0,    15,
      11,    62,    63,    64,    65,    66,     1,     2,     3,    15,
      80,    63,    64,    65,    66,    51,    52,    53,    54
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};

static const yytype_int8 yycheck[] =
{
<<<<<<< HEAD
       3,    10,    11,     8,     5,     5,     2,     0,     9,     5,
      13,    16,    15,     5,    10,    11,    12,    14,    14,    14,
      16,    10,    11,    12,    13,     0,    12,    13,     3,     4,
       5,     8,    14,     8,     8,    14,    11,     9,    47,    48,
      49,    50,    17,    18,    19,     6,    21,    22,    15,    52,
      53,    47,    48,    49,    50,    55,    25,    26,    27,    55,
       0,    14,    60,     3,     4,     5,    -1,    -1,     8,    -1,
      -1,    11,    75,    -1,    -1,    -1,    72,    17,    18,    19,
      -1,    21,    22,     3,     4,     5,    -1,     7,     8,    -1,
      -1,    11,     9,    10,    11,    12,    13,    17,    18,    19,
      -1,    21,    22,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    11,     3,     4,     5,    -1,    -1,     8,    -1,    -1,
      11,    21,    -1,     3,     4,     5,    -1,    -1,     8,    -1,
      21,    11
=======
       2,     3,     5,     5,     2,     3,     3,     5,    12,    13,
      12,    13,    14,    15,    16,    13,    13,    15,    15,     0,
       0,    10,    11,     3,     4,     5,     3,     5,     8,    14,
       0,    11,     9,     0,     0,     0,    16,    17,    18,     0,
      20,    21,    12,    13,    14,    12,    13,    14,    14,    14,
      39,    40,    14,    14,     0,    57,    59,    59,     0,    57,
      57,    59,    51,    52,    53,    54,     8,     5,    14,    14,
      14,     9,    14,     0,    63,    64,    65,    66,    76,     8,
       6,     0,    14,    85,     3,     4,     5,    14,     3,     8,
       5,    70,    11,     8,    -1,    -1,    11,    16,    17,    18,
      -1,    20,    21,     3,     4,     5,     8,     7,     8,    -1,
      -1,    11,    14,    15,    -1,    -1,    16,    17,    18,    -1,
      20,    21,     3,     4,     5,     6,    -1,     8,    -1,    -1,
      11,    -1,     3,     4,     5,    -1,    -1,     8,    -1,    20,
      11,     9,    10,    11,    12,    13,    24,    25,    26,    20,
       9,    10,    11,    12,    13,    10,    11,    12,    13
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
<<<<<<< HEAD
       0,    25,    26,    27,    29,    51,     0,     3,     4,     5,
       8,    11,    17,    18,    19,    21,    22,    33,    34,    37,
      38,    39,    40,    41,    45,    47,    48,    49,    50,    52,
       6,    30,    36,    39,    50,     0,     0,    48,    37,    50,
      37,    50,    39,    42,    50,    39,    50,    10,    11,    12,
      13,    14,     8,    16,    43,    35,     9,    14,    14,     8,
      44,    14,    37,    37,    37,    37,    31,    32,    39,    39,
       7,    48,    46,    36,     9,    15,    14,     9,    50,    39
=======
       0,    24,    25,    26,    28,    50,     0,     3,     4,     5,
       8,    11,    16,    17,    18,    20,    21,    30,    31,    34,
      35,    36,    37,    38,    39,    43,    46,    47,    48,    49,
      51,     6,    29,    33,    37,    49,     0,     0,    47,     8,
      11,    34,    43,    49,    34,    49,    37,    40,    49,    37,
      49,    10,    11,    12,    13,    14,     8,    15,    41,    32,
      34,    34,     9,    10,    11,    12,    13,    14,    14,     8,
      42,    14,    34,    34,    34,    34,    44,    37,     7,    47,
       9,    34,    34,    34,    34,    45,    33,     9,    43,    14,
       9,    49
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
<<<<<<< HEAD
       0,    28,    29,    29,    29,    29,    30,    30,    31,    31,
      32,    32,    33,    33,    33,    34,    35,    35,    36,    37,
      37,    37,    37,    37,    37,    37,    38,    39,    39,    39,
      40,    41,    42,    43,    44,    45,    46,    46,    47,    48,
      48,    48,    48,    48,    48,    49,    50,    51,    51,    52
=======
       0,    27,    28,    28,    28,    28,    29,    29,    30,    30,
      30,    31,    32,    32,    33,    34,    34,    34,    34,    34,
      34,    34,    34,    35,    35,    35,    35,    35,    35,    36,
      37,    37,    37,    37,    38,    39,    40,    41,    42,    43,
      44,    44,    45,    45,    46,    47,    47,    47,    47,    47,
      47,    48,    49,    50,    50,    51
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
<<<<<<< HEAD
       0,     2,     3,     2,     2,     2,     1,     1,     0,     1,
       1,     3,     1,     1,     1,     4,     0,     2,     3,     2,
       3,     3,     3,     3,     3,     1,     3,     1,     1,     2,
       2,     2,     3,     3,     3,     1,     0,     2,     3,     1,
       1,     1,     1,     2,     1,     1,     1,     0,     2,     3
=======
       0,     2,     3,     2,     2,     2,     1,     1,     1,     1,
       1,     4,     0,     2,     3,     2,     3,     3,     3,     3,
       3,     1,     1,     2,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     2,     2,     2,     3,     3,     3,     1,
       0,     2,     0,     2,     3,     1,     1,     1,     1,     2,
       1,     1,     1,     0,     2,     3
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (scanner, result, ctx, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, scanner, result, ctx); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, yyscan_t scanner, ast **result, struct parser_ctx *ctx)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (scanner);
  YY_USE (result);
  YY_USE (ctx);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, yyscan_t scanner, ast **result, struct parser_ctx *ctx)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, scanner, result, ctx);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, yyscan_t scanner, ast **result, struct parser_ctx *ctx)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], scanner, result, ctx);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner, result, ctx); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, yyscan_t scanner, ast **result, struct parser_ctx *ctx)
{
  YY_USE (yyvaluep);
  YY_USE (scanner);
  YY_USE (result);
  YY_USE (ctx);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_STRING: /* STRING  */
#line 42 "build/src/parser/bison/parser.y"
            { free(((*yyvaluep).string_value)); }
<<<<<<< HEAD
#line 1282 "parser.tab.c"
=======
#line 1293 "parser.tab.c"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
        break;

    case YYSYMBOL_SYMBOL_NAME: /* SYMBOL_NAME  */
#line 42 "build/src/parser/bison/parser.y"
            { free(((*yyvaluep).symbol_name_value)); }
<<<<<<< HEAD
#line 1288 "parser.tab.c"
        break;

    case YYSYMBOL_readable: /* readable  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1294 "parser.tab.c"
        break;

    case YYSYMBOL_arguments: /* arguments  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1300 "parser.tab.c"
        break;

    case YYSYMBOL_argument_list: /* argument_list  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1306 "parser.tab.c"
        break;

    case YYSYMBOL_atom: /* atom  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1312 "parser.tab.c"
        break;

    case YYSYMBOL_binding: /* binding  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1318 "parser.tab.c"
        break;

    case YYSYMBOL_block_items: /* block_items  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1324 "parser.tab.c"
        break;

    case YYSYMBOL_block: /* block  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1330 "parser.tab.c"
        break;

    case YYSYMBOL_computable: /* computable  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1336 "parser.tab.c"
        break;

    case YYSYMBOL_eval: /* eval  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1342 "parser.tab.c"
        break;

    case YYSYMBOL_evaluable: /* evaluable  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1348 "parser.tab.c"
        break;

    case YYSYMBOL_function_call: /* function_call  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1354 "parser.tab.c"
        break;

    case YYSYMBOL_function_definition: /* function_definition  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1360 "parser.tab.c"
        break;

    case YYSYMBOL_function: /* function  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1366 "parser.tab.c"
        break;

    case YYSYMBOL_list_of_arguments: /* list_of_arguments  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1372 "parser.tab.c"
        break;

    case YYSYMBOL_list_of_parameters: /* list_of_parameters  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1378 "parser.tab.c"
        break;

    case YYSYMBOL_number_atom: /* number_atom  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1384 "parser.tab.c"
        break;

    case YYSYMBOL_parameters: /* parameters  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1390 "parser.tab.c"
        break;

    case YYSYMBOL_reading: /* reading  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1396 "parser.tab.c"
        break;

    case YYSYMBOL_statement: /* statement  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1402 "parser.tab.c"
        break;

    case YYSYMBOL_string_atom: /* string_atom  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1408 "parser.tab.c"
        break;

    case YYSYMBOL_symbol_name_atom: /* symbol_name_atom  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1414 "parser.tab.c"
        break;

    case YYSYMBOL_translation_unit: /* translation_unit  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1420 "parser.tab.c"
        break;

    case YYSYMBOL_writing: /* writing  */
#line 76 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1426 "parser.tab.c"
=======
#line 1299 "parser.tab.c"
        break;

    case YYSYMBOL_readable: /* readable  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1305 "parser.tab.c"
        break;

    case YYSYMBOL_atom: /* atom  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1311 "parser.tab.c"
        break;

    case YYSYMBOL_binding: /* binding  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1317 "parser.tab.c"
        break;

    case YYSYMBOL_block_items: /* block_items  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1323 "parser.tab.c"
        break;

    case YYSYMBOL_block: /* block  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1329 "parser.tab.c"
        break;

    case YYSYMBOL_computable_core: /* computable_core  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1335 "parser.tab.c"
        break;

    case YYSYMBOL_computable: /* computable  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1341 "parser.tab.c"
        break;

    case YYSYMBOL_eval: /* eval  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1347 "parser.tab.c"
        break;

    case YYSYMBOL_evaluable: /* evaluable  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1353 "parser.tab.c"
        break;

    case YYSYMBOL_function_call: /* function_call  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1359 "parser.tab.c"
        break;

    case YYSYMBOL_function_definition: /* function_definition  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1365 "parser.tab.c"
        break;

    case YYSYMBOL_function: /* function  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1371 "parser.tab.c"
        break;

    case YYSYMBOL_list_of_numbers: /* list_of_numbers  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1377 "parser.tab.c"
        break;

    case YYSYMBOL_list_of_parameters: /* list_of_parameters  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1383 "parser.tab.c"
        break;

    case YYSYMBOL_number_atom: /* number_atom  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1389 "parser.tab.c"
        break;

    case YYSYMBOL_numbers: /* numbers  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1395 "parser.tab.c"
        break;

    case YYSYMBOL_parameters: /* parameters  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1401 "parser.tab.c"
        break;

    case YYSYMBOL_reading: /* reading  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1407 "parser.tab.c"
        break;

    case YYSYMBOL_statement: /* statement  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1413 "parser.tab.c"
        break;

    case YYSYMBOL_string_atom: /* string_atom  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1419 "parser.tab.c"
        break;

    case YYSYMBOL_symbol_name_atom: /* symbol_name_atom  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1425 "parser.tab.c"
        break;

    case YYSYMBOL_translation_unit: /* translation_unit  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1431 "parser.tab.c"
        break;

    case YYSYMBOL_writing: /* writing  */
#line 75 "build/src/parser/bison/parser.y"
            { if (((*yyvaluep).ast)) ctx->ops.destroy(((*yyvaluep).ast)); }
#line 1437 "parser.tab.c"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (yyscan_t scanner, ast **result, struct parser_ctx *ctx)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= END)
    {
      yychar = END;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* start: TU_MODE translation_unit "end of file"  */
<<<<<<< HEAD
#line 87 "build/src/parser/bison/parser.y"
=======
#line 86 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                                   {
        if (ctx->goal != PARSE_GOAL_TU) YYERROR;
        *result = (yyvsp[-1].ast);
    }
<<<<<<< HEAD
#line 1708 "parser.tab.c"
    break;

  case 3: /* start: ONE_STATEMENT_MODE statement  */
#line 91 "build/src/parser/bison/parser.y"
=======
#line 1719 "parser.tab.c"
    break;

  case 3: /* start: ONE_STATEMENT_MODE statement  */
#line 90 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                                   {
        if (ctx->goal != PARSE_GOAL_ONE_STATEMENT) YYERROR;
        *result = (yyvsp[0].ast);
        YYACCEPT;
    }
<<<<<<< HEAD
#line 1718 "parser.tab.c"
    break;

  case 4: /* start: ONE_STATEMENT_MODE "end of file"  */
#line 96 "build/src/parser/bison/parser.y"
=======
#line 1729 "parser.tab.c"
    break;

  case 4: /* start: ONE_STATEMENT_MODE "end of file"  */
#line 95 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                             {
        if (ctx->goal != PARSE_GOAL_ONE_STATEMENT) YYERROR;
        *result = NULL;
        YYACCEPT;
    }
<<<<<<< HEAD
#line 1728 "parser.tab.c"
    break;

  case 5: /* start: READABLE_MODE readable  */
#line 101 "build/src/parser/bison/parser.y"
=======
#line 1739 "parser.tab.c"
    break;

  case 5: /* start: READABLE_MODE readable  */
#line 100 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                             {
        if (ctx->goal != PARSE_GOAL_READABLE) YYERROR;
        *result = (yyvsp[0].ast);
        YYACCEPT;
    }
<<<<<<< HEAD
#line 1738 "parser.tab.c"
    break;

  case 6: /* readable: evaluable  */
#line 109 "build/src/parser/bison/parser.y"
                {(yyval.ast) = (yyvsp[0].ast);}
#line 1744 "parser.tab.c"
    break;

  case 7: /* readable: block  */
#line 110 "build/src/parser/bison/parser.y"
            {(yyval.ast) = (yyvsp[0].ast);}
#line 1750 "parser.tab.c"
    break;

  case 8: /* arguments: %empty  */
#line 116 "build/src/parser/bison/parser.y"
           {
      ast *a = ctx->ops.create_children_node_var(AST_TYPE_ARGUMENTS, 0);
      if (!a) (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_ARGUMENTS_NODE_CREATION_FAILED,
                    "ast creation for arguments node failed");
      else (yyval.ast) = a;
    }
#line 1762 "parser.tab.c"
    break;

  case 9: /* arguments: argument_list  */
#line 123 "build/src/parser/bison/parser.y"
                  { (yyval.ast) = (yyvsp[0].ast); }
#line 1768 "parser.tab.c"
    break;

  case 10: /* argument_list: evaluable  */
#line 127 "build/src/parser/bison/parser.y"
              {
      ast *a = ctx->ops.create_children_node_var(AST_TYPE_ARGUMENTS, 1, (yyvsp[0].ast));
      if (!a) { ctx->ops.destroy((yyvsp[0].ast)); (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                               AST_ERROR_CODE_ARGUMENTS_NODE_CREATION_FAILED,
                               "ast creation for arguments node failed"); }
      else (yyval.ast) = a;
    }
#line 1780 "parser.tab.c"
    break;

  case 11: /* argument_list: argument_list COMMA evaluable  */
#line 134 "build/src/parser/bison/parser.y"
                                  {
      if (!(yyvsp[-2].ast) || (yyvsp[-2].ast)->type == AST_TYPE_ERROR) { ctx->ops.destroy((yyvsp[0].ast)); (yyval.ast) = (yyvsp[-2].ast); }
      else if (ctx->ops.children_append_take((yyvsp[-2].ast), (yyvsp[0].ast))) (yyval.ast) = (yyvsp[-2].ast);
      else { ctx->ops.destroy((yyvsp[-2].ast)); ctx->ops.destroy((yyvsp[0].ast));
             (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_ARGUMENTS_APPEND_FAILED,
                    "ast append failed when adding an evaluable to a list of arguments"); }
    }
#line 1793 "parser.tab.c"
    break;

  case 12: /* atom: number_atom  */
#line 147 "build/src/parser/bison/parser.y"
                { (yyval.ast) = (yyvsp[0].ast); }
#line 1799 "parser.tab.c"
    break;

  case 13: /* atom: string_atom  */
#line 148 "build/src/parser/bison/parser.y"
                { (yyval.ast) = (yyvsp[0].ast); }
#line 1805 "parser.tab.c"
    break;

  case 14: /* atom: symbol_name_atom  */
#line 149 "build/src/parser/bison/parser.y"
                     { (yyval.ast) = (yyvsp[0].ast); }
#line 1811 "parser.tab.c"
    break;

  case 15: /* binding: symbol_name_atom EQUAL evaluable SEMICOLON  */
#line 155 "build/src/parser/bison/parser.y"
=======
#line 1749 "parser.tab.c"
    break;

  case 6: /* readable: evaluable  */
#line 108 "build/src/parser/bison/parser.y"
                {(yyval.ast) = (yyvsp[0].ast);}
#line 1755 "parser.tab.c"
    break;

  case 7: /* readable: block  */
#line 109 "build/src/parser/bison/parser.y"
            {(yyval.ast) = (yyvsp[0].ast);}
#line 1761 "parser.tab.c"
    break;

  case 8: /* atom: number_atom  */
#line 115 "build/src/parser/bison/parser.y"
                { (yyval.ast) = (yyvsp[0].ast); }
#line 1767 "parser.tab.c"
    break;

  case 9: /* atom: string_atom  */
#line 116 "build/src/parser/bison/parser.y"
                { (yyval.ast) = (yyvsp[0].ast); }
#line 1773 "parser.tab.c"
    break;

  case 10: /* atom: symbol_name_atom  */
#line 117 "build/src/parser/bison/parser.y"
                     { (yyval.ast) = (yyvsp[0].ast); }
#line 1779 "parser.tab.c"
    break;

  case 11: /* binding: symbol_name_atom EQUAL evaluable SEMICOLON  */
#line 123 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                                               {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_BINDING,
            2,
            (yyvsp[-3].ast),
            (yyvsp[-1].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-3].ast));
            ctx->ops.destroy((yyvsp[-1].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED,
                "ast creation for a binding node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 1832 "parser.tab.c"
    break;

  case 16: /* block_items: %empty  */
#line 176 "build/src/parser/bison/parser.y"
=======
#line 1800 "parser.tab.c"
    break;

  case 12: /* block_items: %empty  */
#line 144 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
           {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_BLOCK_ITEMS,
            0 );
        if (!a) {
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_BLOCK_ITEMS_NODE_CREATION_FAILED,
                "ast creation for the content of a block node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 1849 "parser.tab.c"
    break;

  case 17: /* block_items: block_items statement  */
#line 188 "build/src/parser/bison/parser.y"
=======
#line 1817 "parser.tab.c"
    break;

  case 13: /* block_items: block_items statement  */
#line 156 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                          {
        if ((!(yyvsp[-1].ast)) || ((yyvsp[-1].ast)->type == AST_TYPE_ERROR)) {
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = (yyvsp[-1].ast);
        } else {
            if (ctx->ops.children_append_take((yyvsp[-1].ast), (yyvsp[0].ast))) {
                (yyval.ast) = (yyvsp[-1].ast);
            } else {
                ctx->ops.destroy((yyvsp[-1].ast));
                ctx->ops.destroy((yyvsp[0].ast));
                (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_BLOCK_ITEMS_APPEND_FAILED,
                    "ast append failed when adding a statement to the content of a block" );
            }
        }
  }
<<<<<<< HEAD
#line 1870 "parser.tab.c"
    break;

  case 18: /* block: LBRACE block_items RBRACE  */
#line 209 "build/src/parser/bison/parser.y"
=======
#line 1838 "parser.tab.c"
    break;

  case 14: /* block: LBRACE block_items RBRACE  */
#line 177 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                              {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_BLOCK,
            1,
            (yyvsp[-1].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-1].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_BLOCK_NODE_CREATION_FAILED,
                "ast creation for a block node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 1889 "parser.tab.c"
    break;

  case 19: /* computable: SUBTRACT computable  */
#line 228 "build/src/parser/bison/parser.y"
                                       {
=======
#line 1857 "parser.tab.c"
    break;

  case 15: /* computable_core: SUBTRACT computable_core  */
#line 196 "build/src/parser/bison/parser.y"
                                            {
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
        ast *a = ctx->ops.create_children_node_var(
           AST_TYPE_NEGATION,
           1,
           (yyvsp[0].ast) );
        if (!a) {
           ctx->ops.destroy((yyvsp[0].ast));
           (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
               AST_ERROR_CODE_NEGATION_NODE_CREATION_FAILED,
               "ast creation for a negation node failed");
        } else {
           (yyval.ast) = a;
        }
    }
<<<<<<< HEAD
#line 1908 "parser.tab.c"
    break;

  case 20: /* computable: computable ADD computable  */
#line 242 "build/src/parser/bison/parser.y"
                                {
=======
#line 1876 "parser.tab.c"
    break;

  case 16: /* computable_core: computable_core ADD computable_core  */
#line 210 "build/src/parser/bison/parser.y"
                                          {
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_ADDITION,
            2,
            (yyvsp[-2].ast),
            (yyvsp[0].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-2].ast));
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_ADDITION_NODE_CREATION_FAILED,
                "ast creation for a addition node failed" );
        } else {
            (yyval.ast) = a;
        }
     }
<<<<<<< HEAD
#line 1929 "parser.tab.c"
    break;

  case 21: /* computable: computable SUBTRACT computable  */
#line 258 "build/src/parser/bison/parser.y"
                                     {
=======
#line 1897 "parser.tab.c"
    break;

  case 17: /* computable_core: computable_core SUBTRACT computable_core  */
#line 226 "build/src/parser/bison/parser.y"
                                               {
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
         ast *a = ctx->ops.create_children_node_var(
             AST_TYPE_SUBTRACTION,
             2,
             (yyvsp[-2].ast),
             (yyvsp[0].ast) );
         if (!a) {
             ctx->ops.destroy((yyvsp[-2].ast));
             ctx->ops.destroy((yyvsp[0].ast));
             (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                 AST_ERROR_CODE_SUBTRACTION_NODE_CREATION_FAILED,
                 "ast creation for a subtraction node failed" );
         } else {
             (yyval.ast) = a;
         }
     }
<<<<<<< HEAD
#line 1950 "parser.tab.c"
    break;

  case 22: /* computable: computable MULTIPLY computable  */
#line 274 "build/src/parser/bison/parser.y"
                                     {
=======
#line 1918 "parser.tab.c"
    break;

  case 18: /* computable_core: computable_core MULTIPLY computable_core  */
#line 242 "build/src/parser/bison/parser.y"
                                               {
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
          ast *a = ctx->ops.create_children_node_var(
              AST_TYPE_MULTIPLICATION,
              2,
              (yyvsp[-2].ast),
              (yyvsp[0].ast) );
          if (!a) {
              ctx->ops.destroy((yyvsp[-2].ast));
              ctx->ops.destroy((yyvsp[0].ast));
              (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                  AST_ERROR_CODE_MULTIPLICATION_NODE_CREATION_FAILED,
                  "ast creation for a multiplication node failed" );
          } else {
              (yyval.ast) = a;
          }
      }
<<<<<<< HEAD
#line 1971 "parser.tab.c"
    break;

  case 23: /* computable: computable DIVIDE computable  */
#line 290 "build/src/parser/bison/parser.y"
                                   {
=======
#line 1939 "parser.tab.c"
    break;

  case 19: /* computable_core: computable_core DIVIDE computable_core  */
#line 258 "build/src/parser/bison/parser.y"
                                             {
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
          ast *a = ctx->ops.create_children_node_var(
              AST_TYPE_DIVISION,
              2,
              (yyvsp[-2].ast),
              (yyvsp[0].ast) );
          if (!a) {
              ctx->ops.destroy((yyvsp[-2].ast));
              ctx->ops.destroy((yyvsp[0].ast));
              (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                  AST_ERROR_CODE_DIVISION_NODE_CREATION_FAILED,
                  "ast creation for a division node failed" );
          } else {
              (yyval.ast) = a;
          }
      }
<<<<<<< HEAD
#line 1992 "parser.tab.c"
    break;

  case 24: /* computable: LPAREN computable RPAREN  */
#line 306 "build/src/parser/bison/parser.y"
                               {
          (yyval.ast) = (yyvsp[-1].ast);
      }
#line 2000 "parser.tab.c"
    break;

  case 25: /* computable: atom  */
#line 309 "build/src/parser/bison/parser.y"
           {
          (yyval.ast) = (yyvsp[0].ast);
      }
#line 2008 "parser.tab.c"
    break;

  case 26: /* eval: EVAL symbol_name_atom SEMICOLON  */
#line 318 "build/src/parser/bison/parser.y"
=======
#line 1960 "parser.tab.c"
    break;

  case 20: /* computable_core: LPAREN computable_core RPAREN  */
#line 274 "build/src/parser/bison/parser.y"
                                    {
          (yyval.ast) = (yyvsp[-1].ast);
      }
#line 1968 "parser.tab.c"
    break;

  case 21: /* computable_core: number_atom  */
#line 277 "build/src/parser/bison/parser.y"
                  {
          (yyval.ast) = (yyvsp[0].ast);
      }
#line 1976 "parser.tab.c"
    break;

  case 22: /* computable_core: symbol_name_atom  */
#line 280 "build/src/parser/bison/parser.y"
                       {
          (yyval.ast) = (yyvsp[0].ast);
      }
#line 1984 "parser.tab.c"
    break;

  case 23: /* computable: SUBTRACT computable_core  */
#line 286 "build/src/parser/bison/parser.y"
                                            {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_NEGATION,
            1,
            (yyvsp[0].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_NEGATION_NODE_CREATION_FAILED,
                "ast creation for a negation node failed");
        } else {
            (yyval.ast) = a;
        }
    }
#line 2003 "parser.tab.c"
    break;

  case 24: /* computable: computable_core ADD computable_core  */
#line 300 "build/src/parser/bison/parser.y"
                                          {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_ADDITION,
            2,
            (yyvsp[-2].ast),
            (yyvsp[0].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-2].ast));
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_ADDITION_NODE_CREATION_FAILED,
                "ast creation for a addition node failed" );
        } else {
            (yyval.ast) = a;
        }
     }
#line 2024 "parser.tab.c"
    break;

  case 25: /* computable: computable_core SUBTRACT computable_core  */
#line 316 "build/src/parser/bison/parser.y"
                                               {
         ast *a = ctx->ops.create_children_node_var(
             AST_TYPE_SUBTRACTION,
             2,
             (yyvsp[-2].ast),
             (yyvsp[0].ast) );
         if (!a) {
             ctx->ops.destroy((yyvsp[-2].ast));
             ctx->ops.destroy((yyvsp[0].ast));
             (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                 AST_ERROR_CODE_SUBTRACTION_NODE_CREATION_FAILED,
                 "ast creation for a subtraction node failed" );
         } else {
             (yyval.ast) = a;
         }
     }
#line 2045 "parser.tab.c"
    break;

  case 26: /* computable: computable_core MULTIPLY computable_core  */
#line 332 "build/src/parser/bison/parser.y"
                                               {
          ast *a = ctx->ops.create_children_node_var(
              AST_TYPE_MULTIPLICATION,
              2,
              (yyvsp[-2].ast),
              (yyvsp[0].ast) );
          if (!a) {
              ctx->ops.destroy((yyvsp[-2].ast));
              ctx->ops.destroy((yyvsp[0].ast));
              (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                  AST_ERROR_CODE_MULTIPLICATION_NODE_CREATION_FAILED,
                  "ast creation for a multiplication node failed" );
          } else {
              (yyval.ast) = a;
          }
      }
#line 2066 "parser.tab.c"
    break;

  case 27: /* computable: computable_core DIVIDE computable_core  */
#line 348 "build/src/parser/bison/parser.y"
                                             {
          ast *a = ctx->ops.create_children_node_var(
              AST_TYPE_DIVISION,
              2,
              (yyvsp[-2].ast),
              (yyvsp[0].ast) );
          if (!a) {
              ctx->ops.destroy((yyvsp[-2].ast));
              ctx->ops.destroy((yyvsp[0].ast));
              (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                  AST_ERROR_CODE_DIVISION_NODE_CREATION_FAILED,
                  "ast creation for a division node failed" );
          } else {
              (yyval.ast) = a;
          }
      }
#line 2087 "parser.tab.c"
    break;

  case 28: /* computable: LPAREN computable_core RPAREN  */
#line 364 "build/src/parser/bison/parser.y"
                                    {
          (yyval.ast) = (yyvsp[-1].ast);
      }
#line 2095 "parser.tab.c"
    break;

  case 29: /* eval: EVAL symbol_name_atom SEMICOLON  */
#line 372 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                                    {
        ast *a = ctx->ops.create_children_node_var(
                            AST_TYPE_EVAL,
                            1,
                            (yyvsp[-1].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-1].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_EVAL_NODE_CREATION_FAILED,
                "ast creation for an eval node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2027 "parser.tab.c"
    break;

  case 27: /* evaluable: function_call  */
#line 337 "build/src/parser/bison/parser.y"
                  { (yyval.ast) = (yyvsp[0].ast); }
#line 2033 "parser.tab.c"
    break;

  case 28: /* evaluable: computable  */
#line 338 "build/src/parser/bison/parser.y"
               { (yyval.ast) = (yyvsp[0].ast); }
#line 2039 "parser.tab.c"
    break;

  case 29: /* evaluable: QUOTE evaluable  */
#line 339 "build/src/parser/bison/parser.y"
=======
#line 2114 "parser.tab.c"
    break;

  case 30: /* evaluable: function_call  */
#line 391 "build/src/parser/bison/parser.y"
                  { (yyval.ast) = (yyvsp[0].ast); }
#line 2120 "parser.tab.c"
    break;

  case 31: /* evaluable: atom  */
#line 392 "build/src/parser/bison/parser.y"
         { (yyval.ast) = (yyvsp[0].ast); }
#line 2126 "parser.tab.c"
    break;

  case 32: /* evaluable: computable  */
#line 393 "build/src/parser/bison/parser.y"
               { (yyval.ast) = (yyvsp[0].ast); }
#line 2132 "parser.tab.c"
    break;

  case 33: /* evaluable: QUOTE evaluable  */
#line 394 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                    {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_QUOTE,
            1,
            (yyvsp[0].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_QUOTE_NODE_CREATION_FAILED,
                "ast creation for a quote node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2058 "parser.tab.c"
    break;

  case 30: /* function_call: symbol_name_atom list_of_arguments  */
#line 358 "build/src/parser/bison/parser.y"
                                       {
=======
#line 2151 "parser.tab.c"
    break;

  case 34: /* function_call: symbol_name_atom list_of_numbers  */
#line 413 "build/src/parser/bison/parser.y"
                                     {
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_FUNCTION_CALL,
            2,
            (yyvsp[-1].ast),
            (yyvsp[0].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-1].ast));
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_FUNCTION_CALL_NODE_CREATION_FAILED,
                "ast creation for a function call node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2079 "parser.tab.c"
    break;

  case 31: /* function_definition: DEFINE function  */
#line 379 "build/src/parser/bison/parser.y"
=======
#line 2172 "parser.tab.c"
    break;

  case 35: /* function_definition: DEFINE function  */
#line 434 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                    {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_FUNCTION_DEFINITION,
            1,
            (yyvsp[0].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_FUNCTION_DEFINITION_NODE_CREATION_FAILED,
                "ast creation for a function definition node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2098 "parser.tab.c"
    break;

  case 32: /* function: symbol_name_atom list_of_parameters block  */
#line 398 "build/src/parser/bison/parser.y"
=======
#line 2191 "parser.tab.c"
    break;

  case 36: /* function: symbol_name_atom list_of_parameters block  */
#line 453 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                                              {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_FUNCTION,
            3,
            (yyvsp[-2].ast),
            (yyvsp[-1].ast),
            (yyvsp[0].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-2].ast));
            ctx->ops.destroy((yyvsp[-1].ast));
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_FUNCTION_NODE_CREATION_FAILED,
                "ast creation for a function node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2121 "parser.tab.c"
    break;

  case 33: /* list_of_arguments: LPAREN arguments RPAREN  */
#line 421 "build/src/parser/bison/parser.y"
                            {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_LIST_OF_ARGUMENTS,
=======
#line 2214 "parser.tab.c"
    break;

  case 37: /* list_of_numbers: LPAREN numbers RPAREN  */
#line 476 "build/src/parser/bison/parser.y"
                          {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_LIST_OF_NUMBERS,
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
            1,
            (yyvsp[-1].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-1].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
<<<<<<< HEAD
                AST_ERROR_CODE_LIST_OF_ARGUMENTS_NODE_CREATION_FAILED,
                "ast creation for a node for a list of arguments failed" );
=======
                AST_ERROR_CODE_LIST_OF_NUMBERS_NODE_CREATION_FAILED,
                "ast creation for a node for a list of numbers failed" );
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2140 "parser.tab.c"
    break;

  case 34: /* list_of_parameters: LPAREN parameters RPAREN  */
#line 440 "build/src/parser/bison/parser.y"
=======
#line 2233 "parser.tab.c"
    break;

  case 38: /* list_of_parameters: LPAREN parameters RPAREN  */
#line 495 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                             {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_LIST_OF_PARAMETERS,
            1,
            (yyvsp[-1].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-1].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_LIST_OF_PARAMETERS_NODE_CREATION_FAILED,
                "ast creation for a node for a list of parameters failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2159 "parser.tab.c"
    break;

  case 35: /* number_atom: INTEGER  */
#line 459 "build/src/parser/bison/parser.y"
=======
#line 2252 "parser.tab.c"
    break;

  case 39: /* number_atom: INTEGER  */
#line 514 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
            {
        ast *a = ctx->ops.create_int_node((yyvsp[0].int_value));
        (yyval.ast) = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_INT_NODE_CREATION_FAILED,
                    "ast creation for a number failed" );
    }
<<<<<<< HEAD
#line 2172 "parser.tab.c"
    break;

  case 36: /* parameters: %empty  */
#line 472 "build/src/parser/bison/parser.y"
=======
#line 2265 "parser.tab.c"
    break;

  case 40: /* numbers: %empty  */
#line 527 "build/src/parser/bison/parser.y"
           {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_NUMBERS,
            0 );
        if (!a) {
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_NUMBERS_NODE_CREATION_FAILED,
                "ast creation for numbers node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
#line 2282 "parser.tab.c"
    break;

  case 41: /* numbers: numbers number_atom  */
#line 539 "build/src/parser/bison/parser.y"
                        {
        if ((!(yyvsp[-1].ast)) || ((yyvsp[-1].ast)->type == AST_TYPE_ERROR)) {
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = (yyvsp[-1].ast);
        } else {
            if (ctx->ops.children_append_take((yyvsp[-1].ast), (yyvsp[0].ast))) {
                (yyval.ast) = (yyvsp[-1].ast);
            } else {
                ctx->ops.destroy((yyvsp[-1].ast));
                ctx->ops.destroy((yyvsp[0].ast));
                (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_NUMBERS_APPEND_FAILED,
                    "ast append failed when adding a number to a list of numbers" );
            }
        }
  }
#line 2303 "parser.tab.c"
    break;

  case 42: /* parameters: %empty  */
#line 593 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
           {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_PARAMETERS,
            0 );
        if (!a) {
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_PARAMETERS_NODE_CREATION_FAILED,
                "ast creation for parameters node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2189 "parser.tab.c"
    break;

  case 37: /* parameters: parameters symbol_name_atom  */
#line 484 "build/src/parser/bison/parser.y"
=======
#line 2320 "parser.tab.c"
    break;

  case 43: /* parameters: parameters symbol_name_atom  */
#line 605 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                                {
        if ((!(yyvsp[-1].ast)) || ((yyvsp[-1].ast)->type == AST_TYPE_ERROR)) {
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = (yyvsp[-1].ast);
        } else {
            if (ctx->ops.children_append_take((yyvsp[-1].ast), (yyvsp[0].ast))) {
                (yyval.ast) = (yyvsp[-1].ast);
            } else {
                ctx->ops.destroy((yyvsp[-1].ast));
                ctx->ops.destroy((yyvsp[0].ast));
                (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_PARAMETERS_APPEND_FAILED,
                    "ast append failed when adding a parameter to a list of parameters" );
            }
        }
  }
<<<<<<< HEAD
#line 2210 "parser.tab.c"
    break;

  case 38: /* reading: READ symbol_name_atom SEMICOLON  */
#line 505 "build/src/parser/bison/parser.y"
=======
#line 2341 "parser.tab.c"
    break;

  case 44: /* reading: READ symbol_name_atom SEMICOLON  */
#line 626 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                                    {
        ast *a = ctx->ops.create_children_node_var(
                    AST_TYPE_READING,
                    1,
                    (yyvsp[-1].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-1].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_READING_NODE_CREATION_FAILED,
                "ast creation for a reading node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2229 "parser.tab.c"
    break;

  case 39: /* statement: binding  */
#line 524 "build/src/parser/bison/parser.y"
              { (yyval.ast) = (yyvsp[0].ast); }
#line 2235 "parser.tab.c"
    break;

  case 40: /* statement: writing  */
#line 525 "build/src/parser/bison/parser.y"
              { (yyval.ast) = (yyvsp[0].ast); }
#line 2241 "parser.tab.c"
    break;

  case 41: /* statement: reading  */
#line 526 "build/src/parser/bison/parser.y"
              { (yyval.ast) = (yyvsp[0].ast); }
#line 2247 "parser.tab.c"
    break;

  case 42: /* statement: function_definition  */
#line 527 "build/src/parser/bison/parser.y"
                          { (yyval.ast) = (yyvsp[0].ast); }
#line 2253 "parser.tab.c"
    break;

  case 43: /* statement: evaluable SEMICOLON  */
#line 528 "build/src/parser/bison/parser.y"
                          { (yyval.ast) = (yyvsp[-1].ast); }
#line 2259 "parser.tab.c"
    break;

  case 45: /* string_atom: STRING  */
#line 535 "build/src/parser/bison/parser.y"
=======
#line 2360 "parser.tab.c"
    break;

  case 45: /* statement: binding  */
#line 645 "build/src/parser/bison/parser.y"
              { (yyval.ast) = (yyvsp[0].ast); }
#line 2366 "parser.tab.c"
    break;

  case 46: /* statement: writing  */
#line 646 "build/src/parser/bison/parser.y"
              { (yyval.ast) = (yyvsp[0].ast); }
#line 2372 "parser.tab.c"
    break;

  case 47: /* statement: reading  */
#line 647 "build/src/parser/bison/parser.y"
              { (yyval.ast) = (yyvsp[0].ast); }
#line 2378 "parser.tab.c"
    break;

  case 48: /* statement: function_definition  */
#line 648 "build/src/parser/bison/parser.y"
                          { (yyval.ast) = (yyvsp[0].ast); }
#line 2384 "parser.tab.c"
    break;

  case 49: /* statement: evaluable SEMICOLON  */
#line 649 "build/src/parser/bison/parser.y"
                          { (yyval.ast) = (yyvsp[-1].ast); }
#line 2390 "parser.tab.c"
    break;

  case 51: /* string_atom: STRING  */
#line 656 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
           {
        ast *a = ctx->ops.create_string_node((yyvsp[0].string_value));
        free((yyvsp[0].string_value));
        (yyval.ast) = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_STRING_NODE_CREATION_FAILED,
                    "ast creation for a string failed" );
    }
<<<<<<< HEAD
#line 2273 "parser.tab.c"
    break;

  case 46: /* symbol_name_atom: SYMBOL_NAME  */
#line 549 "build/src/parser/bison/parser.y"
=======
#line 2404 "parser.tab.c"
    break;

  case 52: /* symbol_name_atom: SYMBOL_NAME  */
#line 670 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                {
        ast *a = ctx->ops.create_symbol_name_node((yyvsp[0].symbol_name_value));
        free((yyvsp[0].symbol_name_value));
        (yyval.ast) = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_SYMBOL_NAME_NODE_CREATION_FAILED,
                    "ast creation for a symbol name failed" );
  }
<<<<<<< HEAD
#line 2287 "parser.tab.c"
    break;

  case 47: /* translation_unit: %empty  */
#line 563 "build/src/parser/bison/parser.y"
=======
#line 2418 "parser.tab.c"
    break;

  case 53: /* translation_unit: %empty  */
#line 684 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
           {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_TRANSLATION_UNIT,
            0 );
        if (!a) {
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_TRANSLATION_UNIT_NODE_CREATION_FAILED,
                "ast creation for a translation unit node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2304 "parser.tab.c"
    break;

  case 48: /* translation_unit: translation_unit statement  */
#line 575 "build/src/parser/bison/parser.y"
=======
#line 2435 "parser.tab.c"
    break;

  case 54: /* translation_unit: translation_unit statement  */
#line 696 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                               {
        if ((!(yyvsp[-1].ast)) || ((yyvsp[-1].ast)->type == AST_TYPE_ERROR)) {
            ctx->ops.destroy((yyvsp[0].ast));
            (yyval.ast) = (yyvsp[-1].ast);
        } else {
            if (ctx->ops.children_append_take((yyvsp[-1].ast), (yyvsp[0].ast))) {
                (yyval.ast) = (yyvsp[-1].ast);
            } else {
                ctx->ops.destroy((yyvsp[-1].ast));
                ctx->ops.destroy((yyvsp[0].ast));
                (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_TRANSLATION_UNIT_APPEND_FAILED,
                    "ast append failed when adding a statement to the translation unit" );
            }
        }
  }
<<<<<<< HEAD
#line 2325 "parser.tab.c"
    break;

  case 49: /* writing: WRITE evaluable SEMICOLON  */
#line 596 "build/src/parser/bison/parser.y"
=======
#line 2456 "parser.tab.c"
    break;

  case 55: /* writing: WRITE evaluable SEMICOLON  */
#line 717 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD
                              {
        ast *a = ctx->ops.create_children_node_var(
                            AST_TYPE_WRITING,
                            1,
                            (yyvsp[-1].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-1].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_WRITING_NODE_CREATION_FAILED,
                "ast creation for a writing node failed" );
        } else {
            (yyval.ast) = a;
        }
  }
<<<<<<< HEAD
#line 2344 "parser.tab.c"
    break;


#line 2348 "parser.tab.c"
=======
#line 2475 "parser.tab.c"
    break;


#line 2479 "parser.tab.c"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (scanner, result, ctx, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= END)
        {
          /* Return failure if at end of input.  */
          if (yychar == END)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, scanner, result, ctx);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, scanner, result, ctx);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, result, ctx, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner, result, ctx);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, scanner, result, ctx);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

<<<<<<< HEAD
#line 614 "build/src/parser/bison/parser.y"
=======
#line 735 "build/src/parser/bison/parser.y"
>>>>>>> feature/interpreter/AST_TYPE_FUNCTION_CALL_TDD


int yyerror(yyscan_t scanner, ast **result, struct parser_ctx *ctx, const char *msg) {
    (void)result;
    if (!((lexer_extra_t*) yyget_extra(scanner))->reached_input_end) ctx->syntax_errors++;
    fprintf(stderr, "Parse error: %s\n", msg);
    return 1;
}

/* @formatter:on */
