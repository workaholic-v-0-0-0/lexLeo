/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_osal_str.c
 * @ingroup osal_str_unit_tests
 * @brief Unit tests for the public `osal_str` module.
 *
 * @details
 * This file contains unit tests for:
 * - `osal_str_default_ops()`
 * - `osal_str_ops_t::strdup()`
 * - `osal_snprintf()`
 * - `osal_strlen()`
 * - `osal_strcmp()`
 * - `osal_strchr()`
 * - `osal_strrchr()`
 * - `osal_isspace()`
 */

#if !defined(__unix__) && !defined(__APPLE__)
#error "These osal_str unit tests are supported only on POSIX-like platforms."
#endif

#include "osal/str/osal_str.h"
#include "osal/str/osal_str_ops.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "lexleo_cmocka.h"

#include "policy/lexleo_cstd_types.h"

/**
 * @brief Test `osal_str_default_ops()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_str_unit_default_ops "osal_str_default_ops() unit tests section"
 * - @ref specifications_osal_str_default_ops "osal_str_default_ops() specifications".
 */
static void test_osal_str_default_ops(void **state)
{
	(void)state;

	const osal_str_ops_t *ret = osal_str_default_ops();

	assert_non_null(ret);
	assert_non_null(ret->strdup);
}

/**
 * @brief Test `osal_str_ops_t::strdup()`.
 *
 * Doubles:
 * - fake_memory
 *
 * See contract:
 * - @ref testing_foundation_osal_str_unit_strdup "osal_str_ops_t::strdup() unit tests section"
 * - @ref specifications_osal_str_ops_strdup "osal_str_ops_t::strdup() specifications".
 */
typedef enum {
	/**
	 * WHEN `osal_str_ops_t::strdup()` is called with `s == NULL`
	 * EXPECT:
	 * - returns `NULL`
	 * - no allocation is performed
	 */
	OSAL_STR_STRDUP_SCENARIO_S_NULL = 0,

	/**
	 * WHEN `osal_str_ops_t::strdup()` is called with `mem_ops == NULL`
	 * EXPECT:
	 * - returns `NULL`
	 * - no allocation is performed
	 */
	OSAL_STR_STRDUP_SCENARIO_MEM_OPS_NULL,

	/**
	 * WHEN `osal_str_ops_t::strdup()` is called with a valid empty string
	 * EXPECT:
	 * - returns a non-NULL pointer
	 * - returned string is empty
	 * - returned pointer is distinct from `s`
	 * - allocation size is 1 byte
	 */
	OSAL_STR_STRDUP_SCENARIO_EMPTY_STRING,

	/**
	 * WHEN `osal_str_ops_t::strdup()` is called with a valid non-empty string
	 * EXPECT:
	 * - returns a non-NULL pointer
	 * - returned string has the same contents as `s`
	 * - returned pointer is distinct from `s`
	 * - allocation size is `strlen(s) + 1`
	 */
	OSAL_STR_STRDUP_SCENARIO_NON_EMPTY_STRING,

	/**
	 * WHEN `mem_ops->malloc` fails
	 * EXPECT:
	 * - returns `NULL`
	 */
	OSAL_STR_STRDUP_SCENARIO_MALLOC_FAILS
} osal_str_strdup_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `osal_str_ops_t::strdup()`.
 */
typedef struct {
	const char *name;

	/* arrange */
	osal_str_strdup_scenario_t scenario;
	const char *s_arg;
	size_t fail_call_idx; /* 0 = no OOM, otherwise 1-based */

	/* assert */
	bool expected_ret_null;
	size_t expected_alloc_size;
	const char *expected_ret_string;
} test_osal_str_strdup_case_t;

/**
 * @brief Runtime fixture for `osal_str_ops_t::strdup`.
 */
typedef struct {
	/* Resources */
	char *ret;

	const osal_str_ops_t *str_ops;

	/* DI */
	const osal_mem_ops_t *mem_ops;

	const test_osal_str_strdup_case_t *tc;
} test_osal_str_strdup_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `osal_str_ops_t::strdup()` tests.
 */
