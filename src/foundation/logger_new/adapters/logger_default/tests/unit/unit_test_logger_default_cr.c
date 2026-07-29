/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_logger_default_cr.c
 * @ingroup logger_default_unit_tests
 * @brief Unit tests implementation for logger_default_cr.c.
 *
 * See also:
 * - @ref testing_foundation_logger_default_cr_unit "logger_default_cr.c unit tests page"
 * - @ref specifications_logger_default "logger_default specifications"
 */

#include "logger_default/cr/logger_default_cr_api.h"
#include "logger/cr/logger_cr_api.h"

#include "logger_default/tests/logger_default_white_box_tests_access.h"

#include "lexleo/test/fake_stream.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "osal/time/test/osal_time_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**********************************************************************************************************************
 * @brief Test `logger_default_vtbl()`.
 *
 * See contract:
 * - @ref specifications_logger_default_vtbl "logger_default_vtbl() specifications".
 *
 * See test description:
 * - @ref testing_foundation_logger_default_unit_logger_default_vtbl "logger_default_vtbl() unit tests section"
 */
static void test_logger_default_vtbl(void **state) {
	(void)state;

	// ACT
	const logger_vtbl_t *ret = logger_default_vtbl();

	// ASSERT
	assert_non_null(ret);
	assert_non_null(ret->log);
	assert_non_null(ret->destroy);
}

/**********************************************************************************************************************
 * @brief Test `logger_default_default_cfg()`.
 *
 * See contract:
 * - @ref specifications_logger_default_default_cfg "logger_default_default_cfg() specifications".
 *
 * See test description:
 * - @ref testing_foundation_logger_default_unit_logger_default_default_cfg "logger_default_default_cfg() unit tests section"
 */
static void test_logger_default_default_cfg(void **state) {
	(void)state;

	// ACT
	logger_default_cfg_t ret = logger_default_default_cfg();

	// ASSERT
	assert_true(ret.append_newline);
}

/**********************************************************************************************************************
 * @brief Test `logger_default_default_env()`.
 *
 * See contract:
 * - @ref specifications_logger_default_default_env "logger_default_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_logger_default_unit_logger_default_default_env "logger_default_default_env() unit tests section"
 */
