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
#line 18 "build/src/parser/bison/parser.y"

#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "parser_ctx.h"

#line 78 "parser.tab.c"

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
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 13 "build/src/parser/bison/parser.y"

#include "parser_types.h"
struct parser_ctx;

#line 115 "parser.tab.c"

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
    EQUAL = 270,                   /* EQUAL  */
    READ = 271,                    /* READ  */
    WRITE = 272,                   /* WRITE  */
    DEFINE = 273,                  /* DEFINE  */
    CALL = 274,                    /* CALL  */
    QUOTE = 275,                   /* QUOTE  */
    UMINUS = 276                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 30 "build/src/parser/bison/parser.y"

    int int_value;
    char *string_value;
    char *symbol_name_value;
ast *ast;

#line 160 "parser.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (yyscan_t scanner, ast **result, struct parser_ctx *ctx);

/* "%code provides" blocks.  */
#line 25 "build/src/parser/bison/parser.y"

int yyerror(yyscan_t scanner, ast **result, struct parser_ctx *ctx, const char *s);
int yylex(YYSTYPE *yylval, yyscan_t scanner);

#line 179 "parser.tab.c"


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
  YYSYMBOL_EQUAL = 15,                     /* EQUAL  */
  YYSYMBOL_READ = 16,                      /* READ  */
  YYSYMBOL_WRITE = 17,                     /* WRITE  */
  YYSYMBOL_DEFINE = 18,                    /* DEFINE  */
  YYSYMBOL_CALL = 19,                      /* CALL  */
  YYSYMBOL_QUOTE = 20,                     /* QUOTE  */
  YYSYMBOL_UMINUS = 21,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 22,                  /* $accept  */
  YYSYMBOL_start = 23,                     /* start  */
  YYSYMBOL_atom = 24,                      /* atom  */
  YYSYMBOL_binding = 25,                   /* binding  */
  YYSYMBOL_block_items = 26,               /* block_items  */
  YYSYMBOL_block = 27,                     /* block  */
  YYSYMBOL_computable_core = 28,           /* computable_core  */
  YYSYMBOL_computable = 29,                /* computable  */
  YYSYMBOL_evaluable = 30,                 /* evaluable  */
  YYSYMBOL_function_call = 31,             /* function_call  */
  YYSYMBOL_function_definition = 32,       /* function_definition  */
  YYSYMBOL_function = 33,                  /* function  */
  YYSYMBOL_list_of_numbers = 34,           /* list_of_numbers  */
  YYSYMBOL_list_of_parameters = 35,        /* list_of_parameters  */
  YYSYMBOL_number_atom = 36,               /* number_atom  */
  YYSYMBOL_numbers = 37,                   /* numbers  */
  YYSYMBOL_parameters = 38,                /* parameters  */
  YYSYMBOL_reading = 39,                   /* reading  */
  YYSYMBOL_statement = 40,                 /* statement  */
  YYSYMBOL_string_atom = 41,               /* string_atom  */
  YYSYMBOL_symbol_name_atom = 42,          /* symbol_name_atom  */
  YYSYMBOL_translation_unit = 43,          /* translation_unit  */
  YYSYMBOL_writing = 44                    /* writing  */
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

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   88

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  22
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  48
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  79

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   276


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
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    70,    70,    76,    77,    78,    84,   105,   117,   138,
     157,   171,   187,   203,   219,   235,   238,   241,   247,   261,
     277,   293,   309,   325,   333,   334,   335,   336,   355,   376,
     395,   418,   437,   456,   469,   481,   502,   514,   535,   554,
     555,   556,   557,   558,   564,   577,   590,   602,   623
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INTEGER", "STRING",
  "SYMBOL_NAME", "LBRACE", "RBRACE", "LPAREN", "RPAREN", "ADD", "SUBTRACT",
  "MULTIPLY", "DIVIDE", "SEMICOLON", "EQUAL", "READ", "WRITE", "DEFINE",
  "CALL", "QUOTE", "UMINUS", "$accept", "start", "atom", "binding",
  "block_items", "block", "computable_core", "computable", "evaluable",
  "function_call", "function_definition", "function", "list_of_numbers",
  "list_of_parameters", "number_atom", "numbers", "parameters", "reading",
  "statement", "string_atom", "symbol_name_atom", "translation_unit",
  "writing", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-30)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-24)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -30,    28,    24,   -30,   -30,    43,    43,    43,   -30,    40,
     -30,   -30,   -30,    35,   -30,    42,    65,   -30,    49,   -30,
     -30,     3,   -30,   -30,   -30,   -30,    74,     6,   -30,   -30,
      41,    41,     3,   -30,    59,   -30,    67,   -30,    68,   -30,
      39,    46,   -30,   -30,   -30,   -30,    41,    41,    50,   -30,
     -30,    69,   -30,    41,    41,    41,    41,   -30,   -30,   -30,
      15,    55,   -30,    70,    41,    41,    41,    41,    61,    64,
      71,    72,   -30,   -30,   -30,     0,     0,   -30,   -30
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
      46,     0,     2,     1,    45,     0,     0,     0,    39,     0,
      42,    41,    47,     0,    40,     0,     0,    29,     0,    43,
      34,     0,    28,    38,    48,    36,     0,     0,    33,    44,
       0,     0,     0,    25,     0,    26,     0,    24,    16,     4,
      17,     0,     7,    30,    31,    35,     0,     0,     0,    16,
      17,    10,    27,     0,     0,     0,     0,     6,    32,    37,
       0,     0,    10,    15,     0,     0,     0,     0,    11,    12,
      13,    14,     9,     8,    15,    11,    12,    13,    14
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -30,   -30,   -30,   -30,   -30,   -30,   -29,   -30,    56,    13,
     -30,   -30,   -30,   -30,   -11,   -30,   -30,   -30,    27,   -30,
      -2,   -30,   -30
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,    33,     8,    60,    43,    34,    35,    36,     9,
      10,    17,    22,    26,    49,    27,    41,    11,    12,    39,
      50,     2,    14
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      13,    48,    51,    15,    16,    18,    28,    29,     4,    28,
      38,    30,    66,    67,    31,    44,    45,    61,    62,    40,
       4,    38,    72,    32,    68,    69,    70,    71,     3,     4,
      40,     5,     6,     7,    37,    75,    76,    77,    78,    59,
       5,     6,     7,    20,    28,    37,     4,    20,     4,    46,
      21,     4,    47,    -5,    19,    58,    23,    25,    13,    63,
      64,    65,    66,    67,    74,    64,    65,    66,    67,    53,
      54,    55,    56,    66,    67,   -19,    66,    67,   -20,    24,
      42,    57,    -3,   -18,   -23,   -21,   -22,    73,    52
};

