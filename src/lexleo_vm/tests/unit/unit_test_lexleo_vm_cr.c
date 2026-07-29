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

#include "lexleo_vm/tests/lexleo_vm_white_box_tests_access.h"

#include "lexleo/test/fake_stream.h"

#include "lexleo/test/fake_logger.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/test/osal_mem_fake_provider.h"
#include "osal/stdio/test/osal_stdio_fake_provider.h"
#include "osal/file/test/osal_file_fake_provider.h"
#include "osal/str/test/osal_str_fake_provider.h"
#include "osal/time/test/osal_time_fake_provider.h"

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
	LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK_OWNED_RESOURCES_CREATED_BY_DEFAULT_INIT
} lexleo_vm_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	lexleo_vm_t_lifecycle_scenario_t scenario;
} test_lexleo_vm_t_lifecycle_case_t;

typedef struct {
	fake_stream_t *fake_in;
	fake_stream_adapter_t fake_in_adapter;
	fake_stream_t *fake_out;
	fake_stream_adapter_t fake_out_adapter;
	fake_stream_t *fake_err;
	fake_stream_adapter_t fake_err_adapter;
	fake_logger_t *fake_logger;
	fake_logger_adapter_t fake_logger_adapter;
	const test_lexleo_vm_t_lifecycle_case_t *tc;
} test_lexleo_vm_t_lifecycle_fixture_t;

static int setup_lexleo_vm_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(lexleo_vm_t_lifecycle, state, tc, fx);

	fake_stream_reset();

	fake_stream_adapter_reset_instance(&fx->fake_in_adapter);
	fx->fake_in = fake_stream_create_instance(&fx->fake_in_adapter);

	fake_stream_adapter_reset_instance(&fx->fake_out_adapter);
	fx->fake_out = fake_stream_create_instance(&fx->fake_out_adapter);

	fake_stream_adapter_reset_instance(&fx->fake_err_adapter);
	fx->fake_err = fake_stream_create_instance(&fx->fake_err_adapter);

	fake_logger_reset();

	fake_logger_adapter_reset_instance(&fx->fake_logger_adapter);
	fx->fake_logger = fake_logger_create_instance(&fx->fake_logger_adapter);

	fake_memory_reset();

	fake_str_reset();

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
	lexleo_vm_t *lexleo_vm = NULL;
	if (tc->scenario == LEXLEO_VM_T_LIFECYCLE_SCENARIO_LEXLEO_VM_HANDLE_OOM) { fake_memory_fail_only_on_call(1); }
	lexleo_vm_env_t lexleo_vm_env =
		lexleo_vm_default_env(
			osal_mem_test_fake_ops(),
			osal_stdio_test_fake_ops(),
			osal_file_test_fake_ops(),
			osal_str_test_fake_ops(),
			osal_time_test_fake_ops(),
			fx->fake_in,
			fx->fake_out,
			fx->fake_err,
			fx->fake_logger
		);

	// ACT
	lexleo_vm_status_t ret = lexleo_vm_create(&lexleo_vm, &lexleo_vm_env);

	// ASSERT
	if (tc->scenario == LEXLEO_VM_T_LIFECYCLE_SCENARIO_LEXLEO_VM_HANDLE_OOM) {
		assert_int_equal(ret, LEXLEO_VM_STATUS_OOM);
		assert_null(lexleo_vm);
		return;
	}
	assert_int_equal(ret, LEXLEO_VM_STATUS_OK);
	assert_non_null(lexleo_vm);
	assert_ptr_equal(lexleo_vm_get_mem_ops(lexleo_vm), lexleo_vm_env.mem_ops);
	assert_ptr_equal(lexleo_vm_get_stdio_ops(lexleo_vm), lexleo_vm_env.stdio_ops);
	assert_ptr_equal(lexleo_vm_get_file_ops(lexleo_vm), lexleo_vm_env.file_ops);
	assert_ptr_equal(lexleo_vm_get_str_ops(lexleo_vm), lexleo_vm_env.str_ops);
	assert_ptr_equal(lexleo_vm_get_time_ops(lexleo_vm), lexleo_vm_env.time_ops);
	assert_ptr_equal(lexleo_vm_get_in(lexleo_vm), lexleo_vm_env.in);
	assert_ptr_equal(lexleo_vm_get_out(lexleo_vm), lexleo_vm_env.out);
	assert_ptr_equal(lexleo_vm_get_err(lexleo_vm), lexleo_vm_env.err);
	assert_ptr_equal(lexleo_vm_get_logger(lexleo_vm), lexleo_vm_env.logger);
	assert_non_null(lexleo_vm_get_lexleo_vm_owned_resources(lexleo_vm));
	assert_null(lexleo_vm_get_stream_factory(lexleo_vm));
	assert_null(lexleo_vm_get_stream_standard_stream_creator(lexleo_vm));
	assert_null(lexleo_vm_get_stream_regular_file_creator(lexleo_vm));
	assert_null(lexleo_vm_get_stream_dynamic_buffer_creator(lexleo_vm));

	// ARRANGE
	if (tc->scenario == LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK_OWNED_RESOURCES_INJECTED_BY_TEST_INFRA) {
		lexleo_vm_inject_stream_factory(lexleo_vm, g_fake_stream_factory);
		lexleo_vm_inject_stream_standard_stream_creator(lexleo_vm, g_fake_stream_standard_stream_creator);
		lexleo_vm_inject_stream_regular_file_creator(lexleo_vm, g_fake_stream_regular_file_creator);
		lexleo_vm_inject_stream_dynamic_buffer_creator(lexleo_vm, g_fake_stream_dynamic_buffer_creator);
	}
	lexleo_vm_cfg_t lexleo_vm_cfg = lexleo_vm_default_cfg();

	// ACT
	ret = lexleo_vm_complete_default_init(lexleo_vm, &lexleo_vm_cfg);

	// ASSERT
	assert_int_equal(ret, LEXLEO_VM_STATUS_OK);
	stream_factory_t *stream_factory = lexleo_vm_get_stream_factory(lexleo_vm);
	stream_standard_stream_creator_t *stream_standard_stream_creator = lexleo_vm_get_stream_standard_stream_creator(lexleo_vm);
	stream_regular_file_creator_t *stream_regular_file_creator = lexleo_vm_get_stream_regular_file_creator(lexleo_vm);
	stream_dynamic_buffer_creator_t *stream_dynamic_buffer_creator = lexleo_vm_get_stream_dynamic_buffer_creator(lexleo_vm);
	switch (tc->scenario) {
		case LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK_OWNED_RESOURCES_INJECTED_BY_TEST_INFRA: {
			assert_ptr_equal(stream_factory, g_fake_stream_factory);
			assert_ptr_equal(stream_standard_stream_creator, g_fake_stream_standard_stream_creator);
			assert_ptr_equal(stream_regular_file_creator, g_fake_stream_regular_file_creator);
			assert_ptr_equal(stream_dynamic_buffer_creator, g_fake_stream_dynamic_buffer_creator);
			break;
		}
		case LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK_OWNED_RESOURCES_CREATED_BY_DEFAULT_INIT: {
			assert_non_null(stream_factory);
			assert_ptr_not_equal(stream_factory, g_fake_stream_factory);
			assert_non_null(stream_standard_stream_creator);
			assert_ptr_not_equal(stream_standard_stream_creator, g_fake_stream_standard_stream_creator);
			assert_non_null(stream_regular_file_creator);
			assert_ptr_not_equal(stream_regular_file_creator, g_fake_stream_regular_file_creator);
			assert_non_null(stream_dynamic_buffer_creator);
			assert_ptr_not_equal(stream_dynamic_buffer_creator, g_fake_stream_dynamic_buffer_creator);
			break;
		}
		default: {
			fail();
		}
	}

	// ACT
	lexleo_vm_destroy(&lexleo_vm);

	// ASSERT
	assert_null(lexleo_vm);
	assert_int_equal(fx->fake_in_adapter.close_call_count, 0);
	assert_int_equal(fx->fake_out_adapter.close_call_count, 0);
	assert_int_equal(fx->fake_err_adapter.close_call_count, 0);
	assert_int_equal(fx->fake_logger_adapter.destroy_call_count, 0);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_lexleo_vm_t_lifecycle_case_t CASE_LEXLEO_VM_T_LIFECYCLE_LEXLEO_VM_HANDLE_OOM = {
	.name = "lexleo_vm_t_lifecycle_lexleo_vm_handle_oom",
	.scenario = LEXLEO_VM_T_LIFECYCLE_SCENARIO_LEXLEO_VM_HANDLE_OOM,
};