static void test_logger_default_default_env(void **state) {
	(void)state;

	// ARRANGE
	stream_t *dummy_stream = (stream_t *)(uintptr_t)0xDEADBEEFu;
	const osal_time_ops_t *dummy_time_ops = (const osal_time_ops_t *)(uintptr_t)0xDEADC0DEu;
	const osal_mem_ops_t *dummy_mem_ops = (const osal_mem_ops_t *)(uintptr_t)0xBADC0FFEu;

	// ACT
	logger_default_env_t ret = logger_default_default_env(dummy_stream, dummy_time_ops, dummy_mem_ops);

	// ASSERT
	assert_ptr_equal(ret.stream, dummy_stream);
	assert_ptr_equal(ret.time_ops, dummy_time_ops);
	assert_ptr_equal(ret.mem_ops, dummy_mem_ops);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `logger_default_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_logger_default_create "logger_default_create() specifications".
 * - @ref specifications_logger_default_complete_default_init "logger_default_complete_default_init() specifications".
 * - @ref specifications_logger_default_destroy "logger_default_destroy() specifications".
 * - @ref specifications_logger_default_vtbl "logger_default_vtbl() specifications".
 * - @ref specifications_logger_default_default_cfg "logger_default_default_cfg() specifications".
 * - @ref specifications_logger_default_default_env "logger_default_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_logger_default_unit_logger_default_t_lifecycle "`logger_default_t` lifecycle unit tests section"
 */
typedef enum {
	LOGGER_DEFAULT_T_LIFECYCLE_SCENARIO_LOGGER_DEFAULT_HANDLE_OOM = 0,
	LOGGER_DEFAULT_T_LIFECYCLE_SCENARIO_OK
} logger_default_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	logger_default_t_lifecycle_scenario_t scenario;
} test_logger_default_t_lifecycle_case_t;

typedef struct {
	fake_stream_t *fake_stream;
	fake_stream_adapter_t fake_stream_adapter;
	const test_logger_default_t_lifecycle_case_t *tc;
} test_logger_default_t_lifecycle_fixture_t;

static int setup_logger_default_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(logger_default_t_lifecycle, state, tc, fx);

	fake_stream_reset();
	fake_stream_adapter_reset_instance(&fx->fake_stream_adapter);
	fx->fake_stream = fake_stream_create_instance(&fx->fake_stream_adapter);
	fake_time_reset();
	fake_memory_reset();

	*state = fx;
	return 0;
}

static int teardown_logger_default_t_lifecycle(void **state)
{
	test_logger_default_t_lifecycle_fixture_t *fx = (test_logger_default_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_logger_default_t_lifecycle(void **state) {
	test_logger_default_t_lifecycle_fixture_t *fx = (test_logger_default_t_lifecycle_fixture_t *)(*state);
	const test_logger_default_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	logger_default_t *sentinel = (logger_default_t *)(uintptr_t)0xDEADC0DEu;
	logger_default_t *logger_default = sentinel;
	const logger_default_env_t logger_default_env =
		logger_default_default_env(
			fx->fake_stream,
			osal_time_test_fake_ops(),
			osal_mem_test_fake_ops()
		);
	if (tc->scenario == LOGGER_DEFAULT_T_LIFECYCLE_SCENARIO_LOGGER_DEFAULT_HANDLE_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	logger_default_status_t ret = logger_default_create(&logger_default, &logger_default_env);

	// ASSERT
	if (tc->scenario == LOGGER_DEFAULT_T_LIFECYCLE_SCENARIO_LOGGER_DEFAULT_HANDLE_OOM) {
		assert_int_equal(ret, LOGGER_DEFAULT_STATUS_OOM);
		assert_ptr_equal(logger_default, sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}
	assert_int_equal(ret, LOGGER_DEFAULT_STATUS_OK);
	assert_true(logger_default != sentinel);
	assert_non_null(logger_default);
	assert_ptr_equal(logger_default_get_stream(logger_default), logger_default_env.stream);
	assert_ptr_equal(logger_default_get_time_ops(logger_default), logger_default_env.time_ops);
	assert_ptr_equal(logger_default_get_mem_ops(logger_default), logger_default_env.mem_ops);

	// ARRANGE
	logger_default_cfg_t logger_default_cfg = logger_default_default_cfg();
	const stream_t *stream_snapshot = logger_default_get_stream(logger_default);
	const osal_time_ops_t *time_ops_snapshot = logger_default_get_time_ops(logger_default);
	const osal_mem_ops_t *mem_ops_snapshot = logger_default_get_mem_ops(logger_default);

	// ACT
	ret = logger_default_complete_default_init(logger_default, &logger_default_cfg);

	// ASSERT
	assert_int_equal(ret, LOGGER_DEFAULT_STATUS_OK);
	assert_ptr_equal(logger_default_get_stream(logger_default), stream_snapshot);
	assert_ptr_equal(logger_default_get_time_ops(logger_default), time_ops_snapshot);
	assert_ptr_equal(logger_default_get_mem_ops(logger_default), mem_ops_snapshot);
	assert_true(logger_default_get_append_newline_flag(logger_default));

	// ACT
	logger_status_t destroy_ret = logger_default_vtbl()->destroy(logger_default);

	// ASSERT
	assert_int_equal(destroy_ret, LOGGER_STATUS_OK);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_logger_default_t_lifecycle_case_t CASE_LOGGER_DEFAULT_T_LIFECYCLE_LOGGER_DEFAULT_HANDLE_OOM = {
	.name = "logger_default_t_lifecycle_logger_default_handle_oom",
	.scenario = LOGGER_DEFAULT_T_LIFECYCLE_SCENARIO_LOGGER_DEFAULT_HANDLE_OOM,
};

static const test_logger_default_t_lifecycle_case_t CASE_LOGGER_DEFAULT_T_LIFECYCLE_LOGGER_DEFAULT_OK = {
	.name = "logger_default_t_lifecycle_logger_default_ok",
	.scenario = LOGGER_DEFAULT_T_LIFECYCLE_SCENARIO_OK,
};

#define LOGGER_DEFAULT_T_LIFECYCLE_CASES(X) \
X(CASE_LOGGER_DEFAULT_T_LIFECYCLE_LOGGER_DEFAULT_HANDLE_OOM) \
X(CASE_LOGGER_DEFAULT_T_LIFECYCLE_LOGGER_DEFAULT_OK)

#define MAKE_LOGGER_DEFAULT_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(logger_default_t_lifecycle, case_sym)

static const struct CMUnitTest logger_default_t_lifecycle_tests[] = {
	LOGGER_DEFAULT_T_LIFECYCLE_CASES(MAKE_LOGGER_DEFAULT_T_LIFECYCLE_TEST)
};

#undef LOGGER_DEFAULT_T_LIFECYCLE_CASES
#undef MAKE_LOGGER_DEFAULT_T_LIFECYCLE_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest logger_default_cr_tests_non_parametric[] = {
		cmocka_unit_test(test_logger_default_vtbl),
		cmocka_unit_test(test_logger_default_default_cfg),
		cmocka_unit_test(test_logger_default_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(logger_default_cr_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(logger_default_t_lifecycle_tests, NULL, NULL);

	return failed;
}
/** @endcond */
