/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_lexleo_app_cr.c
 * @ingroup lexleo_app_unit_tests
 * @brief Unit tests implementation for lexleo_app_cr.
 *
 * @details
 * This file implements the unit-level validation of lexleo_app_cr.
 *
 * See also:
 * - @ref testing_foundation_lexleo_app_cr_unit "lexleo_app_cr.c unit tests page"
 * - @ref specifications_lexleo_app "lexleo_app specifications"
 */

#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_api.h"

#include "lexleo_app/tests/lexleo_app_white_box_tests_access_cr_wrapper.h"
#include "lexleo_app/tests/lexleo_app_white_box_tests_access.h"

#include "lexleo/test/fake_stream.h"

#include "lexleo/test/fake_logger.h"

#include "lexleo/test/fake_vm.h"

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
 * @brief Test `lexleo_app_default_cfg()`.
 *
 * See contract:
 * - @ref specifications_lexleo_app_default_cfg "lexleo_app_default_cfg() specifications".
 *
 * See test description:
 * - @ref testing_foundation_lexleo_app_unit_lexleo_app_default_cfg "lexleo_app_default_cfg() unit tests section"
 */
static void test_lexleo_app_default_cfg(void **state) {
	(void)state;

	// ACT
	lexleo_app_cfg_t ret = lexleo_app_default_cfg();

	// ASSERT
	assert_int_equal(ret.in.kind, LEXLEO_APP_IO_STDIO);
	assert_null(ret.in.path);
	assert_null(ret.in.mode);
	assert_int_equal(ret.out.kind, LEXLEO_APP_IO_STDIO);
	assert_null(ret.out.path);
	assert_null(ret.out.mode);
	assert_int_equal(ret.err.kind, LEXLEO_APP_IO_STDIO);
	assert_null(ret.err.path);
	assert_null(ret.err.mode);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `lexleo_app_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_lexleo_app_create_init "lexleo_app_create_init() specifications".
 * - @ref specifications_lexleo_app_destroy "lexleo_app_destroy() specifications".
 *
 * See test description:
 * - @ref testing_foundation_lexleo_app_unit_lexleo_app_t_lifecycle "`lexleo_app_t` lifecycle unit tests section"
 */
typedef enum {
	LEXLEO_APP_T_LIFECYCLE_SCENARIO_LEXLEO_APP_HANDLE_OOM = 0,
	LEXLEO_APP_T_LIFECYCLE_SCENARIO_OK_INTERNAL_FIELDS_INJECTED_BY_TEST_INFRA,
} lexleo_app_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	lexleo_app_t_lifecycle_scenario_t scenario;
} test_lexleo_app_t_lifecycle_case_t;

typedef struct {
	stream_t *fake_in;
	fake_stream_adapter_t fake_in_adapter;
	stream_t *fake_out;
	fake_stream_adapter_t fake_out_adapter;
	stream_t *fake_err;
	fake_stream_adapter_t fake_err_adapter;
	stream_t *fake_logger_stream;
	fake_stream_adapter_t fake_logger_stream_adapter;
	logger_t *fake_logger;
	fake_logger_adapter_t fake_logger_adapter;
	lexleo_vm_t *fake_vm;

	const test_lexleo_app_t_lifecycle_case_t *tc;
} test_lexleo_app_t_lifecycle_fixture_t;

static int setup_lexleo_app_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(lexleo_app_t_lifecycle, state, tc, fx);

	fake_stream_reset();

	fake_stream_adapter_reset_instance(&fx->fake_in_adapter);
	fx->fake_in = fake_stream_create_instance(&fx->fake_in_adapter);

	fake_stream_adapter_reset_instance(&fx->fake_out_adapter);
	fx->fake_out = fake_stream_create_instance(&fx->fake_out_adapter);

	fake_stream_adapter_reset_instance(&fx->fake_err_adapter);
	fx->fake_err = fake_stream_create_instance(&fx->fake_err_adapter);

	fake_stream_adapter_reset_instance(&fx->fake_logger_stream_adapter);
	fx->fake_logger_stream = fake_stream_create_instance(&fx->fake_logger_stream_adapter);

	fake_logger_reset();

	fake_logger_adapter_reset_instance(&fx->fake_logger_adapter);
	fx->fake_logger = fake_logger_create_instance(&fx->fake_logger_adapter);

	fake_memory_reset();

	fake_str_reset();

	fake_vm_reset();
	fx->fake_vm = fake_vm_fake_to_real(&g_fake_vm);

	*state = fx;
	return 0;
}

static int teardown_lexleo_app_t_lifecycle(void **state)
{
	test_lexleo_app_t_lifecycle_fixture_t *fx = (test_lexleo_app_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_lexleo_app_t_lifecycle(void **state) {
	test_lexleo_app_t_lifecycle_fixture_t *fx = (test_lexleo_app_t_lifecycle_fixture_t *)(*state);
	const test_lexleo_app_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	lexleo_app_t *lexleo_app = NULL;
	if (tc->scenario == LEXLEO_APP_T_LIFECYCLE_SCENARIO_LEXLEO_APP_HANDLE_OOM) { fake_memory_fail_only_on_call(1); }
	lexleo_app_env_t *lexleo_app_env =
		lexleo_app_test_default_env_p(
			osal_mem_test_fake_ops(),
			osal_stdio_test_fake_ops(),
			osal_file_test_fake_ops(),
			osal_str_test_fake_ops(),
			osal_time_test_fake_ops()
		);

	// ACT
	lexleo_app_cr_test_status_t ret = lexleo_app_test_create(&lexleo_app, lexleo_app_env);

	// ASSERT
	if (tc->scenario == LEXLEO_APP_T_LIFECYCLE_SCENARIO_LEXLEO_APP_HANDLE_OOM) {
		assert_int_equal(ret, LEXLEO_APP_TEST_STATUS_OOM);
		assert_null(lexleo_app);
		return;
	}
	assert_int_equal(ret, LEXLEO_APP_TEST_STATUS_OK);
	assert_non_null(lexleo_app);
	assert_ptr_equal(lexleo_app_get_mem_ops(lexleo_app), osal_mem_test_fake_ops());
	assert_ptr_equal(lexleo_app_get_stdio_ops(lexleo_app), osal_stdio_test_fake_ops());
	assert_ptr_equal(lexleo_app_get_file_ops(lexleo_app), osal_file_test_fake_ops());
	assert_ptr_equal(lexleo_app_get_str_ops(lexleo_app), osal_str_test_fake_ops());
	assert_ptr_equal(lexleo_app_get_time_ops(lexleo_app), osal_time_test_fake_ops());
	assert_null(lexleo_app_get_in(lexleo_app));
	assert_null(lexleo_app_get_out(lexleo_app));
	assert_null(lexleo_app_get_err(lexleo_app));
	assert_string_equal(lexleo_app_get_log_path(lexleo_app), "");
	assert_null(lexleo_app_get_logger_stream(lexleo_app));
	assert_null(lexleo_app_get_logger(lexleo_app));
	assert_null(lexleo_app_get_vm(lexleo_app));

	// ARRANGE
	lexleo_app_inject_in(lexleo_app, fx->fake_in);
	lexleo_app_inject_out(lexleo_app, fx->fake_out);
	lexleo_app_inject_err(lexleo_app, fx->fake_err);
	const char *dummy_log_path = "dummy/log/path";
	lexleo_app_set_log_path(lexleo_app, dummy_log_path);
	lexleo_app_inject_logger_stream(lexleo_app, fx->fake_logger_stream);
	lexleo_app_inject_logger(lexleo_app, fx->fake_logger);
	lexleo_app_inject_vm(lexleo_app, fx->fake_vm);
	lexleo_app_cfg_t lexleo_app_cfg = lexleo_app_default_cfg();

	// ACT
	ret = lexleo_app_test_complete_default_init(lexleo_app, &lexleo_app_cfg);

	// ASSERT
	assert_int_equal(ret, LEXLEO_APP_TEST_STATUS_OK);
	assert_ptr_equal(lexleo_app_get_mem_ops(lexleo_app), osal_mem_test_fake_ops());
	assert_ptr_equal(lexleo_app_get_stdio_ops(lexleo_app), osal_stdio_test_fake_ops());
	assert_ptr_equal(lexleo_app_get_file_ops(lexleo_app), osal_file_test_fake_ops());
	assert_ptr_equal(lexleo_app_get_str_ops(lexleo_app), osal_str_test_fake_ops());
	assert_ptr_equal(lexleo_app_get_time_ops(lexleo_app), osal_time_test_fake_ops());

	assert_ptr_equal(lexleo_app_get_in(lexleo_app), fx->fake_in);
	assert_ptr_equal(lexleo_app_get_out(lexleo_app), fx->fake_out);
	assert_ptr_equal(lexleo_app_get_err(lexleo_app), fx->fake_err);
	assert_string_equal(lexleo_app_get_log_path(lexleo_app), dummy_log_path);
	assert_ptr_equal(lexleo_app_get_logger_stream(lexleo_app), fx->fake_logger_stream);
	assert_ptr_equal(lexleo_app_get_logger(lexleo_app), fx->fake_logger);
	assert_ptr_equal(lexleo_app_get_vm(lexleo_app), fx->fake_vm);

	// ACT
	lexleo_app_destroy(&lexleo_app);

	// ASSERT
	assert_null(lexleo_app);
	assert_int_equal(fx->fake_in_adapter.close_call_count, 1);
	assert_int_equal(fx->fake_out_adapter.close_call_count, 1);
	assert_int_equal(fx->fake_err_adapter.close_call_count, 1);
	assert_int_equal(fx->fake_logger_adapter.destroy_call_count, 1);
	assert_int_equal(fx->fake_logger_stream_adapter.close_call_count, 1);
	assert_int_equal(g_fake_vm_ctrl.destroy_call_count, 1);
	assert_ptr_equal(g_fake_vm_ctrl.last_destroy_vm_value, fx->fake_vm);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_lexleo_app_t_lifecycle_case_t CASE_LEXLEO_APP_T_LIFECYCLE_LEXLEO_APP_HANDLE_OOM = {
	.name = "lexleo_app_t_lifecycle_lexleo_app_handle_oom",
	.scenario = LEXLEO_APP_T_LIFECYCLE_SCENARIO_LEXLEO_APP_HANDLE_OOM,
};

static const test_lexleo_app_t_lifecycle_case_t CASE_LEXLEO_APP_T_LIFECYCLE_OK_INTERNAL_FIELDS_INJECTED_BY_TEST_INFRA = {
	.name = "lexleo_app_t_lifecycle_ok_internal_fields_injected_by_test_infra",
	.scenario = LEXLEO_APP_T_LIFECYCLE_SCENARIO_OK_INTERNAL_FIELDS_INJECTED_BY_TEST_INFRA,
};

#define LEXLEO_APP_T_LIFECYCLE_CASES(X) \
X(CASE_LEXLEO_APP_T_LIFECYCLE_LEXLEO_APP_HANDLE_OOM) \
X(CASE_LEXLEO_APP_T_LIFECYCLE_OK_INTERNAL_FIELDS_INJECTED_BY_TEST_INFRA)

#define MAKE_LEXLEO_APP_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(lexleo_app_t_lifecycle, case_sym)

static const struct CMUnitTest lexleo_app_t_lifecycle_tests[] = {
	LEXLEO_APP_T_LIFECYCLE_CASES(MAKE_LEXLEO_APP_T_LIFECYCLE_TEST)
};

#undef LEXLEO_APP_T_LIFECYCLE_CASES
#undef MAKE_LEXLEO_APP_T_LIFECYCLE_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest lexleo_app_cr_tests_non_parametric[] = {
		cmocka_unit_test(test_lexleo_app_default_cfg),
	};

	int failed = 0;
	failed += cmocka_run_group_tests(lexleo_app_cr_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(lexleo_app_t_lifecycle_tests, NULL, NULL);

	return failed;
}
/** @endcond */
