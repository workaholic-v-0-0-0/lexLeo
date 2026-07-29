/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_logger_cr.c
 * @ingroup logger_unit_tests
 * @brief Unit tests implementation for logger_cr.
 *
 * @details
 * This file implements the unit-level validation of logger_cr.
 *
 * See also:
 * - @ref testing_foundation_logger_cr_unit "logger_cr.c unit tests page"
 * - @ref specifications_logger "logger specifications"
 */

#include "logger/cr/logger_cr_api.h"

#include "logger/tests/logger_fake_adapter.h"
#include "logger/tests/logger_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**********************************************************************************************************************
 * @brief Test `logger_default_env()`.
 *
 * See contract:
 * - @ref specifications_logger_default_env "logger_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_logger_unit_logger_default_env "logger_default_env() unit tests section"
 */
static void test_logger_default_env(void **state) {
	(void)state;

	// ARRANGE
	const logger_vtbl_t dummy_vtbl = {0};
	const logger_vtbl_t *dummy_vtbl_p = &dummy_vtbl;
	const osal_mem_ops_t dummy_mem_ops = {0};
	const osal_mem_ops_t *dummy_mem_ops_p = &dummy_mem_ops;

	// ACT
	logger_env_t ret = logger_default_env(dummy_vtbl_p, dummy_mem_ops_p);

	// ASSERT
	assert_ptr_equal(ret.mem_ops, dummy_mem_ops_p);
	assert_ptr_equal(ret.vtbl, dummy_vtbl_p);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `logger_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_logger_create "logger_create() specifications".
 * - @ref specifications_logger_complete_default_init "logger_complete_default_init() specifications".
 * - @ref specifications_logger_destroy "logger_destroy() specifications"
 *
 * See test description:
 * - @ref testing_foundation_logger_unit_logger_t_lifecycle "`logger_t` lifecycle unit tests section"
 */
typedef enum {
	LOGGER_T_LIFECYCLE_SCENARIO_OOM = 0,
	LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA,
	LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INITIALIZED_BY_PROD_CR,
	LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE,
	LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INITIALIZED_BY_PROD_CR_DESTROY_TWICE
} logger_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	logger_t_lifecycle_scenario_t scenario;
} test_logger_t_lifecycle_case_t;

typedef struct {
	// Borrowed DI.
	logger_env_t env;

	// Owned DI.
	logger_fake_adapter_backend_t fake_adapter_backend_injected_by_test_infra;
	logger_fake_adapter_backend_t fake_adapter_backend_initialized_by_prod_cr;

	// SUT resources.
	logger_t *logger;

	// Test infrastructure resources.
	logger_t *sentinel;

	const test_logger_t_lifecycle_case_t *tc;
} test_logger_t_lifecycle_fixture_t;

static int setup_logger_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(logger_t_lifecycle, state, tc, fx);

	fake_memory_reset();
	fx->env.mem_ops = osal_mem_test_fake_ops();
	fx->env.vtbl = &logger_fake_adapter_vtbl;
	logger_fake_adapter_init_backend(&fx->fake_adapter_backend_injected_by_test_infra);
	logger_fake_adapter_init_backend(&fx->fake_adapter_backend_initialized_by_prod_cr);
	fx->sentinel = (logger_t *)(uintptr_t)0xDEADC0DEu;
	fx->logger = fx->sentinel;

	*state = fx;
	return 0;
}

static int teardown_logger_t_lifecycle(void **state)
{
	test_logger_t_lifecycle_fixture_t *fx = (test_logger_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_logger_t_lifecycle(void **state)
{
	test_logger_t_lifecycle_fixture_t *fx = (test_logger_t_lifecycle_fixture_t *)(*state);
	const test_logger_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	if (tc->scenario == LOGGER_T_LIFECYCLE_SCENARIO_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	logger_status_t ret = logger_create(&fx->logger, &fx->env);

	// ASSERT
	if (tc->scenario == LOGGER_T_LIFECYCLE_SCENARIO_OOM) {
		assert_int_equal(ret, LOGGER_STATUS_OOM);
		assert_ptr_equal(fx->logger, fx->sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}

	assert_int_equal(ret, LOGGER_STATUS_OK);
	assert_true(fx->logger != fx->sentinel);
	assert_non_null(fx->logger);
	assert_ptr_equal(logger_get_vtbl(fx->logger), fx->env.vtbl);
	assert_ptr_equal(logger_get_mem_ops(fx->logger), fx->env.mem_ops);
	assert_null(logger_get_backend(fx->logger));

	// ARRANGE
	void *backend_arg = NULL;
	switch (tc->scenario) {
		case LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE:
		case LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA:
			logger_inject_backend(fx->logger, &fx->fake_adapter_backend_injected_by_test_infra);
			assert_non_null(logger_get_backend(fx->logger));
			backend_arg = NULL;
			break;
		case LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INITIALIZED_BY_PROD_CR_DESTROY_TWICE:
		case LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INITIALIZED_BY_PROD_CR:
			backend_arg = (void*)&fx->fake_adapter_backend_initialized_by_prod_cr;
			break;
		default: fail();
	}

	// ACT
	ret = logger_complete_default_init(fx->logger, backend_arg);

	// ASSERT
	assert_int_equal(ret, LOGGER_STATUS_OK);
	switch (tc->scenario) {
		case LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE:
		case LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA:
			assert_ptr_equal(logger_get_backend(fx->logger), (void*)&fx->fake_adapter_backend_injected_by_test_infra);
			break;
		case LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INITIALIZED_BY_PROD_CR_DESTROY_TWICE:
		case LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INITIALIZED_BY_PROD_CR:
			assert_ptr_equal(logger_get_backend(fx->logger), (void*)&fx->fake_adapter_backend_initialized_by_prod_cr);
			break;
		default: fail();
	}

	// ARRANGE
	logger_fake_adapter_backend_t *backend = logger_get_backend(fx->logger);

	// ACT
	logger_destroy(&fx->logger);
	if (
		   tc->scenario == LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE
		|| tc->scenario == LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INITIALIZED_BY_PROD_CR_DESTROY_TWICE
	) {
		logger_destroy(&fx->logger);
	}

	// ASSERT
	assert_null(fx->logger);
	assert_int_equal(backend->destroy_call_count, 1);
	assert_ptr_equal(backend->last_destroy_backend, backend);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_logger_t_lifecycle_case_t CASE_LOGGER_T_LIFECYCLE_OOM = {
	.name = "logger_t_lifecycle_oom",
	.scenario = LOGGER_T_LIFECYCLE_SCENARIO_OOM,
};

static const test_logger_t_lifecycle_case_t CASE_LOGGER_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_TEST_INFRA = {
	.name = "logger_t_lifecycle_ok_backend_injected_by_test_infra",
	.scenario = LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA,
};

static const test_logger_t_lifecycle_case_t CASE_LOGGER_T_LIFECYCLE_OK_BACKEND_INITIALIZED_BY_PROD_CR = {
	.name = "logger_t_lifecycle_ok_backend_initialized_by_prod_cr",
	.scenario = LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INITIALIZED_BY_PROD_CR,
};

static const test_logger_t_lifecycle_case_t CASE_LOGGER_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE = {
	.name = "logger_t_lifecycle_ok_backend_injected_by_test_infra_destroy_twice",
	.scenario = LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE,
};

static const test_logger_t_lifecycle_case_t CASE_LOGGER_T_LIFECYCLE_OK_BACKEND_INITIALIZED_BY_PROD_CR_DESTROY_TWICE = {
	.name = "logger_t_lifecycle_ok_backend_initialized_by_prod_cr_destroy_twice",
	.scenario = LOGGER_T_LIFECYCLE_SCENARIO_OK_BACKEND_INITIALIZED_BY_PROD_CR_DESTROY_TWICE,
};

#define LOGGER_T_LIFECYCLE_CASES(X) \
X(CASE_LOGGER_T_LIFECYCLE_OOM) \
X(CASE_LOGGER_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_TEST_INFRA) \
X(CASE_LOGGER_T_LIFECYCLE_OK_BACKEND_INITIALIZED_BY_PROD_CR) \
X(CASE_LOGGER_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE) \
X(CASE_LOGGER_T_LIFECYCLE_OK_BACKEND_INITIALIZED_BY_PROD_CR_DESTROY_TWICE)

#define MAKE_LOGGER_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(logger_t_lifecycle, case_sym)

static const struct CMUnitTest logger_t_lifecycle_tests[] = {
	LOGGER_T_LIFECYCLE_CASES(MAKE_LOGGER_T_LIFECYCLE_TEST)
};

#undef LOGGER_T_LIFECYCLE_CASES
#undef MAKE_LOGGER_T_LIFECYCLE_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest logger_cr_tests_non_parametric[] = {
		cmocka_unit_test(test_logger_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(logger_cr_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(logger_t_lifecycle_tests, NULL, NULL);

	return failed;
}
/** @endcond */
