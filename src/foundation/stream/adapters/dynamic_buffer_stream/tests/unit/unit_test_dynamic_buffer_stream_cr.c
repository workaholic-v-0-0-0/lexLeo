/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_dynamic_buffer_stream_cr.c
 * @ingroup dynamic_buffer_stream_unit_tests
 * @brief Unit tests implementation for dynamic_buffer_stream_cr.c.
 *
 * @details
 * Tests the CR-facing services of the `dynamic_buffer_stream` adapter,
 * including construction from external borrowed dependencies, completion with
 * owned resources, and adapter provider lifecycle.
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_cr_unit "dynamic_buffer_stream_cr.c unit tests page"
 * - @ref specifications_dynamic_buffer_stream "dynamic_buffer_stream specifications"
 */

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_api.h"
#include "stream/cr/stream_cr_api.h"

#include "dynamic_buffer_stream/tests/dynamic_buffer_stream_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**********************************************************************************************************************
 * @brief Test `dynamic_buffer_stream_vtbl()`.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_vtbl "dynamic_buffer_stream_vtbl() specifications".
 *
 * See test description:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_dynamic_buffer_stream_vtbl "dynamic_buffer_stream_vtbl() unit tests section"
 */
static void test_dynamic_buffer_stream_vtbl(void **state) {
	(void)state;

	// ACT
	const stream_vtbl_t *ret = dynamic_buffer_stream_vtbl();

	// ASSERT
	assert_non_null(ret);
	assert_non_null(ret->read);
	assert_non_null(ret->write);
	assert_non_null(ret->flush);
	assert_non_null(ret->close);
}

/**********************************************************************************************************************
 * @brief Test `dynamic_buffer_stream_default_cfg()`.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_default_cfg "dynamic_buffer_stream_default_cfg() specifications".
 *
 * See test description:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_dynamic_buffer_stream_default_cfg "dynamic_buffer_stream_default_cfg() unit tests section"
 */
static void test_dynamic_buffer_stream_default_cfg(void **state) {
	(void)state;

	// ACT
	dynamic_buffer_stream_cfg_t ret = dynamic_buffer_stream_default_cfg();

	// ASSERT
	assert_true(ret.default_cap > 0);
}

/**********************************************************************************************************************
 * @brief Test `dynamic_buffer_stream_default_env()`.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_default_env "dynamic_buffer_stream_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_dynamic_buffer_stream_default_env "dynamic_buffer_stream_default_env() unit tests section"
 */
