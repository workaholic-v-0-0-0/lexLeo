/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/core/lexer/adapters/lexleo_flex/tests/unit/
 * unit_test_lexleo_flex.c
 */

#include "lexleo_flex_test_api.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "lexleo_cmocka_xmacro_helpers.h"

/* draft
 * which fcts to test?
 * flex_next and destroy
 * yes but through a lexer_t
 * so create too? -> yes
 */

//-----------------------------------------------------------------------------
// TEST
// lexer_status_t lexleo_flex_create_lexer(
//		lexer_t **out,
//		struct input_provider *prov,
//		const lexleo_flex_ctx_t *ctx )
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TEST CONTEXT
//-----------------------------------------------------------------------------
// Isolation:
//  - Flex library
//
//
//
// Test doubles:
//   - osal_mem_ops_t
//   - param: struct input_provider *prov
//
//



//-----------------------------------------------------------------------------
// CASE STRUCTURE
//-----------------------------------------------------------------------------

typedef struct {

    // test name
    const char *name;



    // arrange utilities

    // assert utilities

    // test infrastructure cleanup utilities

    // information sharing utilities

} test_lexleo_flex_create_case_t;

typedef struct {


	const test_lexleo_flex_create_case_t *tc;
} test_lexleo_flex_create_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_create(void **state) {
    const test_lexleo_flex_create_case_t *tc =
        (const test_lexleo_flex_create_case_t *)*state;

    test_lexleo_flex_create_fixture_t *fx = malloc(sizeof(*fx));
	if (!fx) return -1;

    memset(fx, 0, sizeof(*fx));
    fx->tc = tc;

    /* initialize runtime state here */
    /* fx->mem = ... */
    /* fx->backend = ... */

    *state = fx;   // <-- IMPORTANT

    return 0;
}


static int teardown_create(void **state) {

    return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_create(void **state) {
	const test_lexleo_flex_create_case_t *test_case =
		(const test_lexleo_flex_create_case_t *) *state;

}

// Given:
//  -
// Expected:
//  -
static const test_create_case CASE_DESTROY_ = {
    .name = "create_",
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define LEXLEO_FLEX_CREATE_CASES(X) \
    X(case_create_ok)              \
    X(case_create_oom_backend)

#define LEXLEO_MAKE_CREATE_TEST(case_sym) \
    LEXLEO_MAKE_TEST(create, case_sym)

static const struct CMUnitTest create_tests[] = {
    LEXLEO_FLEX_CREATE_CASES(LEXLEO_MAKE_CREATE_TEST)
};

#undef LEXLEO_FLEX_CREATE_CASES
#undef LEXLEO_MAKE_CREATE_TEST





//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// TEST
// static void destroy(void *backend)
//-----------------------------------------------------------------------------
// Note: static adapter implementation; tested via the lexer port contract
// (vtbl dispatch), not called directly.


//-----------------------------------------------------------------------------
// TEST CONTEXT
//-----------------------------------------------------------------------------
// Isolation:
//  - Flex library
//
//
//
// Test doubles:
//   - osal_mem_ops_t
//
//
//



//-----------------------------------------------------------------------------
// CASE STRUCTURE
//-----------------------------------------------------------------------------

typedef struct {

    // test name
    const char *name;



    // arrange utilities

    // assert utilities

    // test infrastructure cleanup utilities

    // information sharing utilities

} test_lexleo_flex_destroy_case_t;

typedef struct {


	const test_lexleo_flex_destroy_case_t *tc;
} test_lexleo_flex_destroy_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_destroy(void **state) {
    const test_lexleo_flex_destroy_case_t *tc =
        (const test_lexleo_flex_destroy_case_t *)*state;

    test_lexleo_flex_destroy_fixture_t *fx = malloc(sizeof(*fx));
	if (!fx) return -1;

    memset(fx, 0, sizeof(*fx));
    fx->tc = tc;

    /* initialize runtime state here */
    /* fx->mem = ... */
    /* fx->backend = ... */

    *state = fx;   // <-- IMPORTANT

    return 0;
}


static int teardown_destroy(void **state) {

    return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_destroy(void **state) {
	const test_lexleo_flex_destroy_case_t *test_case =
		(const test_lexleo_flex_destroy_case_t *) *state;

}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

// Given:
//  -
// Expected:
//  -
static const test_destroy_case CASE_DESTROY_ = {
    .name = "destroy_",
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define LEXLEO_FLEX_DESTROY_CASES(X) \
    X(case_destroy_ok)              \
    X(case_destroy_oom_backend)

#define LEXLEO_MAKE_DESTROY_TEST(case_sym) \
    LEXLEO_MAKE_TEST(destroy, case_sym)

static const struct CMUnitTest destroy_tests[] = {
    LEXLEO_FLEX_DESTROY_CASES(LEXLEO_MAKE_DESTROY_TEST)
};

#undef LEXLEO_FLEX_DESTROY_CASES
#undef LEXLEO_MAKE_DESTROY_TEST

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	int failed = 0;
    failed += cmocka_run_group_tests(destroy_tests, NULL, NULL);
    return failed;
}





// src/adapters/lexer/lexleo_flex/tests/unit/unit_test_lexleo_flex.c