static const yytype_int8 yycheck[] =
{
       2,    30,    31,     5,     6,     7,     3,     4,     5,     3,
      21,     8,    12,    13,    11,     9,    27,    46,    47,    21,
       5,    32,     7,    20,    53,    54,    55,    56,     0,     5,
      32,    16,    17,    18,    21,    64,    65,    66,    67,    41,
      16,    17,    18,     8,     3,    32,     5,     8,     5,     8,
      15,     5,    11,    14,    14,     9,    14,     8,    60,     9,
      10,    11,    12,    13,     9,    10,    11,    12,    13,    10,
      11,    12,    13,    12,    13,    14,    12,    13,    14,    14,
       6,    14,    14,    14,    14,    14,    14,    60,    32
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    23,    43,     0,     5,    16,    17,    18,    25,    31,
      32,    39,    40,    42,    44,    42,    42,    33,    42,    14,
       8,    15,    34,    14,    14,     8,    35,    37,     3,     4,
       8,    11,    20,    24,    28,    29,    30,    31,    36,    41,
      42,    38,     6,    27,     9,    36,     8,    11,    28,    36,
      42,    28,    30,    10,    11,    12,    13,    14,     9,    42,
      26,    28,    28,     9,    10,    11,    12,    13,    28,    28,
      28,    28,     7,    40,     9,    28,    28,    28,    28
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    22,    23,    24,    24,    24,    25,    26,    26,    27,
      28,    28,    28,    28,    28,    28,    28,    28,    29,    29,
      29,    29,    29,    29,    30,    30,    30,    30,    31,    32,
      33,    34,    35,    36,    37,    37,    38,    38,    39,    40,
      40,    40,    40,    40,    41,    42,    43,    43,    44
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     4,     0,     2,     3,
       2,     3,     3,     3,     3,     3,     1,     1,     2,     3,
       3,     3,     3,     3,     1,     1,     1,     2,     2,     2,
       3,     3,     3,     1,     0,     2,     0,     2,     3,     1,
       1,     1,     1,     2,     1,     1,     0,     2,     3
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
#line 39 "build/src/parser/bison/parser.y"
            { free(((*yyvaluep).string_value)); }
#line 983 "parser.tab.c"
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

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
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
  case 2: /* start: translation_unit  */
#line 70 "build/src/parser/bison/parser.y"
                  { *result = (yyvsp[0].ast); }
#line 1259 "parser.tab.c"
    break;

  case 3: /* atom: number_atom  */
#line 76 "build/src/parser/bison/parser.y"
                { (yyval.ast) = (yyvsp[0].ast); }
#line 1265 "parser.tab.c"
    break;

  case 4: /* atom: string_atom  */
#line 77 "build/src/parser/bison/parser.y"
                { (yyval.ast) = (yyvsp[0].ast); }
#line 1271 "parser.tab.c"
    break;

  case 5: /* atom: symbol_name_atom  */
#line 78 "build/src/parser/bison/parser.y"
                     { (yyval.ast) = (yyvsp[0].ast); }
#line 1277 "parser.tab.c"
    break;

  case 6: /* binding: symbol_name_atom EQUAL evaluable SEMICOLON  */
#line 84 "build/src/parser/bison/parser.y"
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
#line 1298 "parser.tab.c"
    break;

  case 7: /* block_items: %empty  */
#line 105 "build/src/parser/bison/parser.y"
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
#line 1315 "parser.tab.c"
    break;

  case 8: /* block_items: block_items statement  */
#line 117 "build/src/parser/bison/parser.y"
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
#line 1336 "parser.tab.c"
    break;

  case 9: /* block: LBRACE block_items RBRACE  */
#line 138 "build/src/parser/bison/parser.y"
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
#line 1355 "parser.tab.c"
    break;

  case 10: /* computable_core: SUBTRACT computable_core  */
#line 157 "build/src/parser/bison/parser.y"
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
#line 1374 "parser.tab.c"
    break;

  case 11: /* computable_core: computable_core ADD computable_core  */
#line 171 "build/src/parser/bison/parser.y"
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
#line 1395 "parser.tab.c"
    break;

  case 12: /* computable_core: computable_core SUBTRACT computable_core  */
#line 187 "build/src/parser/bison/parser.y"
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
#line 1416 "parser.tab.c"
    break;

  case 13: /* computable_core: computable_core MULTIPLY computable_core  */
#line 203 "build/src/parser/bison/parser.y"
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
#line 1437 "parser.tab.c"
    break;

  case 14: /* computable_core: computable_core DIVIDE computable_core  */
#line 219 "build/src/parser/bison/parser.y"
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
#line 1458 "parser.tab.c"
    break;

  case 15: /* computable_core: LPAREN computable_core RPAREN  */
#line 235 "build/src/parser/bison/parser.y"
                                    {
          (yyval.ast) = (yyvsp[-1].ast);
      }
#line 1466 "parser.tab.c"
    break;

  case 16: /* computable_core: number_atom  */
#line 238 "build/src/parser/bison/parser.y"
                  {
          (yyval.ast) = (yyvsp[0].ast);
      }
#line 1474 "parser.tab.c"
    break;

  case 17: /* computable_core: symbol_name_atom  */
#line 241 "build/src/parser/bison/parser.y"
                       {
          (yyval.ast) = (yyvsp[0].ast);
      }
#line 1482 "parser.tab.c"
    break;

  case 18: /* computable: SUBTRACT computable_core  */
#line 247 "build/src/parser/bison/parser.y"
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
#line 1501 "parser.tab.c"
    break;

  case 19: /* computable: computable_core ADD computable_core  */
#line 261 "build/src/parser/bison/parser.y"
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
#line 1522 "parser.tab.c"
    break;

  case 20: /* computable: computable_core SUBTRACT computable_core  */
#line 277 "build/src/parser/bison/parser.y"
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
#line 1543 "parser.tab.c"
    break;

  case 21: /* computable: computable_core MULTIPLY computable_core  */
#line 293 "build/src/parser/bison/parser.y"
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
#line 1564 "parser.tab.c"
    break;

  case 22: /* computable: computable_core DIVIDE computable_core  */
#line 309 "build/src/parser/bison/parser.y"
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
#line 1585 "parser.tab.c"
    break;

  case 23: /* computable: LPAREN computable_core RPAREN  */
#line 325 "build/src/parser/bison/parser.y"
                                    {
          (yyval.ast) = (yyvsp[-1].ast);
      }
#line 1593 "parser.tab.c"
    break;

  case 24: /* evaluable: function_call  */
#line 333 "build/src/parser/bison/parser.y"
                  { (yyval.ast) = (yyvsp[0].ast); }
#line 1599 "parser.tab.c"
    break;

  case 25: /* evaluable: atom  */
#line 334 "build/src/parser/bison/parser.y"
         { (yyval.ast) = (yyvsp[0].ast); }
#line 1605 "parser.tab.c"
    break;

  case 26: /* evaluable: computable  */
#line 335 "build/src/parser/bison/parser.y"
               { (yyval.ast) = (yyvsp[0].ast); }
#line 1611 "parser.tab.c"
    break;

  case 27: /* evaluable: QUOTE evaluable  */
#line 336 "build/src/parser/bison/parser.y"
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
#line 1630 "parser.tab.c"
    break;

  case 28: /* function_call: symbol_name_atom list_of_numbers  */
#line 355 "build/src/parser/bison/parser.y"
                                     {
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
#line 1651 "parser.tab.c"
    break;

  case 29: /* function_definition: DEFINE function  */
#line 376 "build/src/parser/bison/parser.y"
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
#line 1670 "parser.tab.c"
    break;

  case 30: /* function: symbol_name_atom list_of_parameters block  */
#line 395 "build/src/parser/bison/parser.y"
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
#line 1693 "parser.tab.c"
    break;

  case 31: /* list_of_numbers: LPAREN numbers RPAREN  */
#line 418 "build/src/parser/bison/parser.y"
                          {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_LIST_OF_NUMBERS,
            1,
            (yyvsp[-1].ast) );
        if (!a) {
            ctx->ops.destroy((yyvsp[-1].ast));
            (yyval.ast) = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_LIST_OF_NUMBERS_NODE_CREATION_FAILED,
                "ast creation for a node for a list of numbers failed" );
        } else {
            (yyval.ast) = a;
        }
  }
