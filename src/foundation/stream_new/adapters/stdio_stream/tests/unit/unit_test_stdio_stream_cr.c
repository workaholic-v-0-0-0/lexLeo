/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_stdio_stream_cr.c
 * @ingroup stdio_stream_unit_tests
 * @brief Unit tests implementation for stdio_stream_cr.c.
 *
 * @details
 * Tests the CR-facing services of the `stdio_stream` adapter, including
 * construction from external borrowed dependencies, completion with internal
 * borrowed dependencies, and adapter provider lifecycle.
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_cr_unit "stdio_stream_cr.c unit tests page"
 * - @ref specifications_stdio_stream "stdio_stream specifications"
 */

#include "stdio_stream/cr/stdio_stream_cr_api.h"
#include "stream/cr/stream_cr_api.h"

#include "stdio_stream/tests/stdio_stream_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"
#include "osal/stdio/test/osal_stdio_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**********************************************************************************************************************
 * @brief Test `stdio_stream_vtbl()`.
 *
 * See contract:
 * - @ref specifications_stdio_stream_vtbl "stdio_stream_vtbl() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stdio_stream_unit_stdio_stream_vtbl "stdio_stream_vtbl() unit tests section"
 */
static void test_stdio_stream_vtbl(void **state) {
	(void)state;

	// ACT
	const stream_vtbl_t *ret = stdio_stream_vtbl();

	// ASSERT
	assert_non_null(ret);
	assert_non_null(ret->read);
	assert_non_null(ret->write);
	assert_non_null(ret->flush);
	assert_non_null(ret->close);
}

/**********************************************************************************************************************
 * @brief Test `stdio_stream_default_cfg()`.
 *
 * See contract:
 * - @ref specifications_stdio_stream_default_cfg "stdio_stream_default_cfg() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stdio_stream_unit_stdio_stream_default_cfg "stdio_stream_default_cfg() unit tests section"
 */
static void test_stdio_stream_default_cfg(void **state) {
	(void)state;

	// ACT
	stdio_stream_cfg_t ret = stdio_stream_default_cfg();

	// ASSERT
	assert_int_equal(ret.reserved, 0);
}

/**********************************************************************************************************************
 * @brief Test `stdio_stream_default_env()`.
 *
 * See contract:
 * - @ref specifications_stdio_stream_default_env "stdio_stream_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stdio_stream_unit_stdio_stream_default_env "stdio_stream_default_env() unit tests section"
 */