static int setup_osal_str_strdup(void **state)
{
	const test_osal_str_strdup_case_t *tc =
		(const test_osal_str_strdup_case_t *)(*state);

	test_osal_str_strdup_fixture_t *fx =
		(test_osal_str_strdup_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == OSAL_STR_STRDUP_SCENARIO_MALLOC_FAILS && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	fx->str_ops = osal_str_default_ops();

	// DI
	fx->mem_ops = osal_mem_test_fake_ops();

	*state = fx;
	return 0;
}

/**
 * @brief Release the `osal_str_ops_t::strdup()` test fixture and verify memory invariants.
 */
static int teardown_osal_str_strdup(void **state)
{
	test_osal_str_strdup_fixture_t *fx =
		(test_osal_str_strdup_fixture_t *)(*state);

	if (fx->ret) {
		fx->mem_ops->free(fx->ret);
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	osal_free(fx);
	*state = NULL;
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `osal_str_ops_t::strdup()`.
 */
static void test_osal_str_strdup(void **state)
{
	test_osal_str_strdup_fixture_t *fx =
		(test_osal_str_strdup_fixture_t *)(*state);
	const test_osal_str_strdup_case_t *tc =
		(const test_osal_str_strdup_case_t *)fx->tc;

	// ARRANGE
	const osal_mem_ops_t *mem_ops_arg = fx->mem_ops;

	// invalid args
	if (tc->scenario == OSAL_STR_STRDUP_SCENARIO_MEM_OPS_NULL) {
		mem_ops_arg = NULL;
	}

	// ACT
	fx->ret = fx->str_ops->strdup(tc->s_arg, mem_ops_arg);

	// ASSERT
	if (tc->expected_ret_null) {
		assert_null(fx->ret);
	} else {
		assert_non_null(fx->ret);
		assert_true(fx->ret != tc->s_arg);
		assert_memory_equal(
			fx->ret,
			tc->expected_ret_string,
			tc->expected_alloc_size
		);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_osal_str_strdup_case_t CASE_OSAL_STR_STRDUP_S_NULL = {
	.name = "osal_str_strdup_s_null",
	.scenario = OSAL_STR_STRDUP_SCENARIO_S_NULL,
	.s_arg = NULL,
	.fail_call_idx = 0, /* no OOM */

	/* assert */
	.expected_ret_null = true,
};

static const test_osal_str_strdup_case_t CASE_OSAL_STR_STRDUP_MEM_OPS_NULL = {
	.name = "osal_str_strdup_mem_ops_null",
	.scenario = OSAL_STR_STRDUP_SCENARIO_MEM_OPS_NULL,

	/* assert */
	.expected_ret_null = true,
};

static const test_osal_str_strdup_case_t CASE_OSAL_STR_STRDUP_EMPTY_STRING = {
	.name = "osal_str_strdup_empty_string",
	.scenario = OSAL_STR_STRDUP_SCENARIO_EMPTY_STRING,
	.s_arg = "",
	.fail_call_idx = 0, /* no OOM */

	/* assert */
	.expected_ret_null = false,
	.expected_alloc_size = 1,
	.expected_ret_string = ""
};

static const test_osal_str_strdup_case_t CASE_OSAL_STR_STRDUP_NON_EMPTY_STRING = {
	.name = "osal_str_strdup_non_empty_string",
	.scenario = OSAL_STR_STRDUP_SCENARIO_NON_EMPTY_STRING,
	.s_arg = "abc",
	.fail_call_idx = 0, /* no OOM */

	/* assert */
	.expected_ret_null = false,
	.expected_alloc_size = 4,
	.expected_ret_string = "abc"
};

static const test_osal_str_strdup_case_t CASE_OSAL_STR_STRDUP_MALLOC_FAILS = {
	.name = "osal_str_strdup_malloc_fails",
	.scenario = OSAL_STR_STRDUP_SCENARIO_MALLOC_FAILS,
	.s_arg = "abc",
	.fail_call_idx = 1, /* OOM */

	/* assert */
	.expected_ret_null = true
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define OSAL_STR_STRDUP_CASES(X) \
X(CASE_OSAL_STR_STRDUP_S_NULL) \
X(CASE_OSAL_STR_STRDUP_MEM_OPS_NULL) \
X(CASE_OSAL_STR_STRDUP_EMPTY_STRING) \
X(CASE_OSAL_STR_STRDUP_NON_EMPTY_STRING) \
X(CASE_OSAL_STR_STRDUP_MALLOC_FAILS)

#define OSAL_STR_MAKE_STRDUP_TEST(case_sym) \
LEXLEO_MAKE_TEST(osal_str_strdup, case_sym)

static const struct CMUnitTest osal_str_strdup_tests[] = {
	OSAL_STR_STRDUP_CASES(OSAL_STR_MAKE_STRDUP_TEST)
};

#undef OSAL_STR_STRDUP_CASES
#undef OSAL_STR_MAKE_STRDUP_TEST

/** @endcond */

/**
 * @brief Test `osal_snprintf()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_str_unit_osal_snprintf "osal_snprintf() unit tests section"
 * - @ref specifications_osal_snprintf "osal_snprintf() specifications".
 */
typedef enum {
	/**
	 * WHEN `osal_snprintf()` is called with `format == "abc"`
	 * AND `str == NULL`
	 * AND `size == 3`
	 * AND no optional arg
	 * EXPECT:
	 * - returns `-1`
	 */
	OSAL_STR_SNPRINTF_SCENARIO_STR_NULL_SIZE_NOT_ZERO = 0,

	/**
	 * WHEN `osal_snprintf()` is called with `format == NULL`
	 * AND `str != NULL`
	 * AND `size == 3`
	 * AND no optional arg
	 * EXPECT:
	 * - returns `-1`
	 * - no byte in the buffer pointed to by `str` is modified
	 */
	OSAL_STR_SNPRINTF_SCENARIO_FORMAT_NULL,

	/**
	 * WHEN `osal_snprintf()` is called with `format == "number is %d"`
	 * AND `str != NULL`
	 * AND `size == 0`
	 * AND `<first optional arg> == 12345`
	 * EXPECT:
	 * - returns `15`
	 * - no byte in the buffer pointed to by `str` is modified
	 */
	OSAL_STR_SNPRINTF_SCENARIO_SIZE_ZERO_FORMAT_NON_NULL,

	/**
	 * WHEN `osal_snprintf()` is called with `format == "number is %d"`
	 * AND `str == NULL`
	 * AND `size == 0`
	 * AND `<first optional arg> == 12345`
	 * EXPECT:
	 * - returns `15`
	 */
	OSAL_STR_SNPRINTF_SCENARIO_SIZE_ZERO_STR_NULL_FORMAT_NON_NULL,

	/**
	 * WHEN `osal_snprintf()` is called with `format == "number is %d"`
	 * AND `str != NULL`
	 * AND `size == 15`
	 * AND `<first optional arg> == 12345`
	 * EXPECT:
	 * - returns `15`
	 * - str == "number is 1234" (null-terminated)
	 */
	OSAL_STR_SNPRINTF_SCENARIO_TRUNCATED,

	/**
	 * WHEN `osal_snprintf()` is called with `format == "number is %d"`
	 * AND `str != NULL`
	 * AND `size == 16`
	 * AND `<first optional arg> == 12345`
	 * EXPECT:
	 * - returns `15`
	 * - str == "number is 12345" (null-terminated)
	 */
	OSAL_STR_SNPRINTF_SCENARIO_EXACT_FIT,

	/**
	 * WHEN `osal_snprintf()` is called with `format == "number is %d"`
	 * AND `str != NULL`
	 * AND `size == 32`
	 * AND `<first optional arg> == 12345`
	 * EXPECT:
	 * - returns `15`
	 * - str == "number is 12345" (null-terminated)
	 */
	OSAL_STR_SNPRINTF_SCENARIO_LARGE_BUFFER,

} osal_snprintf_scenario_t;

/** @cond INTERNAL */

#define OSAL_STR_SNPRINTF_BUFFER_SIZE 32

/**
 * @brief One parametric test case for `osal_snprintf()`.
 */
typedef struct {
	const char *name;

	/* arrange */
	osal_snprintf_scenario_t scenario;
	size_t size_arg;
	const char *format_arg;
	bool has_an_optional_arg;
	int optional_arg;

	/* assert */
	int expected_ret;
	const char *expected_ret_string;
} test_osal_str_snprintf_case_t;

/**
 * @brief Runtime fixture for `osal_snprintf()`.
 */
typedef struct {
	/* Resources */
	char str_arg[OSAL_STR_SNPRINTF_BUFFER_SIZE];
	char str_arg_snapshot[OSAL_STR_SNPRINTF_BUFFER_SIZE];

	const test_osal_str_snprintf_case_t *tc;
} test_osal_str_snprintf_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `osal_snprintf()` tests.
 */
static int setup_osal_str_snprintf(void **state)
{
	const test_osal_str_snprintf_case_t *tc =
		(const test_osal_str_snprintf_case_t *)(*state);

	test_osal_str_snprintf_fixture_t *fx =
		(test_osal_str_snprintf_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	*state = fx;
	return 0;
}

/**
 * @brief Release the `osal_snprintf()` test fixture and verify memory invariants.
 */
static int teardown_osal_str_snprintf(void **state)
{
	test_osal_str_snprintf_fixture_t *fx =
		(test_osal_str_snprintf_fixture_t *)(*state);

	osal_free(fx);
	*state = NULL;
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `osal_snprintf()`.
 */
static void test_osal_str_snprintf(void **state)
{
	test_osal_str_snprintf_fixture_t *fx =
		(test_osal_str_snprintf_fixture_t *)(*state);
	const test_osal_str_snprintf_case_t *tc =
		(const test_osal_str_snprintf_case_t *)fx->tc;

	// ARRANGE
	int ret = 0;
	char *str_arg = fx->str_arg;
	size_t size_arg = tc->size_arg;
	const char *format_arg = tc->format_arg;
	int optional_arg = 0;
	if (tc->has_an_optional_arg) {
		optional_arg = tc->optional_arg;
	}

	// invalid args
	if (
		   tc->scenario == OSAL_STR_SNPRINTF_SCENARIO_STR_NULL_SIZE_NOT_ZERO
		|| tc->scenario == OSAL_STR_SNPRINTF_SCENARIO_SIZE_ZERO_STR_NULL_FORMAT_NON_NULL
	) {
		str_arg = NULL;
	}

	if (str_arg) {
		osal_memset(
			str_arg,
			'X',
			sizeof(fx->str_arg)
		);
		osal_memcpy(
			fx->str_arg_snapshot,
			str_arg,
			OSAL_STR_SNPRINTF_BUFFER_SIZE
		);
	}

	// ACT
	if  (tc->has_an_optional_arg) {
		ret = osal_snprintf(str_arg, size_arg, format_arg, optional_arg);
	} else {
		ret = osal_snprintf(str_arg, size_arg, format_arg);
	}

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);
	if (tc->expected_ret_string) {
		size_t len = osal_strlen(tc->expected_ret_string);
		assert_memory_equal(
			str_arg,
			tc->expected_ret_string,
			len + 1
		);
		assert_memory_equal(
			str_arg + len + 1,
			fx->str_arg_snapshot + len + 1,
			OSAL_STR_SNPRINTF_BUFFER_SIZE - ( len + 1 )
		);
	} else {
		if (str_arg) {
			assert_memory_equal(
				str_arg,
				fx->str_arg_snapshot,
				OSAL_STR_SNPRINTF_BUFFER_SIZE
			);
		}
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_osal_str_snprintf_case_t CASE_OSAL_STR_SNPRINTF_STR_NULL_SIZE_NOT_ZERO = {
	.name = "osal_str_snprintf_str_null_size_not_zero",
	.scenario = OSAL_STR_SNPRINTF_SCENARIO_STR_NULL_SIZE_NOT_ZERO,
	.size_arg = 3,
	.format_arg = "abc",
	.has_an_optional_arg = false,

	.expected_ret = -1,
	.expected_ret_string = NULL
};

static const test_osal_str_snprintf_case_t CASE_OSAL_STR_SNPRINTF_FORMAT_NULL = {
	.name = "osal_str_snprintf_format_null",
	.scenario = OSAL_STR_SNPRINTF_SCENARIO_FORMAT_NULL,
	.size_arg = 3,
	.format_arg = NULL,
	.has_an_optional_arg = false,

	.expected_ret = -1,
	.expected_ret_string = NULL
};

static const test_osal_str_snprintf_case_t CASE_OSAL_STR_SNPRINTF_SIZE_ZERO_FORMAT_NON_NULL = {
	.name = "osal_str_snprintf_size_zero_format_non_null",
	.scenario = OSAL_STR_SNPRINTF_SCENARIO_SIZE_ZERO_FORMAT_NON_NULL,
	.size_arg = 0,
	.format_arg = "number is %d",
	.has_an_optional_arg = true,
	.optional_arg = 12345,

	.expected_ret = 15,
	.expected_ret_string = NULL
};

static const test_osal_str_snprintf_case_t CASE_OSAL_STR_SNPRINTF_SIZE_ZERO_STR_NULL_FORMAT_NON_NULL = {
	.name = "osal_str_snprintf_size_zero_str_null_format_non_null",
	.scenario = OSAL_STR_SNPRINTF_SCENARIO_SIZE_ZERO_STR_NULL_FORMAT_NON_NULL,
	.size_arg = 0,
	.format_arg = "number is %d",
	.has_an_optional_arg = true,
	.optional_arg = 12345,

	.expected_ret = 15,
	.expected_ret_string = NULL
};

static const test_osal_str_snprintf_case_t CASE_OSAL_STR_SNPRINTF_TRUNCATED = {
	.name = "osal_str_snprintf_truncated",
	.scenario = OSAL_STR_SNPRINTF_SCENARIO_TRUNCATED,
	.size_arg = 15,
	.format_arg = "number is %d",
	.has_an_optional_arg = true,
	.optional_arg = 12345,

	.expected_ret = 15,
	.expected_ret_string = "number is 1234"
};

static const test_osal_str_snprintf_case_t CASE_OSAL_STR_SNPRINTF_EXACT_FIT = {
	.name = "osal_str_snprintf_exact_fit",
	.scenario = OSAL_STR_SNPRINTF_SCENARIO_EXACT_FIT,
	.size_arg = 16,
	.format_arg = "number is %d",
	.has_an_optional_arg = true,
	.optional_arg = 12345,

	.expected_ret = 15,
	.expected_ret_string = "number is 12345"
};

static const test_osal_str_snprintf_case_t CASE_OSAL_STR_SNPRINTF_LARGE_BUFFER = {
	.name = "osal_str_snprintf_large_buffer",
	.scenario = OSAL_STR_SNPRINTF_SCENARIO_LARGE_BUFFER,
	.size_arg = 32,
	.format_arg = "number is %d",
	.has_an_optional_arg = true,
	.optional_arg = 12345,

	.expected_ret = 15,
	.expected_ret_string = "number is 12345"
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define OSAL_STR_SNPRINTF_CASES(X) \
X(CASE_OSAL_STR_SNPRINTF_STR_NULL_SIZE_NOT_ZERO) \
X(CASE_OSAL_STR_SNPRINTF_FORMAT_NULL) \
X(CASE_OSAL_STR_SNPRINTF_SIZE_ZERO_FORMAT_NON_NULL) \
X(CASE_OSAL_STR_SNPRINTF_SIZE_ZERO_STR_NULL_FORMAT_NON_NULL) \
X(CASE_OSAL_STR_SNPRINTF_TRUNCATED) \
X(CASE_OSAL_STR_SNPRINTF_EXACT_FIT) \
X(CASE_OSAL_STR_SNPRINTF_LARGE_BUFFER)

#define OSAL_STR_MAKE_SNPRINTF_TEST(case_sym) \
LEXLEO_MAKE_TEST(osal_str_snprintf, case_sym)

static const struct CMUnitTest osal_str_snprintf_tests[] = {
	OSAL_STR_SNPRINTF_CASES(OSAL_STR_MAKE_SNPRINTF_TEST)
};

#undef OSAL_STR_SNPRINTF_CASES
#undef OSAL_STR_MAKE_SNPRINTF_TEST

/** @endcond */

/**
 * @brief Test `osal_strlen()`
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_str_unit_osal_strlen "osal_strlen() unit tests section"
 * - @ref specifications_osal_strlen "osal_strlen() specifications".
 */
static void test_osal_str_osal_strlen(void **state) {
	(void)state;

	// arrange
	const char *s = "1234567";

	// act
	size_t ret = osal_strlen(s);

	// assert
	assert_true(ret == 7);
}

/**
 * @brief Test `osal_strcmp()`
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_str_unit_osal_strcmp "osal_strcmp() unit tests section"
 * - @ref specifications_osal_strcmp "osal_strcmp() specifications".
 */
static void test_osal_str_osal_strcmp(void **state) {
	(void)state;

	// arrange
	const char *s1 = "001";
	const char *s2 = "002";

	// act
	int ret = osal_strcmp(s1, s2);

	// assert
	assert_true(ret < 0);
}

/**
 * @brief Test `osal_strchr()`
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_str_unit_osal_strchr "osal_strchr() unit tests section"
 * - @ref specifications_osal_strchr "osal_strchr() specifications".
 */
static void test_osal_str_osal_strchr(void **state) {
	(void)state;

	// arrange
	const char *s = "this_will_be_cut_a1a2a3";
	int c = (int)'a';

	// act
	char *ret = osal_strchr(s, c);

	// assert
	assert_non_null(ret);
	assert_string_equal(ret, "a1a2a3");
}

/**
 * @brief Test `osal_strrchr()`
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_str_unit_osal_strrchr "osal_strrchr() unit tests section"
 * - @ref specifications_osal_strrchr "osal_strrchr() specifications".
 */
static void test_osal_str_osal_strrchr(void **state) {
	(void)state;

	// arrange
	const char *s = "this_will_be_cut_a1a2a3";
	int c = (int)'a';

	// act
	char *ret = osal_strrchr(s, c);

	// assert
	assert_non_null(ret);
	assert_string_equal(ret, "a3");
}

/**
 * @brief Test `osal_isspace()`
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_str_unit_osal_isspace "osal_isspace() unit tests section"
 * - @ref specifications_osal_isspace "osal_isspace() specifications".
 */
static void test_osal_str_osal_isspace(void **state) {
	(void)state;

	// arrange
	int c = (int)' ';

	// act
	int ret = osal_isspace(c);

	// assert
	assert_true(ret != 0);
}

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void)
{
	static const struct CMUnitTest osal_str_non_parametric_tests[] = {
		cmocka_unit_test(test_osal_str_default_ops),
		cmocka_unit_test(test_osal_str_osal_strlen),
		cmocka_unit_test(test_osal_str_osal_strcmp),
		cmocka_unit_test(test_osal_str_osal_strchr),
		cmocka_unit_test(test_osal_str_osal_strrchr),
		cmocka_unit_test(test_osal_str_osal_isspace)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(osal_str_non_parametric_tests, NULL, NULL);
	failed += cmocka_run_group_tests(osal_str_strdup_tests, NULL, NULL);
	failed += cmocka_run_group_tests(osal_str_snprintf_tests, NULL, NULL);

	return failed;
}

/** @endcond */