#line 1712 "parser.tab.c"
    break;

  case 32: /* list_of_parameters: LPAREN parameters RPAREN  */
#line 437 "build/src/parser/bison/parser.y"
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
#line 1731 "parser.tab.c"
    break;

  case 33: /* number_atom: INTEGER  */
#line 456 "build/src/parser/bison/parser.y"
            {
        ast *a = ctx->ops.create_int_node((yyvsp[0].int_value));
        (yyval.ast) = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_INT_NODE_CREATION_FAILED,
                    "ast creation for a number failed" );
    }
#line 1744 "parser.tab.c"
    break;

  case 34: /* numbers: %empty  */
#line 469 "build/src/parser/bison/parser.y"
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
#line 1761 "parser.tab.c"
    break;

  case 35: /* numbers: numbers number_atom  */
#line 481 "build/src/parser/bison/parser.y"
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
#line 1782 "parser.tab.c"
    break;

  case 36: /* parameters: %empty  */
#line 502 "build/src/parser/bison/parser.y"
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
#line 1799 "parser.tab.c"
    break;

  case 37: /* parameters: parameters symbol_name_atom  */
#line 514 "build/src/parser/bison/parser.y"
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
#line 1820 "parser.tab.c"
    break;

  case 38: /* reading: READ symbol_name_atom SEMICOLON  */