static void test_stdio_stream_default_env(void **state) {
	(void)state;

	// ARRANGE
	const osal_stdio_ops_t *dummy_stdio_ops = (const osal_stdio_ops_t *)(uintptr_t)0xDEADC0DEu;
	const osal_mem_ops_t *dummy_mem_ops = (const osal_mem_ops_t *)(uintptr_t)0xBADC0FFEu;

	// ACT
	stdio_stream_env_t ret = stdio_stream_default_env(dummy_stdio_ops, dummy_mem_ops);

	// ASSERT
	assert_ptr_equal(ret.stdio_ops, dummy_stdio_ops);
	assert_ptr_equal(ret.mem_ops, dummy_mem_ops);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `stdio_stream_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_stdio_stream_create "stdio_stream_create() specifications".
 * - @ref specifications_stdio_stream_complete_default_init "stdio_stream_complete_default_init() specifications".
 * - @ref specifications_stdio_stream_close "stdio_stream_close() specifications".
 * - @ref specifications_stdio_stream_vtbl "stdio_stream_vtbl() specifications".
 * - @ref specifications_stdio_stream_default_cfg "stdio_stream_default_cfg() specifications".
 * - @ref specifications_stdio_stream_default_env "stdio_stream_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stdio_stream_unit_stdio_stream_t_lifecycle "`stdio_stream_t` lifecycle unit tests section"
 */
typedef enum {
	STDIO_STREAM_T_LIFECYCLE_SCENARIO_STDIO_STREAM_HANDLE_OOM = 0,
	STDIO_STREAM_T_LIFECYCLE_SCENARIO_OK_STDIO_INJECTED_BY_TEST_INFRA,
	STDIO_STREAM_T_LIFECYCLE_SCENARIO_OK_STDIO_FETCHED_BY_DEFAULT_INIT,
} stdio_stream_t_lifecycle_scenario_t;

typedef struct {
	const char *name;
	stdio_stream_t_lifecycle_scenario_t scenario;
	stream_standard_stream_kind_t kind;
} test_stdio_stream_t_lifecycle_case_t;

typedef struct {
	const test_stdio_stream_t_lifecycle_case_t *tc;
} test_stdio_stream_t_lifecycle_fixture_t;

static int setup_stdio_stream_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stdio_stream_t_lifecycle, state, tc, fx);

	fake_stdio_reset();
	fake_memory_reset();

	*state = fx;
	return 0;
}

static int teardown_stdio_stream_t_lifecycle(void **state)
{
	test_stdio_stream_t_lifecycle_fixture_t *fx = (test_stdio_stream_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_stdio_stream_t_lifecycle(void **state) {
	test_stdio_stream_t_lifecycle_fixture_t *fx = (test_stdio_stream_t_lifecycle_fixture_t *)(*state);
	const test_stdio_stream_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	stdio_stream_t *sentinel = (stdio_stream_t *)(uintptr_t)0xDEADC0DEu;
	stdio_stream_t *stdio_stream = sentinel;
	const stdio_stream_env_t stdio_stream_env = {
		.stdio_ops = osal_stdio_test_fake_ops(),
		.mem_ops = osal_mem_test_fake_ops()
	};
	if (tc->scenario == STDIO_STREAM_T_LIFECYCLE_SCENARIO_STDIO_STREAM_HANDLE_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	stdio_stream_status_t ret = stdio_stream_create(&stdio_stream, &stdio_stream_env);

	// ASSERT
	if (tc->scenario == STDIO_STREAM_T_LIFECYCLE_SCENARIO_STDIO_STREAM_HANDLE_OOM) {
		assert_int_equal(ret, STDIO_STREAM_STATUS_OOM);
		assert_ptr_equal(stdio_stream, sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}
	assert_int_equal(ret, STDIO_STREAM_STATUS_OK);
	assert_true(stdio_stream != sentinel);
	assert_non_null(stdio_stream);
	assert_ptr_equal(stdio_stream_get_stdio_ops(stdio_stream), stdio_stream_env.stdio_ops);
	assert_ptr_equal(stdio_stream_get_mem_ops(stdio_stream), stdio_stream_env.mem_ops);
	assert_null(stdio_stream_get_stdio(stdio_stream));

	// ARRANGE
	stdio_stream_cfg_t stdio_stream_cfg = stdio_stream_default_cfg();
	stream_standard_stream_creator_args_t args = {.kind = tc->kind};
	const osal_stdio_ops_t *stdio_ops_snapshot = stdio_stream_get_stdio_ops(stdio_stream);
	const osal_mem_ops_t *mem_ops_snapshot = stdio_stream_get_mem_ops(stdio_stream);
	fake_stdio_t fake_stdio = {0};
	OSAL_STDIO *expected_stdio = NULL;
	if (tc->scenario == STDIO_STREAM_T_LIFECYCLE_SCENARIO_OK_STDIO_INJECTED_BY_TEST_INFRA) {
		stdio_stream_inject_stdio(stdio_stream, fake_stdio_to_osal_stdio_stream(&fake_stdio));
	}

	// ACT
	ret = stdio_stream_complete_default_init(stdio_stream, &stdio_stream_cfg, &args);

	// ASSERT
	assert_int_equal(ret, STDIO_STREAM_STATUS_OK);
	assert_ptr_equal(stdio_stream_get_stdio_ops(stdio_stream), stdio_ops_snapshot);
	assert_ptr_equal(stdio_stream_get_mem_ops(stdio_stream), mem_ops_snapshot);
	switch (tc->scenario) {
		case STDIO_STREAM_T_LIFECYCLE_SCENARIO_OK_STDIO_INJECTED_BY_TEST_INFRA:
			assert_int_equal(get_stdin_call_count, 0);
			assert_int_equal(get_stdout_call_count, 0);
			assert_int_equal(get_stderr_call_count, 0);
			expected_stdio = fake_stdio_to_osal_stdio_stream(&fake_stdio);
			break;
		case STDIO_STREAM_T_LIFECYCLE_SCENARIO_OK_STDIO_FETCHED_BY_DEFAULT_INIT:
			switch (tc->kind) {
				case STREAM_STANDARD_STREAM_KIND_STDIN:
					assert_int_equal(get_stdin_call_count, 1);
					expected_stdio = fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stdin);
					break;
				case STREAM_STANDARD_STREAM_KIND_STDOUT:
					assert_int_equal(get_stdout_call_count, 1);
					expected_stdio = fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stdout);
					break;
				case STREAM_STANDARD_STREAM_KIND_STDERR:
					assert_int_equal(get_stderr_call_count, 1);
					expected_stdio = fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stderr);
					break;
				default: fail();
			}
			break;
		default: fail();
	}
	assert_ptr_equal(stdio_stream_get_stdio(stdio_stream), expected_stdio);

	// ACT
	ret = stdio_stream_vtbl()->close(stdio_stream);

	// ASSERT
	assert_int_equal(ret, STDIO_STREAM_STATUS_OK);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_stdio_stream_t_lifecycle_case_t CASE_STDIO_STREAM_T_LIFECYCLE_STDIO_STREAM_HANDLE_OOM = {
	.name = "stdio_stream_t_lifecycle_stdio_stream_handle_oom",
	.scenario = STDIO_STREAM_T_LIFECYCLE_SCENARIO_STDIO_STREAM_HANDLE_OOM
};

static const test_stdio_stream_t_lifecycle_case_t CASE_STDIO_STREAM_T_LIFECYCLE_OK_STDIO_INJECTED_BY_TEST_INFRA = {
	.name = "stdio_stream_t_lifecycle_stdio_injected_by_test_infra",
	.scenario = STDIO_STREAM_T_LIFECYCLE_SCENARIO_OK_STDIO_INJECTED_BY_TEST_INFRA
};

static const test_stdio_stream_t_lifecycle_case_t CASE_STDIO_STREAM_T_LIFECYCLE_OK_STDIO_FETCHED_BY_DEFAULT_INIT = {
	.name = "stdio_stream_t_lifecycle_stdio_fetched_by_default_init",
	.scenario = STDIO_STREAM_T_LIFECYCLE_SCENARIO_OK_STDIO_FETCHED_BY_DEFAULT_INIT,
	.kind = STREAM_STANDARD_STREAM_KIND_STDIN
};

#define STDIO_STREAM_T_LIFECYCLE_CASES(X) \
X(CASE_STDIO_STREAM_T_LIFECYCLE_STDIO_STREAM_HANDLE_OOM) \
X(CASE_STDIO_STREAM_T_LIFECYCLE_OK_STDIO_INJECTED_BY_TEST_INFRA) \
X(CASE_STDIO_STREAM_T_LIFECYCLE_OK_STDIO_FETCHED_BY_DEFAULT_INIT)

#define MAKE_STDIO_STREAM_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stdio_stream_t_lifecycle, case_sym)

static const struct CMUnitTest stdio_stream_t_lifecycle_tests[] = {
	STDIO_STREAM_T_LIFECYCLE_CASES(MAKE_STDIO_STREAM_T_LIFECYCLE_TEST)
};

#undef STDIO_STREAM_T_LIFECYCLE_CASES
#undef MAKE_STDIO_STREAM_T_LIFECYCLE_TEST

/** @endcond */

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_adapter_provider_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_stdio_stream_create_adapter_provider "stdio_stream_create_adapter_provider".
 * - @ref specifications_stream_destroy_adapter_provider "stream_destroy_adapter_provider() specifications"
 *
 * See test description:
 * - @ref testing_foundation_stdio_stream_unit_stream_adapter_provider_t_lifecycle "`stream_adapter_provider_t` lifecycle unit tests section"
 */
typedef enum {
	STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM = 0,
	STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OK
} stdio_stream_stream_adapter_provider_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	stdio_stream_stream_adapter_provider_t_lifecycle_scenario_t scenario;
} test_stdio_stream_stream_adapter_provider_t_lifecycle_case_t;

typedef struct {
	const test_stdio_stream_stream_adapter_provider_t_lifecycle_case_t *tc;
} test_stdio_stream_stream_adapter_provider_t_lifecycle_fixture_t;

static int setup_stdio_stream_stream_adapter_provider_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stdio_stream_stream_adapter_provider_t_lifecycle, state, tc, fx);

	fake_memory_reset();
	fake_stdio_reset();

	*state = fx;
	return 0;
}

static int teardown_stdio_stream_stream_adapter_provider_t_lifecycle(void **state)
{
	test_stdio_stream_stream_adapter_provider_t_lifecycle_fixture_t *fx = (test_stdio_stream_stream_adapter_provider_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_stdio_stream_stream_adapter_provider_t_lifecycle(void **state)
{
	test_stdio_stream_stream_adapter_provider_t_lifecycle_fixture_t *fx = (test_stdio_stream_stream_adapter_provider_t_lifecycle_fixture_t *)(*state);
	const test_stdio_stream_stream_adapter_provider_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	stream_adapter_provider_t *sentinel = (stream_adapter_provider_t *)(uintptr_t)0xDEADC0DEu;
	stream_adapter_provider_t *stream_adapter_provider = sentinel;
	stdio_stream_cfg_t stdio_stream_cfg = stdio_stream_default_cfg();
	const stdio_stream_env_t stdio_stream_env = {
		.stdio_ops = osal_stdio_test_fake_ops(),
		.mem_ops = osal_mem_test_fake_ops()
	};
	if (tc->scenario == STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	stdio_stream_status_t ret = stdio_stream_create_adapter_provider(&stream_adapter_provider, &stdio_stream_cfg, &stdio_stream_env);

	// ASSERT
	if (tc->scenario == STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM) {
		assert_int_equal(ret, STDIO_STREAM_STATUS_OOM);
		assert_ptr_equal(stream_adapter_provider, sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}
	assert_int_equal(ret, STDIO_STREAM_STATUS_OK);
	assert_true(stream_adapter_provider != sentinel);
	assert_non_null(stream_adapter_provider);
	assert_non_null(stream_adapter_provider->backend_ctor);
	assert_ptr_equal(stream_adapter_provider->vtbl, stdio_stream_vtbl());
	assert_non_null(stream_adapter_provider->ud);
	assert_non_null(stream_adapter_provider->ud_dtor);
	assert_ptr_equal(stream_adapter_provider->mem, stdio_stream_env.mem_ops);

	// ACT
	stream_destroy_adapter_provider(stream_adapter_provider);

	// ASSERT
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_stdio_stream_stream_adapter_provider_t_lifecycle_case_t CASE_STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OOM = {
	.name = "stdio_stream_stream_adapter_provider_t_lifecycle_oom",
	.scenario = STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM,
};

static const test_stdio_stream_stream_adapter_provider_t_lifecycle_case_t CASE_STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OK = {
	.name = "stdio_stream_stream_adapter_provider_t_lifecycle_ok",
	.scenario = STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OK,
};

#define STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_CASES(X) \
X(CASE_STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OOM) \
X(CASE_STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OK)

#define MAKE_STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stdio_stream_stream_adapter_provider_t_lifecycle, case_sym)

static const struct CMUnitTest stdio_stream_stream_adapter_provider_t_lifecycle_tests[] = {
	STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_CASES(MAKE_STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_TEST)
};

#undef STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_CASES
#undef MAKE_STDIO_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest stdio_stream_cr_tests_non_parametric[] = {
		cmocka_unit_test(test_stdio_stream_vtbl),
		cmocka_unit_test(test_stdio_stream_default_cfg),
		cmocka_unit_test(test_stdio_stream_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(stdio_stream_cr_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(stdio_stream_t_lifecycle_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stdio_stream_stream_adapter_provider_t_lifecycle_tests, NULL, NULL);

	return failed;
}
/** @endcond */
