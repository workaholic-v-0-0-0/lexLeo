/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_osal_stdio.c
 * @ingroup osal_stdio_unit_tests
 * @brief Unit tests for the public `osal_stdio` module.
 *
 * @details
 * This file contains unit tests for:
 * - `osal_stdio_default_ops()`
 * - `osal_stdio_ops_t::get_stdin()`
 * - `osal_stdio_ops_t::get_stdout()`
 * - `osal_stdio_ops_t::get_stderr()`
 */

#if !defined(__unix__) && !defined(__APPLE__)
#error "These osal_stdio unit tests are supported only on POSIX-like platforms."
#endif

#include "osal/stdio/osal_stdio_ops.h"

#include "lexleo_cmocka.h"

/**
 * @brief Test `osal_stdio_default_ops()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_stdio_unit_default_ops "osal_stdio_default_ops() unit tests section"
 * - @ref specifications_osal_stdio_default_ops "osal_stdio_default_ops() specifications".
 */
static void test_osal_stdio_default_ops(void **state)
{
	(void)state;

	const osal_stdio_ops_t *ret = osal_stdio_default_ops();

	assert_non_null(ret);
	assert_non_null(ret->get_stdin);
	assert_non_null(ret->get_stdout);
	assert_non_null(ret->get_stderr);
	assert_non_null(ret->read);
	assert_non_null(ret->write);
	assert_non_null(ret->flush);
	assert_non_null(ret->error);
	assert_non_null(ret->eof);
}

/**
 * @brief Test `osal_stdio_ops_t::get_stdin()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_stdio_unit_get_stdin "osal_stdio_ops_t::get_stdin() unit tests section"
 * - @ref specifications_osal_stdio_get_stdin "osal_stdio_ops_t::get_stdin() specifications".
 */
static void test_osal_stdio_get_stdin(void **state)
{
	(void)state;

	// arrange
	const osal_stdio_ops_t *stdio_ops = osal_stdio_default_ops();
	assert_non_null(stdio_ops);
	assert_non_null(stdio_ops->get_stdin);

	// act
	OSAL_STDIO *ret_1 = stdio_ops->get_stdin();
	OSAL_STDIO *ret_2 = stdio_ops->get_stdin();

	// assert
	assert_non_null(ret_1);
	assert_non_null(ret_2);
	assert_ptr_equal(ret_1, ret_2);
}

/**
 * @brief Test `osal_stdio_ops_t::get_stdout()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_stdio_unit_get_stdout "osal_stdio_ops_t::get_stdout() unit tests section"
 * - @ref specifications_osal_stdio_get_stdout "osal_stdio_ops_t::get_stdout() specifications".
 */
static void test_osal_stdio_get_stdout(void **state)
{
	(void)state;

	// arrange
	const osal_stdio_ops_t *stdio_ops = osal_stdio_default_ops();
	assert_non_null(stdio_ops);
	assert_non_null(stdio_ops->get_stdout);

	// act
	OSAL_STDIO *ret_1 = stdio_ops->get_stdout();
	OSAL_STDIO *ret_2 = stdio_ops->get_stdout();

	// assert
	assert_non_null(ret_1);
	assert_non_null(ret_2);
	assert_ptr_equal(ret_1, ret_2);
}

/**
 * @brief Test `osal_stdio_ops_t::get_stderr()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_stdio_unit_get_stderr "osal_stdio_ops_t::get_stderr() unit tests section"
 * - @ref specifications_osal_stdio_get_stderr "osal_stdio_ops_t::get_stderr() specifications".
 */
static void test_osal_stdio_get_stderr(void **state)
{
	(void)state;

	// arrange
	const osal_stdio_ops_t *stdio_ops = osal_stdio_default_ops();
	assert_non_null(stdio_ops);
	assert_non_null(stdio_ops->get_stderr);

	// act
	OSAL_STDIO *ret_1 = stdio_ops->get_stderr();
	OSAL_STDIO *ret_2 = stdio_ops->get_stderr();

	// assert
	assert_non_null(ret_1);
	assert_non_null(ret_2);
	assert_ptr_equal(ret_1, ret_2);
}

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void)
{
	static const struct CMUnitTest osal_stdio_non_parametric_tests[] = {
		cmocka_unit_test(test_osal_stdio_default_ops),
		cmocka_unit_test(test_osal_stdio_get_stdin),
		cmocka_unit_test(test_osal_stdio_get_stdout),
		cmocka_unit_test(test_osal_stdio_get_stderr)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(osal_stdio_non_parametric_tests, NULL, NULL);

	return failed;
}

/** @endcond */