#line 535 "build/src/parser/bison/parser.y"
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
#line 1839 "parser.tab.c"
    break;

  case 39: /* statement: binding  */
#line 554 "build/src/parser/bison/parser.y"
            { (yyval.ast) = (yyvsp[0].ast); }
#line 1845 "parser.tab.c"
    break;

  case 40: /* statement: writing  */
#line 555 "build/src/parser/bison/parser.y"
            { (yyval.ast) = (yyvsp[0].ast); }
#line 1851 "parser.tab.c"
    break;

  case 41: /* statement: reading  */
#line 556 "build/src/parser/bison/parser.y"
            { (yyval.ast) = (yyvsp[0].ast); }
#line 1857 "parser.tab.c"
    break;

  case 42: /* statement: function_definition  */
#line 557 "build/src/parser/bison/parser.y"
                        { (yyval.ast) = (yyvsp[0].ast); }
#line 1863 "parser.tab.c"
    break;

  case 43: /* statement: function_call SEMICOLON  */
#line 558 "build/src/parser/bison/parser.y"
                            { (yyval.ast) = (yyvsp[-1].ast); }
#line 1869 "parser.tab.c"
    break;

  case 44: /* string_atom: STRING  */
#line 564 "build/src/parser/bison/parser.y"
           {
        ast *a = ctx->ops.create_string_node((yyvsp[0].string_value));
        (yyval.ast) = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_STRING_NODE_CREATION_FAILED,
                    "ast creation for a string failed" );
    }
