/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_osal_time.c
 * @ingroup osal_time_unit_tests
 * @brief Unit tests for the public `osal_time` module.
 *
 * @details
 * This file contains unit tests for:
 * - `osal_time_default_ops()`
 * - `osal_time_ops_t::now()`
 */

#if !defined(__unix__) && !defined(__APPLE__)
#error "These osal_time unit tests are supported only on POSIX-like platforms."
#endif

#include "osal/time/osal_time_ops.h"

#include "osal/mem/osal_mem.h"

#include "lexleo_cmocka.h"

/**
 * @brief Test `osal_time_default_ops()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_time_unit_default_ops "osal_time_default_ops() unit tests section"
 * - @ref specifications_osal_time_default_ops "osal_time_default_ops() specifications".
 */
static void test_osal_time_default_ops(void **state)
{
	(void)state;

	const osal_time_ops_t *ret = osal_time_default_ops();

	assert_non_null(ret);
	assert_non_null(ret->now);
}

/**
 * @brief Test `osal_time_ops_t::now()` with `out == NULL`.
 *
 * No doubles.
 *
 * See contract:
 * - @ref testing_foundation_osal_time_unit_now "osal_time_ops_t::now() unit tests section"
 * - @ref specifications_osal_time_ops_now "osal_time_ops_t::now() specifications".
 */
static void test_osal_time_now_out_null(void **state)
{
	(void)state;

	// arrange
	const osal_time_ops_t *time_ops = osal_time_default_ops();

	// act
	osal_time_status_t ret = time_ops->now(NULL);

	// assert
	assert_int_equal(ret, OSAL_TIME_STATUS_INVALID);
}

/**
 * @brief Test `osal_time_ops_t::now()` success path.
 *
 * No doubles.
 *
 * See contract:
 * - @ref testing_foundation_osal_time_unit_now "osal_time_ops_t::now() unit tests section"
 * - @ref specifications_osal_time_ops_now "osal_time_ops_t::now() specifications".
 */
static void test_osal_time_now_success(void **state)
{
	(void)state;

	// arrange
	const osal_time_ops_t *time_ops = osal_time_default_ops();
	osal_time_t out;
	osal_memset(&out, 0, sizeof(out));

	// act
	osal_time_status_t ret = time_ops->now(&out);

	// assert
	assert_int_equal(ret, OSAL_TIME_STATUS_OK);
	assert_true(out.epoch_seconds != (int64_t)0);
}

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void)
{
	static const struct CMUnitTest osal_time_non_parametric_tests[] = {
		cmocka_unit_test(test_osal_time_default_ops),
		cmocka_unit_test(test_osal_time_now_out_null),
		cmocka_unit_test(test_osal_time_now_success)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(osal_time_non_parametric_tests, NULL, NULL);

	return failed;
}

/** @endcond */
