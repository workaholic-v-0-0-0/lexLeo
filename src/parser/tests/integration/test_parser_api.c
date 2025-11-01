// src/parser/tests/integration/test_parser_api.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <string.h>

#include "osal.h"

#include "parser_ctx.h"
#include "parser.tab.h"
#include "lexer.yy.h"
#include "ast.h"

// forward declaration
bool parse_translation_unit(yyscan_t scanner, ast **out, struct parser_ctx *ctx);
bool parse_one_statement  (yyscan_t scanner, ast **out, struct parser_ctx *ctx);
bool parse_readable (yyscan_t scanner, ast **out, struct parser_ctx *ctx);

static FILE* open_memory_as_FILE(const char *s) {
#ifdef _GNU_SOURCE
    return osal_fmemopen_ro((void*)s, strlen(s));
#else
    FILE *f = tmpfile();
    if (!f) return NULL;
    fputs(s, f);
    rewind(f);
    return f;
#endif
}


static void test_parse_one_statement_three_calls(void **state) {
    (void)state;
    const char *src = "\
a=1;\n\
gamma(2);\n\
quote f(22 31);\n\
c=a+1;";
    FILE *f = open_memory_as_FILE(src);
    assert_non_null(f);

    yyscan_t scanner = NULL;
    assert_int_equal(0, yylex_init(&scanner));
    yyset_in(f, scanner);

    struct parser_ctx *ctx = get_g_parser_ctx_default_one_statement();
    struct ast *node = NULL;

    assert_true(parse_one_statement(scanner, &node, ctx));
    printf("ast_type_to_string(node->type): %s\n", ast_type_to_string(node->type));
    assert_non_null(node);
    ast_print(node);
    ast_destroy(node);
    node = NULL;

    assert_true(parse_one_statement(scanner, &node, ctx));
    assert_non_null(node);
    ast_print(node);
    ast_destroy(node);
    node = NULL;

    assert_true(parse_one_statement(scanner, &node, ctx));
    assert_non_null(node);
    ast_print(node);
    ast_destroy(node);
    node = NULL;

    assert_true(parse_one_statement(scanner, &node, ctx));
    assert_non_null(node);
    ast_print(node);
    ast_destroy(node);
    node = NULL;

    assert_false(parse_one_statement(scanner, &node, ctx));
    assert_null(node);

    assert_int_equal(0, yylex_destroy(scanner));
    fclose(f);
}

static void test_parse_translation_unit(void **state) {
    (void)state;
    const char *src = "a=1; define f(x) {x = 7;} e=7;";
    FILE *f = open_memory_as_FILE(src);
    assert_non_null(f);

    yyscan_t scanner = NULL;
    assert_int_equal(0, yylex_init(&scanner));
    yyset_in(f, scanner);

    struct parser_ctx *ctx = get_g_parser_ctx_default_translation_unit();
    struct ast *node = NULL;

    assert_true(parse_translation_unit(scanner, &node, ctx));
    ast_print(node);

    printf("\n\n\n\n");
    assert_non_null(node);
    ast_destroy(node);
    node = NULL;

    assert_int_equal(0, yylex_destroy(scanner));
    fclose(f);
}

static void test_parse_readable_a_number(void **state) {
    (void)state;
    const char *src = "7";
    FILE *f = open_memory_as_FILE(src);
    assert_non_null(f);

    yyscan_t scanner = NULL;
    assert_int_equal(0, yylex_init(&scanner));
    yyset_in(f, scanner);

    struct parser_ctx *ctx = get_g_parser_ctx_default_readable();
    struct ast *node = NULL;

    assert_true(parse_readable(scanner, &node, ctx));
    assert_non_null(node);
    ast_print(node);
    ast_destroy(node);
    node = NULL;

    assert_false(parse_one_statement(scanner, &node, ctx));
    assert_null(node);

    assert_int_equal(0, yylex_destroy(scanner));
    fclose(f);
}

static void test_parse_readable_a_tu(void **state) {
    (void)state;
    const char *src = "\
a=1;\n\
define f(x y) {a = x+y;}\n\
a=f(1 2);\n\
write a;\n\
;;";//here
    FILE *f = open_memory_as_FILE(src);
    assert_non_null(f);

    yyscan_t scanner = NULL;
    assert_int_equal(0, yylex_init(&scanner));
    yyset_in(f, scanner);

    struct parser_ctx *ctx = get_g_parser_ctx_default_readable();
    struct ast *node = NULL;

    assert_true(parse_readable(scanner, &node, ctx));
    assert_non_null(node);
    ast_print(node);
    ast_destroy(node);
    node = NULL;

    assert_false(parse_one_statement(scanner, &node, ctx));
    assert_null(node);

    assert_int_equal(0, yylex_destroy(scanner));
    fclose(f);
}




//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

static void test_parse_one_statement_three_calls(void **state);
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_one_statement_three_calls),
        cmocka_unit_test(test_parse_translation_unit),
        cmocka_unit_test(test_parse_readable_a_number),
        cmocka_unit_test(test_parse_readable_a_tu),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
