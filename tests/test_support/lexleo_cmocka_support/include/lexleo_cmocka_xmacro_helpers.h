/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_cmocka_xmacro_helpers.h
 * @ingroup test_support_cmocka
 * @brief X-macro helpers for defining parametric CMocka tests.
 */

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

#define LEXLEO_TEST_CASE_TYPE(suite) \
	LEXLEO_PP_CAT(LEXLEO_PP_CAT(test_, suite), _case_t)

#define LEXLEO_TEST_FIXTURE_TYPE(suite) \
	LEXLEO_PP_CAT(LEXLEO_PP_CAT(test_, suite), _fixture_t)

#define LEXLEO_CMOCKA_INIT_SETUP(suite, state, tc_var, fx_var)             \
	const LEXLEO_TEST_CASE_TYPE(suite) *tc_var =                            \
		(const LEXLEO_TEST_CASE_TYPE(suite) *)(*(state));                    \
	LEXLEO_TEST_FIXTURE_TYPE(suite) *fx_var =                               \
		osal_malloc(sizeof(*(fx_var)));                                      \
	if (!(fx_var)) {                                                        \
		return -1;                                                          \
	}                                                                       \
	osal_memset((fx_var), 0, sizeof(*(fx_var)));                            \
	(fx_var)->tc = (tc_var)

#endif /* LEXLEO_CMOCKA_XMACRO_HELPERS_H */