static void test_dynamic_buffer_stream_default_env(void **state) {
	(void)state;

	// ARRANGE
	const osal_mem_ops_t *dummy_mem_ops = (const osal_mem_ops_t *)(uintptr_t)0xBADC0FFEu;

	// ACT
	dynamic_buffer_stream_env_t ret = dynamic_buffer_stream_default_env(dummy_mem_ops);

	// ASSERT
	assert_ptr_equal(ret.mem_ops, dummy_mem_ops);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `dynamic_buffer_stream_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_create "dynamic_buffer_stream_create() specifications".
 * - @ref specifications_dynamic_buffer_stream_complete_default_init "dynamic_buffer_stream_complete_default_init() specifications".
 * - @ref specifications_dynamic_buffer_stream_close "dynamic_buffer_stream_close() specifications".
 * - @ref specifications_dynamic_buffer_stream_vtbl "dynamic_buffer_stream_vtbl() specifications".
 * - @ref specifications_dynamic_buffer_stream_default_cfg "dynamic_buffer_stream_default_cfg() specifications".
 * - @ref specifications_dynamic_buffer_stream_default_env "dynamic_buffer_stream_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_dynamic_buffer_stream_t_lifecycle "`dynamic_buffer_stream_t` lifecycle unit tests section"
 */
typedef enum {
	DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_DYNAMIC_BUFFER_STREAM_HANDLE_OOM = 0,
	DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_DYNAMIC_BUFFER_OOM,
	DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_OK_STATE_INJECTED_BY_TEST_INFRA,
	DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_OK_STATE_CREATED_BY_DEFAULT_INIT
} dynamic_buffer_stream_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	dynamic_buffer_stream_t_lifecycle_scenario_t scenario;
} test_dynamic_buffer_stream_t_lifecycle_case_t;

typedef struct {
	char *buf;
	size_t buf_size;
	const test_dynamic_buffer_stream_t_lifecycle_case_t *tc;
} test_dynamic_buffer_stream_t_lifecycle_fixture_t;

static int setup_dynamic_buffer_stream_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(dynamic_buffer_stream_t_lifecycle, state, tc, fx);

	fake_memory_reset();
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_OK_STATE_INJECTED_BY_TEST_INFRA) {
		fx->buf_size = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY;
		fx->buf = osal_mem_test_fake_ops()->malloc(fx->buf_size * sizeof(char));
		assert_non_null(fx->buf);
	}

	*state = fx;
	return 0;
}

static int teardown_dynamic_buffer_stream_t_lifecycle(void **state)
{
	test_dynamic_buffer_stream_t_lifecycle_fixture_t *fx = (test_dynamic_buffer_stream_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_dynamic_buffer_stream_t_lifecycle(void **state)
{
	test_dynamic_buffer_stream_t_lifecycle_fixture_t *fx = (test_dynamic_buffer_stream_t_lifecycle_fixture_t *)(*state);
	const test_dynamic_buffer_stream_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	dynamic_buffer_stream_t *sentinel = (dynamic_buffer_stream_t *)(uintptr_t)0xDEADC0DEu;
	dynamic_buffer_stream_t *dynamic_buffer_stream = sentinel;
	const dynamic_buffer_stream_env_t dynamic_buffer_stream_env = {
		.mem_ops = osal_mem_test_fake_ops()
	};
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_DYNAMIC_BUFFER_STREAM_HANDLE_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	dynamic_buffer_stream_status_t ret = dynamic_buffer_stream_create(&dynamic_buffer_stream, &dynamic_buffer_stream_env);

	// ASSERT
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_DYNAMIC_BUFFER_STREAM_HANDLE_OOM) {
		assert_int_equal(ret, DYNAMIC_BUFFER_STREAM_STATUS_OOM);
		assert_ptr_equal(dynamic_buffer_stream, sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}
	assert_int_equal(ret, DYNAMIC_BUFFER_STREAM_STATUS_OK);
	assert_true(dynamic_buffer_stream != sentinel);
	assert_non_null(dynamic_buffer_stream);
	assert_ptr_equal(dynamic_buffer_stream_get_mem_ops(dynamic_buffer_stream), dynamic_buffer_stream_env.mem_ops);
	assert_null(dynamic_buffer_stream_get_state(dynamic_buffer_stream).dbuf.buf);

	// ARRANGE
	dynamic_buffer_stream_cfg_t dynamic_buffer_stream_cfg = dynamic_buffer_stream_default_cfg();
	dynamic_buffer_stream_state_t injectable_dynamic_buffer_stream_state = {
		.dbuf = {
			.buf = fx->buf,
			.cap = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY,
			.len = 0,
			.read_pos = 0
		}
	};
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_OK_STATE_INJECTED_BY_TEST_INFRA) {
		dynamic_buffer_stream_inject_state(dynamic_buffer_stream, injectable_dynamic_buffer_stream_state);
	}
	const osal_mem_ops_t *mem_ops_snapshot = dynamic_buffer_stream_get_mem_ops(dynamic_buffer_stream);

	// ARRANGE
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_DYNAMIC_BUFFER_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	ret = dynamic_buffer_stream_complete_default_init(dynamic_buffer_stream, &dynamic_buffer_stream_cfg);

	// ASSERT
	assert_ptr_equal(dynamic_buffer_stream_get_mem_ops(dynamic_buffer_stream), mem_ops_snapshot);
	dynamic_buffer_stream_state_t dynamic_buffer_stream_state = dynamic_buffer_stream_get_state(dynamic_buffer_stream);
	switch (tc->scenario) {
		case DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_DYNAMIC_BUFFER_OOM:
			assert_int_equal(ret, DYNAMIC_BUFFER_STREAM_STATUS_OOM);
			assert_null(dynamic_buffer_stream_state.dbuf.buf);
			assert_true(dynamic_buffer_stream_state.dbuf.cap == 0);
			assert_true(dynamic_buffer_stream_state.dbuf.len == 0);
			assert_true(dynamic_buffer_stream_state.dbuf.read_pos == 0);
			break;
		case DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_OK_STATE_INJECTED_BY_TEST_INFRA:
			assert_int_equal(ret, DYNAMIC_BUFFER_STREAM_STATUS_OK);
			assert_ptr_equal(
				dynamic_buffer_stream_state.dbuf.buf,
				injectable_dynamic_buffer_stream_state.dbuf.buf
			);
			assert_int_equal(
				dynamic_buffer_stream_state.dbuf.cap,
				injectable_dynamic_buffer_stream_state.dbuf.cap
			);
			assert_int_equal(
				dynamic_buffer_stream_state.dbuf.len,
				injectable_dynamic_buffer_stream_state.dbuf.len
			);
			assert_int_equal(
				dynamic_buffer_stream_state.dbuf.read_pos,
				injectable_dynamic_buffer_stream_state.dbuf.read_pos
			);
			break;
		case DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_OK_STATE_CREATED_BY_DEFAULT_INIT:
			assert_int_equal(ret, DYNAMIC_BUFFER_STREAM_STATUS_OK);
			assert_non_null(dynamic_buffer_stream_state.dbuf.buf);
			assert_true(dynamic_buffer_stream_state.dbuf.cap > 0);
			assert_true(dynamic_buffer_stream_state.dbuf.len == 0);
			assert_true(dynamic_buffer_stream_state.dbuf.read_pos == 0);
			break;
		default: fail();
	}
	assert_ptr_equal(dynamic_buffer_stream_get_mem_ops(dynamic_buffer_stream), mem_ops_snapshot);

	// ACT
	ret = dynamic_buffer_stream_vtbl()->close(dynamic_buffer_stream);

	// ASSERT
	assert_int_equal(ret, STREAM_STATUS_OK);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_dynamic_buffer_stream_t_lifecycle_case_t CASE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_DYNAMIC_BUFFER_STREAM_HANDLE_OOM = {
	.name = "dynamic_buffer_stream_t_lifecycle_dynamic_buffer_stream_handle_oom",
	.scenario = DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_DYNAMIC_BUFFER_STREAM_HANDLE_OOM,
};

static const test_dynamic_buffer_stream_t_lifecycle_case_t CASE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_DYNAMIC_BUFFER_OOM = {
	.name = "dynamic_buffer_stream_t_lifecycle_dynamic_buffer_oom",
	.scenario = DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_DYNAMIC_BUFFER_OOM,
};

static const test_dynamic_buffer_stream_t_lifecycle_case_t CASE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_OK_STATE_INJECTED_BY_TEST_INFRA = {
	.name = "dynamic_buffer_stream_t_lifecycle_ok_state_injected_by_test_infra",
	.scenario = DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_OK_STATE_INJECTED_BY_TEST_INFRA,
};

static const test_dynamic_buffer_stream_t_lifecycle_case_t CASE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_OK_STATE_CREATED_BY_DEFAULT_INIT = {
	.name = "dynamic_buffer_stream_t_lifecycle_ok_state_created_by_default_init",
	.scenario = DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_SCENARIO_OK_STATE_CREATED_BY_DEFAULT_INIT,
};

#define DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_CASES(X) \
X(CASE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_DYNAMIC_BUFFER_STREAM_HANDLE_OOM) \
X(CASE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_DYNAMIC_BUFFER_OOM) \
X(CASE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_OK_STATE_INJECTED_BY_TEST_INFRA) \
X(CASE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_OK_STATE_CREATED_BY_DEFAULT_INIT)

#define MAKE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(dynamic_buffer_stream_t_lifecycle, case_sym)

static const struct CMUnitTest dynamic_buffer_stream_t_lifecycle_tests[] = {
	DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_CASES(MAKE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_TEST)
};

#undef DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_CASES
#undef MAKE_DYNAMIC_BUFFER_STREAM_T_LIFECYCLE_TEST

/** @endcond */

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_adapter_provider_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_create_adapter_provider "dynamic_buffer_stream_create_adapter_provider".
 * - @ref specifications_stream_destroy_adapter_provider "stream_destroy_adapter_provider() specifications"
 *
 * See test description:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_stream_adapter_provider_t_lifecycle "`stream_adapter_provider_t` lifecycle unit tests section"
 */
typedef enum {
	DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM = 0,
	DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OK
} dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_scenario_t scenario;
} test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_case_t;

typedef struct {
	const test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_case_t *tc;
} test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_fixture_t;

static int setup_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(dynamic_buffer_stream_stream_adapter_provider_t_lifecycle, state, tc, fx);

	fake_memory_reset();

	*state = fx;
	return 0;
}

static int teardown_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle(void **state)
{
	test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_fixture_t *fx =
		(test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle(void **state)
{
	test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_fixture_t *fx =
		(test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_fixture_t *)(*state);
	const test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	stream_adapter_provider_t *sentinel = (stream_adapter_provider_t *)(uintptr_t)0xDEADC0DEu;
	stream_adapter_provider_t *stream_adapter_provider = sentinel;
	dynamic_buffer_stream_cfg_t dynamic_buffer_stream_cfg = dynamic_buffer_stream_default_cfg();
	const dynamic_buffer_stream_env_t dynamic_buffer_stream_env = {
		.mem_ops = osal_mem_test_fake_ops()
	};
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	dynamic_buffer_stream_status_t ret = dynamic_buffer_stream_create_adapter_provider(&stream_adapter_provider, &dynamic_buffer_stream_cfg, &dynamic_buffer_stream_env);

	// ASSERT
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM) {
		assert_int_equal(ret, DYNAMIC_BUFFER_STREAM_STATUS_OOM);
		assert_ptr_equal(stream_adapter_provider, sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}
	assert_int_equal(ret, DYNAMIC_BUFFER_STREAM_STATUS_OK);
	assert_true(stream_adapter_provider != sentinel);
	assert_non_null(stream_adapter_provider);
	assert_non_null(stream_adapter_provider->backend_ctor);
	assert_ptr_equal(stream_adapter_provider->vtbl, dynamic_buffer_stream_vtbl());
	assert_non_null(stream_adapter_provider->ud);
	assert_non_null(stream_adapter_provider->ud_dtor);
	assert_ptr_equal(stream_adapter_provider->mem, dynamic_buffer_stream_env.mem_ops);

	// ACT
	stream_destroy_adapter_provider(stream_adapter_provider);

	// ASSERT
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_case_t CASE_DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OOM = {
	.name = "dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_oom",
	.scenario = DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM,
};

static const test_dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_case_t CASE_DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OK = {
	.name = "dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_ok",
	.scenario = DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OK,
};

#define DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_CASES(X) \
X(CASE_DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OOM) \
X(CASE_DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OK)

#define MAKE_DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(dynamic_buffer_stream_stream_adapter_provider_t_lifecycle, case_sym)

static const struct CMUnitTest dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_tests[] = {
	DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_CASES(MAKE_DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_TEST)
};

#undef DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_CASES
#undef MAKE_DYNAMIC_BUFFER_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest dynamic_buffer_stream_cr_tests_non_parametric[] = {
		cmocka_unit_test(test_dynamic_buffer_stream_vtbl),
		cmocka_unit_test(test_dynamic_buffer_stream_default_cfg),
		cmocka_unit_test(test_dynamic_buffer_stream_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(dynamic_buffer_stream_cr_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(dynamic_buffer_stream_t_lifecycle_tests, NULL, NULL);
	failed += cmocka_run_group_tests(dynamic_buffer_stream_stream_adapter_provider_t_lifecycle_tests, NULL, NULL);

	return failed;
}
/** @endcond */
