// src/lexer/tests/test_yylex.yy.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <assert.h>
#include <string.h>

#include "parser.tab.h"
#include "lexer.yy.h"

#include "list.h"


//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


YYSTYPE *yylval = NULL;
yyscan_t scanner = NULL; // in build/src/lexer/include/lexer.yy.h: typedef void* yyscan_t;

static list collected_ptr_to_be_freed = NULL;
static const YYSTYPE int_42 = {.int_value = 42};
static const YYSTYPE int_67 = {.int_value = 67};
static const YYSTYPE int_3 = {.int_value = 3};
static const YYSTYPE int_5 = {.int_value = 5};
static const YYSTYPE symbol_livre = {.symbol_name_value = "livre"};
static const YYSTYPE symbol_finishing_with_number_42 = {.symbol_name_value = "finishing_with_number_42"};
static const YYSTYPE symbol_number = {.symbol_name_value = "number"};
static const YYSTYPE string_livre = {.string_value = "livre"};
static const YYSTYPE string_42 = {.string_value = "42"};


//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------


static void alloc_and_save_address_to_be_freed(void **ptr, size_t size) {
    *ptr = malloc(size);
    assert_non_null(*ptr);
    collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, *ptr);
}


//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//----------------------------------------------------------------------------


typedef struct {
    const char *input;
    size_t tokens_nb;
    const int *expected_tokens;
    const YYSTYPE *const *expected_values;
} params_t;


//-----------------------------------------------------------------------------
// GENERAL CONFIG CASES
//-----------------------------------------------------------------------------

// "42"
static const int p_00_expected_tokens[] = {
    INTEGER
};
static const YYSTYPE *const p_00_expected_values[] = {
    &int_42
};
static const params_t p_00 = {
    .input = "42",
    .tokens_nb = 1,
    .expected_tokens = p_00_expected_tokens,
    .expected_values = p_00_expected_values,
};


// "      \n    42 \t\n\t  livre\t\n\n \"livre\""
static const int p_01_expected_tokens[] = {
    INTEGER, SYMBOL, STRING
};
static const YYSTYPE *const p_01_expected_values[] = {
    &int_42, &symbol_livre, &string_livre
};
static const params_t p_01 = {
    .input = "      \n    42 \t\n\t  livre\t\n\n \"livre\"",
    .tokens_nb = 3,
    .expected_tokens = p_01_expected_tokens,
    .expected_values = p_01_expected_values,
};


// livre=42;
static const int p_02_expected_tokens[] = {
    SYMBOL, EQUAL, INTEGER, SEMICOLON
};
static const YYSTYPE *const p_02_expected_values[] = {
    &symbol_livre, NULL, &int_42, NULL
};
static const params_t p_02 = {
    .input = "livre=42;",
    .tokens_nb = 4,
    .expected_tokens = p_02_expected_tokens,
    .expected_values = p_02_expected_values,
};


// livre="42";
static const int p_03_expected_tokens[] = {
    SYMBOL, EQUAL, STRING, SEMICOLON
};
static const YYSTYPE *const p_03_expected_values[] = {
    &symbol_livre, NULL, &string_42, NULL
};
static const params_t p_03 = {
    .input = "livre=\"42\";",
    .tokens_nb = 4,
    .expected_tokens = p_03_expected_tokens,
    .expected_values = p_03_expected_values,
};


// "number=(42+3)*67/5;"
static const int p_04_expected_tokens[] = {
    SYMBOL, EQUAL, LEFT_PARENTHESIS, INTEGER, PLUS, INTEGER, RIGHT_PARENTHESIS, MULTIPLY, INTEGER, DIVIDE, INTEGER,
    SEMICOLON
};
static const YYSTYPE *const p_04_expected_values[] = {
    &symbol_number, NULL, NULL, &int_42, NULL, &int_3, NULL, NULL, &int_67, NULL, &int_5, NULL
};
static const params_t p_04 = {
    .input = "number=(42+3)*67/5;",
    .tokens_nb = 12,
    .expected_tokens = p_04_expected_tokens,
    .expected_values = p_04_expected_values,
};


// "evaluate execute compute read write /*-+)(;finishing_with_number_42"
static const int p_05_expected_tokens[] = {
    EVALUATE, EXECUTE, COMPUTE, READ, WRITE, DIVIDE, MULTIPLY, MINUS, PLUS, RIGHT_PARENTHESIS, LEFT_PARENTHESIS,
    SEMICOLON, SYMBOL
};
static const YYSTYPE *const p_05_expected_values[] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &symbol_finishing_with_number_42
};
static const params_t p_05 = {
    .input = "evaluate execute compute read write /*-+)(;finishing_with_number_42",
    .tokens_nb = 13,
    .expected_tokens = p_05_expected_tokens,
    .expected_values = p_05_expected_values,
};


#define NB_OF_PARAMS 6

/* copy-paste to add a new tests and do noot forget to update params
// ""
static const int p__expected_tokens[] = {

};
static const YYSTYPE *const p__expected_values[] = {

};
static const params_t p_ = {
    .input = "",
    .tokens_nb = ,
    .expected_tokens = p__expected_tokens,
    .expected_values = p__expected_values,
};
*/

static const void *params[NB_OF_PARAMS] = {
    &p_00, &p_01, &p_02, &p_03, &p_04, &p_05
};


//-----------------------------------------------------------------------------
// yylex TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int setup(void **state) {
    alloc_and_save_address_to_be_freed((void **) &yylval, sizeof(YYSTYPE));
    assert(yylex_init(&scanner) == 0);
    memset(yylval, 0, sizeof(YYSTYPE));
    return 0;
}

static int teardown(void **state) {
    assert(yylex_destroy(scanner) == 0);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


static void yylex_tokenize_properly(void **state) {
    params_t *p = (params_t *) *state;
    yy_scan_string(
        p->input,
        scanner);

    int token = 0;
    size_t index = 0;
    while ((token = yylex(yylval, scanner)) != 0) {
        assert(token == p->expected_tokens[index]);
        if (token == INTEGER) {
            assert(yylval->int_value == (p->expected_values[index])->int_value);
        } else if (token == STRING) {
            assert_int_equal(
                0,
                strcmp(
                    yylval->string_value,
                    (p->expected_values[index])->string_value));
            free(yylval->string_value);
        } else if (token == SYMBOL) {
            assert_int_equal(
                0,
                strcmp(
                    yylval->symbol_name_value,
                    (p->expected_values[index])->symbol_name_value));
            free(yylval->string_value);
        }
        index++;
    }
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    struct CMUnitTest yylex_tokenize_properly_tests[NB_OF_PARAMS + 1] = {0};
    for (size_t i = 0; i < NB_OF_PARAMS; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            yylex_tokenize_properly, setup, teardown, (void *) params[i]);
        yylex_tokenize_properly_tests[i] = tmp;
    }

    int failed = 0;
    failed += cmocka_run_group_tests(yylex_tokenize_properly_tests, NULL, NULL);
    return failed;
}