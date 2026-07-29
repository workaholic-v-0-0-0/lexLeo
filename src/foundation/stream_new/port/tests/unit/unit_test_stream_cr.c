/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_stream_cr.c
 * @ingroup stream_unit_tests
 * @brief Unit tests implementation for stream_cr.
 *
 * @details
 * This file implements the unit-level validation of stream_cr.
 *
 * See also:
 * - @ref testing_foundation_stream_cr_unit "stream_cr.c unit tests page"
 * - @ref specifications_stream "stream specifications"
 */

#include "stream/owners/stream_owners_api.h"
#include "stream/cr/stream_cr_api.h"

#include "stream/tests/stream_fake_adapter.h"
#include "stream/tests/stream_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/** @cond INTERNAL */
static const stream_adapter_id_t g_fake_adapter_ids[17] = { "fake_adapter_00", "fake_adapter_01", "fake_adapter_02", "fake_adapter_03",
	"fake_adapter_04", "fake_adapter_05", "fake_adapter_06", "fake_adapter_07", "fake_adapter_08", "fake_adapter_09", "fake_adapter_10",
	"fake_adapter_11", "fake_adapter_12", "fake_adapter_13", "fake_adapter_14", "fake_adapter_15", "fake_adapter_16" };
/** @endcond */

/**********************************************************************************************************************
 * @brief Test `stream_default_env()`.
 *
 * See contract:
 * - @ref specifications_stream_default_env "stream_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_default_env "stream_default_env() unit tests section"
 */
static void test_stream_default_env(void **state) {
	(void)state;

	// ARRANGE
	const stream_vtbl_t dummy_vtbl = {0};
	const stream_vtbl_t *dummy_vtbl_p = &dummy_vtbl;
	const osal_mem_ops_t dummy_mem_ops = {0};
	const osal_mem_ops_t *dummy_mem_ops_p = &dummy_mem_ops;

	// ACT
	stream_env_t ret = stream_default_env(dummy_vtbl_p, dummy_mem_ops_p);

	// ASSERT
	assert_ptr_equal(ret.mem, dummy_mem_ops_p);
	assert_ptr_equal(ret.vtbl, dummy_vtbl_p);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_stream_create "stream_create() specifications".
 * - @ref specifications_stream_complete_default_init "stream_complete_default_init() specifications".
 * - @ref specifications_stream_destroy "stream_destroy() specifications"
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_t_lifecycle "`stream_t` lifecycle unit tests section"
 */
typedef enum {
	STREAM_T_LIFECYCLE_SCENARIO_OOM = 0,
	STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA,
	STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_PROD_CR,
	STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE,
	STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_PROD_CR_DESTROY_TWICE
} stream_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	stream_t_lifecycle_scenario_t scenario;
} test_stream_t_lifecycle_case_t;

typedef struct {
	// Borrowed DI.
	stream_env_t env;

	// Owned DI.
	stream_fake_adapter_backend_t fake_adapter_backend_injected_by_test_infra;
	stream_fake_adapter_backend_t fake_adapter_backend_injected_by_prod_cr;

	// SUT resources.
	stream_t *stream;

	// Test infrastructure resources.
	stream_t *sentinel;

	const test_stream_t_lifecycle_case_t *tc;
} test_stream_t_lifecycle_fixture_t;

static int setup_stream_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_t_lifecycle, state, tc, fx);

	fake_memory_reset();
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.vtbl = &stream_fake_adapter_vtbl;
	stream_fake_adapter_init_backend(&fx->fake_adapter_backend_injected_by_test_infra);
	stream_fake_adapter_init_backend(&fx->fake_adapter_backend_injected_by_prod_cr);
	fx->sentinel = (stream_t *)(uintptr_t)0xDEADC0DEu;
	fx->stream = fx->sentinel;

	*state = fx;
	return 0;
}

