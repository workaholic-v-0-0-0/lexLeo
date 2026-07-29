/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_lexleo_vm_cr.c
 * @ingroup lexleo_vm_unit_tests
 * @brief Unit tests implementation for lexleo_vm_cr.
 *
 * @details
 * This file implements the unit-level validation of lexleo_vm_cr.
 *
 * See also:
 * - @ref testing_foundation_lexleo_vm_cr_unit "lexleo_vm_cr.c unit tests page"
 * - @ref specifications_lexleo_vm "lexleo_vm specifications"
 */

#include "lexleo_vm/cr/lexleo_vm_cr_api.h"

//#include "lexleo_vm/tests/lexleo_vm_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**********************************************************************************************************************
 * @brief Test `lexleo_vm_default_cfg()`.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_default_cfg "lexleo_vm_default_cfg() specifications".
 *
 * See test description:
 * - @ref testing_foundation_lexleo_vm_unit_lexleo_vm_default_cfg "lexleo_vm_default_cfg() unit tests section"
 */
static void test_lexleo_vm_default_cfg(void **state) {
	(void)state;

	// ACT
	lexleo_vm_cfg_t ret = lexleo_vm_default_cfg();

	// ASSERT
	assert_int_equal(ret.reserved, 0);
}

/**********************************************************************************************************************
 * @brief Test `lexleo_vm_default_env()`.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_default_env "lexleo_vm_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_lexleo_vm_unit_lexleo_vm_default_env "lexleo_vm_default_env() unit tests section"
 */