#line 1882 "parser.tab.c"
    break;

  case 45: /* symbol_name_atom: SYMBOL_NAME  */
#line 577 "build/src/parser/bison/parser.y"
                {
        ast *a = ctx->ops.create_symbol_name_node((yyvsp[0].symbol_name_value));
        (yyval.ast) = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_SYMBOL_NAME_NODE_CREATION_FAILED,
                    "ast creation for a symbol name failed" );
  }
#line 1895 "parser.tab.c"
    break;

  case 46: /* translation_unit: %empty  */
#line 590 "build/src/parser/bison/parser.y"
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
#line 1912 "parser.tab.c"
    break;

  case 47: /* translation_unit: translation_unit statement  */
#line 602 "build/src/parser/bison/parser.y"
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
#line 1933 "parser.tab.c"
    break;

  case 48: /* writing: WRITE symbol_name_atom SEMICOLON  */
#line 623 "build/src/parser/bison/parser.y"
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
#line 1952 "parser.tab.c"
    break;


#line 1956 "parser.tab.c"

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
      yyerror (scanner, result, ctx, YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
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

  return yyresult;
}

#line 641 "build/src/parser/bison/parser.y"


int yyerror(yyscan_t scanner, ast **result, struct parser_ctx *ctx, const char *msg) {
    (void)scanner; (void)result; (void)ctx;
    fprintf(stderr, "Parse error: %s\n", msg);
    return 1;
}

/* @formatter:on */