static int teardown_stream_t_lifecycle(void **state)
{
	test_stream_t_lifecycle_fixture_t *fx = (test_stream_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_stream_t_lifecycle(void **state)
{
	test_stream_t_lifecycle_fixture_t *fx = (test_stream_t_lifecycle_fixture_t *)(*state);
	const test_stream_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	if (tc->scenario == STREAM_T_LIFECYCLE_SCENARIO_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	stream_status_t ret = stream_create(&fx->stream, &fx->env);

	// ASSERT
	if (tc->scenario == STREAM_T_LIFECYCLE_SCENARIO_OOM) {
		assert_int_equal(ret, STREAM_STATUS_OOM);
		assert_ptr_equal(fx->stream, fx->sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}

	assert_int_equal(ret, STREAM_STATUS_OK);
	assert_true(fx->stream != fx->sentinel);
	assert_non_null(fx->stream);
	assert_ptr_equal(stream_get_vtbl(fx->stream), fx->env.vtbl);
	assert_ptr_equal(stream_get_mem(fx->stream), fx->env.mem);
	assert_null(stream_get_backend(fx->stream));

	// ARRANGE
	void *backend_arg = NULL;
	switch (tc->scenario) {
		case STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE:
		case STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA:
			stream_inject_backend(fx->stream, &fx->fake_adapter_backend_injected_by_test_infra);
			assert_non_null(stream_get_backend(fx->stream));
			backend_arg = NULL;
			break;
		case STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_PROD_CR_DESTROY_TWICE:
		case STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_PROD_CR:
			backend_arg = (void*)&fx->fake_adapter_backend_injected_by_prod_cr;
			break;
		default: fail();
	}

	// ACT
	ret = stream_complete_default_init(fx->stream, backend_arg);

	// ASSERT
	assert_int_equal(ret, STREAM_STATUS_OK);
	switch (tc->scenario) {
		case STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE:
		case STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA:
			assert_ptr_equal(stream_get_backend(fx->stream), (void*)&fx->fake_adapter_backend_injected_by_test_infra);
			break;
		case STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_PROD_CR_DESTROY_TWICE:
		case STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_PROD_CR:
			assert_ptr_equal(stream_get_backend(fx->stream), (void*)&fx->fake_adapter_backend_injected_by_prod_cr);
			break;
		default: fail();
	}

	// ARRANGE
	stream_fake_adapter_backend_t *backend = stream_get_backend(fx->stream);

	// ACT
	stream_destroy(&fx->stream);
	if (
		   tc->scenario == STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE
		|| tc->scenario == STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_PROD_CR_DESTROY_TWICE
	) {
		stream_destroy(&fx->stream);
	}

	// ASSERT
	assert_null(fx->stream);
	assert_int_equal(backend->close_call_count, 1);
	assert_ptr_equal(backend->last_close_backend, backend);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_stream_t_lifecycle_case_t CASE_STREAM_T_LIFECYCLE_OOM = {
	.name = "stream_t_lifecycle_oom",
	.scenario = STREAM_T_LIFECYCLE_SCENARIO_OOM,
};

static const test_stream_t_lifecycle_case_t CASE_STREAM_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_TEST_INFRA = {
	.name = "stream_t_lifecycle_ok_backend_injected_by_test_infra",
	.scenario = STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA,
};

static const test_stream_t_lifecycle_case_t CASE_STREAM_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_PROD_CR = {
	.name = "stream_t_lifecycle_ok_backend_injected_by_prod_cr",
	.scenario = STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_PROD_CR,
};

static const test_stream_t_lifecycle_case_t CASE_STREAM_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE = {
	.name = "stream_t_lifecycle_ok_backend_injected_by_test_infra_destroy_twice",
	.scenario = STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE,
};

static const test_stream_t_lifecycle_case_t CASE_STREAM_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_PROD_CR_DESTROY_TWICE = {
	.name = "stream_t_lifecycle_ok_backend_injected_by_prod_cr_destroy_twice",
	.scenario = STREAM_T_LIFECYCLE_SCENARIO_OK_BACKEND_INJECTED_BY_PROD_CR_DESTROY_TWICE,
};

#define STREAM_T_LIFECYCLE_CASES(X) \
X(CASE_STREAM_T_LIFECYCLE_OOM) \
X(CASE_STREAM_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_TEST_INFRA) \
X(CASE_STREAM_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_PROD_CR) \
X(CASE_STREAM_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_TEST_INFRA_DESTROY_TWICE) \
X(CASE_STREAM_T_LIFECYCLE_OK_BACKEND_INJECTED_BY_PROD_CR_DESTROY_TWICE)

#define MAKE_STREAM_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_t_lifecycle, case_sym)

static const struct CMUnitTest stream_t_lifecycle_tests[] = {
	STREAM_T_LIFECYCLE_CASES(MAKE_STREAM_T_LIFECYCLE_TEST)
};

#undef STREAM_T_LIFECYCLE_CASES
#undef MAKE_STREAM_T_LIFECYCLE_TEST

/** @endcond */

/******************************************************************************************************************************************
 * @brief Test `stream_default_factory_cfg()`.
 *
 * See contract:
 * - @ref specifications_stream_default_factory_cfg "stream_default_factory_cfg() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_default_factory_cfg "stream_default_factory_cfg() unit tests section"
 */
static void test_stream_default_factory_cfg(void **state) {
	(void)state;

	// ACT
	stream_factory_cfg_t ret = stream_default_factory_cfg();

	// ASSERT
	assert_true(ret.fact_cap == 16);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_factory_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_stream_create_factory "stream_create_factory() specifications".
 * - @ref specifications_stream_destroy_factory "stream_destroy_factory() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_factory_t_lifecycle "`stream_factory_t` lifecycle unit tests section"
 */
typedef enum {
	STREAM_FACTORY_T_LIFECYCLE_SCENARIO_HANDLE_OOM = 0,
	STREAM_FACTORY_T_LIFECYCLE_SCENARIO_REGISTRY_OOM,
	STREAM_FACTORY_T_LIFECYCLE_SCENARIO_OK,
	STREAM_FACTORY_T_LIFECYCLE_SCENARIO_OK_DESTROY_TWICE
} stream_factory_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	stream_factory_t_lifecycle_scenario_t scenario;
} test_stream_factory_t_lifecycle_case_t;

typedef struct {
	// Borrowed DI.
	const osal_mem_ops_t *mem_arg;

	// Resources
	stream_factory_t *factory;

	const test_stream_factory_t_lifecycle_case_t *tc;
} test_stream_factory_t_lifecycle_fixture_t;

static int setup_stream_factory_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_factory_t_lifecycle, state, tc, fx);

	fake_memory_reset();
	fx->mem_arg = osal_mem_test_fake_ops();

	*state = fx;
	return 0;
}

static int teardown_stream_factory_t_lifecycle(void **state)
{
	test_stream_factory_t_lifecycle_fixture_t *fx = (test_stream_factory_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_stream_factory_t_lifecycle(void **state)
{
	test_stream_factory_t_lifecycle_fixture_t *fx = (test_stream_factory_t_lifecycle_fixture_t *)(*state);
	const test_stream_factory_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	const stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
	if (tc->scenario == STREAM_FACTORY_T_LIFECYCLE_SCENARIO_HANDLE_OOM) { fake_memory_fail_only_on_call(1); }
	if (tc->scenario == STREAM_FACTORY_T_LIFECYCLE_SCENARIO_REGISTRY_OOM) { fake_memory_fail_only_on_call(2); }
	stream_factory_t *factory_snapshot = fx->factory;

	// ACT
	stream_factory_status_t ret = stream_create_factory(&fx->factory, &stream_factory_cfg, fx->mem_arg);

	// ASSERT
	switch (tc->scenario) {
		case STREAM_FACTORY_T_LIFECYCLE_SCENARIO_HANDLE_OOM:
		case STREAM_FACTORY_T_LIFECYCLE_SCENARIO_REGISTRY_OOM:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_OOM);
			assert_ptr_equal(fx->factory, factory_snapshot);
			break;
		case STREAM_FACTORY_T_LIFECYCLE_SCENARIO_OK_DESTROY_TWICE:
		case STREAM_FACTORY_T_LIFECYCLE_SCENARIO_OK:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_OK);
			assert_non_null(fx->factory);
			assert_true(fx->factory != factory_snapshot);
			break;
		default: fail();
	}

	// ACT
	stream_destroy_factory(&fx->factory);
	if (tc->scenario == STREAM_FACTORY_T_LIFECYCLE_SCENARIO_OK_DESTROY_TWICE) {
		stream_destroy_factory(&fx->factory);
	}

	// ASSERT
	assert_null(fx->factory);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_stream_factory_t_lifecycle_case_t CASE_STREAM_FACTORY_T_LIFECYCLE_HANDLE_OOM = {
	.name = "stream_factory_t_lifecycle_handle_oom",
	.scenario = STREAM_FACTORY_T_LIFECYCLE_SCENARIO_HANDLE_OOM,
};

static const test_stream_factory_t_lifecycle_case_t CASE_STREAM_FACTORY_T_LIFECYCLE_REGISTRY_OOM = {
	.name = "stream_factory_t_lifecycle_registry_oom",
	.scenario = STREAM_FACTORY_T_LIFECYCLE_SCENARIO_REGISTRY_OOM,
};

static const test_stream_factory_t_lifecycle_case_t CASE_STREAM_FACTORY_T_LIFECYCLE_OK = {
	.name = "stream_factory_t_lifecycle_ok",
	.scenario = STREAM_FACTORY_T_LIFECYCLE_SCENARIO_OK,
};

static const test_stream_factory_t_lifecycle_case_t CASE_STREAM_FACTORY_T_LIFECYCLE_OK_DESTROY_TWICE = {
	.name = "stream_factory_t_lifecycle_ok_destroy_twice",
	.scenario = STREAM_FACTORY_T_LIFECYCLE_SCENARIO_OK_DESTROY_TWICE,
};

#define STREAM_FACTORY_T_LIFECYCLE_CASES(X) \
X(CASE_STREAM_FACTORY_T_LIFECYCLE_HANDLE_OOM) \
X(CASE_STREAM_FACTORY_T_LIFECYCLE_REGISTRY_OOM) \
X(CASE_STREAM_FACTORY_T_LIFECYCLE_OK) \
X(CASE_STREAM_FACTORY_T_LIFECYCLE_OK_DESTROY_TWICE)

#define MAKE_STREAM_FACTORY_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_factory_t_lifecycle, case_sym)

static const struct CMUnitTest stream_factory_t_lifecycle_tests[] = {
	STREAM_FACTORY_T_LIFECYCLE_CASES(MAKE_STREAM_FACTORY_T_LIFECYCLE_TEST)
};

#undef STREAM_FACTORY_T_LIFECYCLE_CASES
#undef MAKE_STREAM_FACTORY_T_LIFECYCLE_TEST

/** @endcond */

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_factory_add_adapter()`.
 *
 * See contract:
 * - @ref specifications_stream_factory_add_adapter "stream_factory_add_adapter() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_factory_add_adapter "stream_factory_add_adapter() unit tests section"
 */
typedef enum {
	STREAM_FACTORY_ADD_ADAPTER_SCENARIO_OK = 0,
	STREAM_FACTORY_ADD_ADAPTER_SCENARIO_ALREADY_EXISTS,
	STREAM_FACTORY_ADD_ADAPTER_SCENARIO_FULL,
} stream_factory_add_adapter_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	stream_factory_add_adapter_scenario_t scenario;
} test_stream_factory_add_adapter_case_t;

typedef struct {
	// Borrowed DI.
	const osal_mem_ops_t *mem_arg;

	// SUT resource
	stream_adapter_provider_t *adapter_provider_arg;

	// Test infrastructure resources
	stream_factory_t *factory;
	stream_fake_adapter_backend_t fake_adapter_backend;
	stream_fake_adapter_backend_ctor_mock_spy_t ctor_mock_spy;
	size_t fake_adapter_provider_already_registered_nb;
	stream_adapter_provider_t **fake_adapter_provider_already_registered;

	const test_stream_factory_add_adapter_case_t *tc;
} test_stream_factory_add_adapter_fixture_t;

static int setup_stream_factory_add_adapter(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_factory_add_adapter, state, tc, fx);

	fx->mem_arg = osal_mem_default_ops();
	const stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
	assert_int_equal(stream_factory_cfg.fact_cap, 16);
	assert_int_equal(stream_create_factory(&fx->factory, &stream_factory_cfg, fx->mem_arg), STREAM_FACTORY_STATUS_OK);
	switch (tc->scenario) {
		case STREAM_FACTORY_ADD_ADAPTER_SCENARIO_OK:
			fx->fake_adapter_provider_already_registered_nb = 0;
			break;
		case STREAM_FACTORY_ADD_ADAPTER_SCENARIO_ALREADY_EXISTS:
			fx->fake_adapter_provider_already_registered_nb = 1;
			break;
		case STREAM_FACTORY_ADD_ADAPTER_SCENARIO_FULL:
			fx->fake_adapter_provider_already_registered_nb = stream_factory_cfg.fact_cap;
			break;
		default: fail();
	}
	if (fx->fake_adapter_provider_already_registered_nb > 0) {
		fx->fake_adapter_provider_already_registered =
			osal_calloc(fx->fake_adapter_provider_already_registered_nb, sizeof(stream_adapter_provider_t *));
		assert_non_null(fx->fake_adapter_provider_already_registered);
	}
	stream_fake_adapter_init_backend(&fx->fake_adapter_backend);
	for (size_t i = 0; i < fx->fake_adapter_provider_already_registered_nb; i++) {
		assert_int_equal(
			stream_fake_adapter_create_provider(
				&fx->fake_adapter_provider_already_registered[i],
				&fx->fake_adapter_backend,
				fx->mem_arg,
				&fx->ctor_mock_spy
			),
			STREAM_STATUS_OK
		);
		assert_int_equal(
			stream_factory_add_adapter(
				fx->factory,
				g_fake_adapter_ids[i],
				fx->fake_adapter_provider_already_registered[i]
			),
			STREAM_FACTORY_STATUS_OK
		);
	}

	*state = fx;
	return 0;
}

static int teardown_stream_factory_add_adapter(void **state)
{
	test_stream_factory_add_adapter_fixture_t *fx = (test_stream_factory_add_adapter_fixture_t *)(*state);
	if (fx->tc->scenario != STREAM_FACTORY_ADD_ADAPTER_SCENARIO_OK) {
		stream_fake_adapter_destroy_provider(&fx->adapter_provider_arg);
	}
	osal_free(fx->fake_adapter_provider_already_registered);
	stream_destroy_factory(&fx->factory);
	osal_free(fx);
	return 0;
}

static void test_stream_factory_add_adapter(void **state)
{
	test_stream_factory_add_adapter_fixture_t *fx = (test_stream_factory_add_adapter_fixture_t *)(*state);
	const test_stream_factory_add_adapter_case_t *tc = fx->tc;

	// ARRANGE
	stream_adapter_id_t adapter_id_arg = NULL;
	switch (tc->scenario) {
		case STREAM_FACTORY_ADD_ADAPTER_SCENARIO_OK:
		case STREAM_FACTORY_ADD_ADAPTER_SCENARIO_ALREADY_EXISTS:
			adapter_id_arg = g_fake_adapter_ids[0];
			break;
		case STREAM_FACTORY_ADD_ADAPTER_SCENARIO_FULL:
			adapter_id_arg = g_fake_adapter_ids[16];
			break;
		default: fail();
	}
	assert_int_equal(
		stream_fake_adapter_create_provider(
			&fx->adapter_provider_arg,
			&fx->fake_adapter_backend,
			fx->mem_arg,
			&fx->ctor_mock_spy
		),
		STREAM_STATUS_OK
	);

	// ACT
	stream_factory_status_t ret = stream_factory_add_adapter(fx->factory, adapter_id_arg, fx->adapter_provider_arg);

	// ASSERT
	switch (tc->scenario) {
		case STREAM_FACTORY_ADD_ADAPTER_SCENARIO_OK:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_OK);
			break;
		case STREAM_FACTORY_ADD_ADAPTER_SCENARIO_ALREADY_EXISTS:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_ALREADY_EXISTS);
			break;
		case STREAM_FACTORY_ADD_ADAPTER_SCENARIO_FULL:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_FULL);
			break;
		default: fail();
	}
}

static const test_stream_factory_add_adapter_case_t CASE_STREAM_FACTORY_ADD_ADAPTER_OK = {
	.name = "stream_factory_add_adapter_ok",
	.scenario = STREAM_FACTORY_ADD_ADAPTER_SCENARIO_OK,
};

static const test_stream_factory_add_adapter_case_t CASE_STREAM_FACTORY_ADD_ADAPTER_ALREADY_EXISTS = {
	.name = "stream_factory_add_adapter_already_exists",
	.scenario = STREAM_FACTORY_ADD_ADAPTER_SCENARIO_ALREADY_EXISTS,
};

static const test_stream_factory_add_adapter_case_t CASE_STREAM_FACTORY_ADD_ADAPTER_FULL = {
	.name = "stream_factory_add_adapter_full",
	.scenario = STREAM_FACTORY_ADD_ADAPTER_SCENARIO_FULL,
};

#define STREAM_FACTORY_ADD_ADAPTER_CASES(X) \
X(CASE_STREAM_FACTORY_ADD_ADAPTER_OK) \
X(CASE_STREAM_FACTORY_ADD_ADAPTER_ALREADY_EXISTS) \
X(CASE_STREAM_FACTORY_ADD_ADAPTER_FULL)

#define MAKE_STREAM_FACTORY_ADD_ADAPTER_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_factory_add_adapter, case_sym)

static const struct CMUnitTest stream_factory_add_adapter_tests[] = {
	STREAM_FACTORY_ADD_ADAPTER_CASES(MAKE_STREAM_FACTORY_ADD_ADAPTER_TEST)
};

#undef STREAM_FACTORY_ADD_ADAPTER_CASES
#undef MAKE_STREAM_FACTORY_ADD_ADAPTER_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest stream_cr_tests_non_parametric[] = {
		cmocka_unit_test(test_stream_default_env),
		cmocka_unit_test(test_stream_default_factory_cfg)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(stream_cr_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(stream_t_lifecycle_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_factory_t_lifecycle_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_factory_add_adapter_tests, NULL, NULL);

	return failed;
}
/** @endcond */