static const test_lexleo_vm_t_lifecycle_case_t CASE_LEXLEO_VM_T_LIFECYCLE_OK_OWNED_RESOURCES_INJECTED_BY_TEST_INFRA = {
	.name = "lexleo_vm_t_lifecycle_ok_owned_resources_injected_by_test_infra",
	.scenario = LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK_OWNED_RESOURCES_INJECTED_BY_TEST_INFRA,
};

static const test_lexleo_vm_t_lifecycle_case_t CASE_LEXLEO_VM_T_LIFECYCLE_OK_OWNED_RESOURCES_CREATED_BY_DEFAULT_INIT = {
	.name = "lexleo_vm_t_lifecycle_ok_owned_resources_created_by_default_init",
	.scenario = LEXLEO_VM_T_LIFECYCLE_SCENARIO_OK_OWNED_RESOURCES_CREATED_BY_DEFAULT_INIT,
};

#define LEXLEO_VM_T_LIFECYCLE_CASES(X) \
X(CASE_LEXLEO_VM_T_LIFECYCLE_LEXLEO_VM_HANDLE_OOM) \
X(CASE_LEXLEO_VM_T_LIFECYCLE_OK_OWNED_RESOURCES_INJECTED_BY_TEST_INFRA) \
X(CASE_LEXLEO_VM_T_LIFECYCLE_OK_OWNED_RESOURCES_CREATED_BY_DEFAULT_INIT)

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
