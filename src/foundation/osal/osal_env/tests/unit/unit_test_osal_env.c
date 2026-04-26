/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_osal_env.c
 * @ingroup osal_env_unit_tests
 * @brief Unit tests for the public `osal_env` module.
 *
 * @details
 * This file contains unit tests for:
 * - `osal_getenv()`
 */

#if !defined(__unix__) && !defined(__APPLE__)
#error "These osal_env unit tests are supported only on POSIX-like platforms."
#endif

#include "osal/env/osal_env.h"

#include "lexleo_cmocka.h"

/**
 * @brief Scenarios for `osal_getenv()`.
 *
 * See contracts:
 * - @ref specifications_osal_getenv
 */
typedef enum {
	/**
	 * WHEN `osal_getenv()` is called with the name of an environment
	 *      variable known to exist for this test process
	 * EXPECT:
	 * - ret != NULL
	 * - ret points to the expected value
	 */
	OSAL_GETENV_SCENARIO_NAME_EXISTS = 0,

	/**
	 * WHEN `osal_getenv()` is called with the name of an environment
	 *      variable known not to exist for this test process
	 * EXPECT:
	 * - ret == NULL
	 */
	OSAL_GETENV_SCENARIO_NAME_MISSING,
} osal_getenv_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `osal_getenv()`.
 */
typedef struct {
	const char *name;
	osal_getenv_scenario_t scenario;
} test_osal_getenv_case_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_osal_getenv(void **state) {
	const test_osal_getenv_case_t *tc = (test_osal_getenv_case_t *)(*state);

	if (tc->scenario == OSAL_GETENV_SCENARIO_NAME_EXISTS) {
		setenv("LEXLEO_TEST_ENV", "hello", 1);
	}
	if (tc->scenario == OSAL_GETENV_SCENARIO_NAME_MISSING) {
		unsetenv("LEXLEO_TEST_ENV");
	}

	return 0;
}

static int teardown_osal_getenv(void **state) {
	(void)state;
	unsetenv("LEXLEO_TEST_ENV");
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `osal_getenv()`.
 */
static void test_osal_getenv(void **state)
{
	const test_osal_getenv_case_t *tc = (test_osal_getenv_case_t *)(*state);

	// ARRANGE
	const char *ret = (const char *)(uintptr_t)0xDEADC0DEu; // sentinel
	const char* name_arg = "LEXLEO_TEST_ENV";

	// ACT
	ret = osal_getenv(name_arg);

	// ASSERT
	if (tc->scenario == OSAL_GETENV_SCENARIO_NAME_EXISTS) {
		assert_non_null(ret);
		assert_string_equal(ret, "hello");
	}
	if (tc->scenario == OSAL_GETENV_SCENARIO_NAME_MISSING) {
		assert_null(ret);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_osal_getenv_case_t CASE_OSAL_GETENV_NAME_EXISTS = {
	.name = "osal_getenv_name_exists",
	.scenario = OSAL_GETENV_SCENARIO_NAME_EXISTS,
};

static const test_osal_getenv_case_t CASE_OSAL_GETENV_NAME_MISSING = {
	.name = "osal_getenv_name_missing",
	.scenario = OSAL_GETENV_SCENARIO_NAME_MISSING,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define OSAL_ENV_GETENV_CASES(X) \
X(CASE_OSAL_GETENV_NAME_EXISTS) \
X(CASE_OSAL_GETENV_NAME_MISSING)

#define OSAL_ENV_MAKE_GETENV_TEST(case_sym) \
LEXLEO_MAKE_TEST(osal_getenv, case_sym)

static const struct CMUnitTest osal_getenv_tests[] = {
	OSAL_ENV_GETENV_CASES(OSAL_ENV_MAKE_GETENV_TEST)
};

#undef OSAL_ENV_GETENV_CASES
#undef OSAL_ENV_MAKE_GETENV_TEST

/** @endcond */

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(osal_getenv_tests, NULL, NULL);

	return failed;
}

/** @endcond */
