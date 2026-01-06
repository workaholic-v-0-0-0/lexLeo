#ifndef LEXLEO_CMOCKA_XMACRO_HELPERS_H
#define LEXLEO_CMOCKA_XMACRO_HELPERS_H

#include <cmocka.h>

/*
 * X-macro helpers for defining CMocka parametric tests.
 *
 * Requirements:
 *   - `case_sym` must designate a test case object with a `name` field
 *     of type `const char *` (stable storage, e.g. string literal).
 *   - The test case object is passed to the test via `initial_state`.
 *
 * Intended for use in test code only.
 */
#define LEXLEO_CMOCKA_MAKE_TEST_ENTRY(case_sym, test_fn, setup_fn, teardown_fn) \
    {                                                                          \
        .name = (case_sym).name,                                               \
        .test_func = (test_fn),                                                \
        .setup_func = (setup_fn),                                              \
        .teardown_func = (teardown_fn),                                        \
        .initial_state = (void *)&(case_sym),                                  \
    }

/* Preprocessor token-pasting helpers (2-step expansion). */
#define LEXLEO_PP_CAT(a, b) LEXLEO_PP_CAT_I(a, b)
#define LEXLEO_PP_CAT_I(a, b) a##b

/* Naming convention:
 *   test_<suite>, setup_<suite>, teardown_<suite>
 */
#define LEXLEO_TEST_FN(suite) LEXLEO_PP_CAT(test_, suite)
#define LEXLEO_SETUP_FN(suite) LEXLEO_PP_CAT(setup_, suite)
#define LEXLEO_TEARDOWN_FN(suite) LEXLEO_PP_CAT(teardown_, suite)

/* One CMUnitTest entry using the naming convention above.
 * Note: the trailing comma is intentional for array initializers.
 */
#define LEXLEO_MAKE_TEST(suite, case_sym)                                      \
    LEXLEO_CMOCKA_MAKE_TEST_ENTRY(                                             \
        case_sym,                                                              \
        LEXLEO_TEST_FN(suite),                                                 \
        LEXLEO_SETUP_FN(suite),                                                \
        LEXLEO_TEARDOWN_FN(suite)                                              \
    ),

#endif /* LEXLEO_CMOCKA_XMACRO_HELPERS_H */