static void test_lexleo_vm_default_env(void **state) {
	(void)state;

	// ARRANGE
	const osal_mem_ops_t *dummy_mem_ops = (const osal_mem_ops_t *)(uintptr_t)0x1234u;;
	const osal_stdio_ops_t *dummy_stdio_ops = (const osal_stdio_ops_t *)(uintptr_t)0x1234u;
	const osal_file_ops_t *dummy_file_ops = (const osal_file_ops_t *)(uintptr_t)0x1234u;
	const osal_str_ops_t *dummy_str_ops = (const osal_str_ops_t *)(uintptr_t)0x1234u;
	const osal_time_ops_t *dummy_time_ops = (const osal_time_ops_t *)(uintptr_t)0x1234u;
	stream_t *dummy_in = (stream_t *)(uintptr_t)0x1234u;
	stream_t *dummy_out = (stream_t *)(uintptr_t)0x1234u;
	stream_t *dummy_err = (stream_t *)(uintptr_t)0x1234u;
	logger_t *dummy_logger = (logger_t *)(uintptr_t)0x1234u;

	// ACT
	lexleo_vm_env_t ret =
		lexleo_vm_default_env(
			dummy_mem_ops,
			dummy_stdio_ops,
			dummy_file_ops,
			dummy_str_ops,
			dummy_time_ops,
			dummy_in,
			dummy_out,
			dummy_err,
			dummy_logger
		);

	// ASSERT
	assert_ptr_equal(ret.mem_ops, dummy_mem_ops);
	assert_ptr_equal(ret.stdio_ops, dummy_stdio_ops);
	assert_ptr_equal(ret.file_ops, dummy_file_ops);
	assert_ptr_equal(ret.str_ops, dummy_str_ops);
	assert_ptr_equal(ret.time_ops, dummy_time_ops);
	assert_ptr_equal(ret.in, dummy_in);
	assert_ptr_equal(ret.out, dummy_out);
	assert_ptr_equal(ret.err, dummy_err);
	assert_ptr_equal(ret.logger, dummy_logger);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `lexleo_vm_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_create "lexleo_vm_create() specifications".
 * - @ref specifications_lexleo_vm_complete_default_init "lexleo_vm_complete_default_init() specifications".
 * - @ref specifications_lexleo_vm_destroy "lexleo_vm_destroy() specifications".
 *
 * See test description:
 * - @ref testing_foundation_lexleo_vm_unit_lexleo_vm_t_lifecycle "`lexleo_vm_t` lifecycle unit tests section"
 */
typedef enum {
	LEXLEO_VM_T_LIFECYCLE_SCENARIO_LEXLEO_VM_HANDLE_OOM = 0,
	LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK_OWNED_RESOURCES_INJECTED_BY_TEST_INFRA,
	LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK_OWNED_RESOURCES_CREATED_BY_DEFAULT_INIT,
	LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK
} lexleo_vm_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	lexleo_vm_t_lifecycle_scenario_t scenario;
} test_lexleo_vm_t_lifecycle_case_t;

typedef struct {//<here> I'm wondering if I make a fake logger before? Then I will have to continue to adjust the following for this test
	fake_stream_t *fake_stream;
	fake_stream_adapter_t fake_stream_adapter;
	const test_lexleo_vm_t_lifecycle_case_t *tc;
} test_lexleo_vm_t_lifecycle_fixture_t;

static int setup_lexleo_vm_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(lexleo_vm_t_lifecycle, state, tc, fx);

	fake_stream_reset();
	fake_stream_adapter_reset_instance(&fx->fake_stream_adapter);
	fx->fake_stream = fake_stream_create_instance(&fx->fake_stream_adapter);
	fake_time_reset();
	fake_memory_reset();

	*state = fx;
	return 0;
}

static int teardown_lexleo_vm_t_lifecycle(void **state)
{
	test_lexleo_vm_t_lifecycle_fixture_t *fx = (test_lexleo_vm_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_lexleo_vm_t_lifecycle(void **state) {
	test_lexleo_vm_t_lifecycle_fixture_t *fx = (test_lexleo_vm_t_lifecycle_fixture_t *)(*state);
	const test_lexleo_vm_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	lexleo_vm_t *sentinel = (lexleo_vm_t *)(uintptr_t)0xDEADC0DEu;
	lexleo_vm_t *lexleo_vm = sentinel;
	const lexleo_vm_env_t lexleo_vm_env =
		lexleo_vm_default_env(
			fx->fake_stream,
			osal_time_test_fake_ops(),
			osal_mem_test_fake_ops()
		);
	if (tc->scenario == LEXLEO_VM_T_LIFECYCLE_SCENARIO_LEXLEO_VM_HANDLE_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	lexleo_vm_status_t ret = lexleo_vm_create(&lexleo_vm, &lexleo_vm_env);

	// ASSERT
	if (tc->scenario == LEXLEO_VM_T_LIFECYCLE_SCENARIO_LEXLEO_VM_HANDLE_OOM) {
		assert_int_equal(ret, LEXLEO_VM_STATUS_OOM);
		assert_ptr_equal(lexleo_vm, sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}
	assert_int_equal(ret, LEXLEO_VM_STATUS_OK);
	assert_true(lexleo_vm != sentinel);
	assert_non_null(lexleo_vm);
	assert_ptr_equal(lexleo_vm_get_stream(lexleo_vm), lexleo_vm_env.stream);
	assert_ptr_equal(lexleo_vm_get_time_ops(lexleo_vm), lexleo_vm_env.time_ops);
	assert_ptr_equal(lexleo_vm_get_mem_ops(lexleo_vm), lexleo_vm_env.mem_ops);

	// ARRANGE
	lexleo_vm_cfg_t lexleo_vm_cfg = lexleo_vm_default_cfg();
	const stream_t *stream_snapshot = lexleo_vm_get_stream(lexleo_vm);
	const osal_time_ops_t *time_ops_snapshot = lexleo_vm_get_time_ops(lexleo_vm);
	const osal_mem_ops_t *mem_ops_snapshot = lexleo_vm_get_mem_ops(lexleo_vm);

	// ACT
	ret = lexleo_vm_complete_default_init(lexleo_vm, &lexleo_vm_cfg);

	// ASSERT
	assert_int_equal(ret, LEXLEO_VM_STATUS_OK);
	assert_ptr_equal(lexleo_vm_get_stream(lexleo_vm), stream_snapshot);
	assert_ptr_equal(lexleo_vm_get_time_ops(lexleo_vm), time_ops_snapshot);
	assert_ptr_equal(lexleo_vm_get_mem_ops(lexleo_vm), mem_ops_snapshot);
	assert_true(lexleo_vm_get_append_newline_flag(lexleo_vm));

	// ACT
	logger_status_t destroy_ret = lexleo_vm_vtbl()->destroy(lexleo_vm);

	// ASSERT
	assert_int_equal(destroy_ret, LOGGER_STATUS_OK);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_lexleo_vm_t_lifecycle_case_t CASE_LEXLEO_VM_T_LIFECYCLE_LEXLEO_VM_HANDLE_OOM = {
	.name = "lexleo_vm_t_lifecycle_lexleo_vm_handle_oom",
	.scenario = LEXLEO_VM_T_LIFECYCLE_SCENARIO_LEXLEO_VM_HANDLE_OOM,
};

static const test_lexleo_vm_t_lifecycle_case_t CASE_LEXLEO_VM_T_LIFECYCLE_LEXLEO_VM_OK = {
	.name = "lexleo_vm_t_lifecycle_lexleo_vm_ok",
	.scenario = LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK,
};

#define LEXLEO_VM_T_LIFECYCLE_CASES(X) \
X(CASE_LEXLEO_VM_T_LIFECYCLE_LEXLEO_VM_HANDLE_OOM) \
X(CASE_LEXLEO_VM_T_LIFECYCLE_LEXLEO_VM_OK)

#define MAKE_LEXLEO_VM_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(lexleo_vm_t_lifecycle, case_sym)

static const struct CMUnitTest lexleo_vm_t_lifecycle_tests[] = {
	LEXLEO_VM_T_LIFECYCLE_CASES(MAKE_LEXLEO_VM_T_LIFECYCLE_TEST)
};

#undef LEXLEO_VM_T_LIFECYCLE_CASES
#undef MAKE_LEXLEO_VM_T_LIFECYCLE_TEST

/** @endcond */


//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest lexleo_vm_cr_tests_non_parametric[] = {
		cmocka_unit_test(test_lexleo_vm_default_cfg),
		cmocka_unit_test(test_lexleo_vm_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(lexleo_vm_cr_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(lexleo_vm_t_lifecycle_tests, NULL, NULL);

	return failed;
}
/** @endcond */
