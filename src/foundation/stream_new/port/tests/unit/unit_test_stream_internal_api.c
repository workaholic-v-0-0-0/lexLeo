/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_stream_internal_api.c
 * @ingroup stream_unit_tests
 * @brief Unit tests implementation for stream/src/internal/stream_internal_api.h.
 *
 * See also:
 * - @ref testing_foundation_stream_internal_api_unit
 * "Internal stream API unit tests section"
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

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_factory_create_stream()`.
 *
 * See contract:
 * - @ref specifications_stream_factory_create_stream "stream_factory_create_stream() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_factory_create_stream "stream_factory_create_stream() unit tests section"
 */
typedef enum {
	STREAM_FACTORY_CREATE_STREAM_SCENARIO_ADAPTER_PROVIDER_NOT_FOUND = 0,
	STREAM_FACTORY_CREATE_STREAM_SCENARIO_BACKEND_CTOR_OOM,
	STREAM_FACTORY_CREATE_STREAM_SCENARIO_STREAM_HANDLE_OOM,
	STREAM_FACTORY_CREATE_STREAM_SCENARIO_OK
} stream_factory_create_stream_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	stream_factory_create_stream_scenario_t scenario;
} test_stream_factory_create_stream_case_t;

typedef struct {
	const osal_mem_ops_t *fake_mem;

	// SUT resource
	stream_t *stream;

	// Test infrastructure resources
	stream_factory_t *factory;
	stream_fake_adapter_backend_t fake_adapter_backend;
	stream_adapter_provider_t *fake_adapter_provider;
	stream_fake_adapter_backend_ctor_mock_spy_t ctor_mock_spy;
	stream_adapter_id_t fake_adapter_id;

	const test_stream_factory_create_stream_case_t *tc;
} test_stream_factory_create_stream_fixture_t;

static int setup_stream_factory_create_stream(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_factory_create_stream, state, tc, fx);

	fake_memory_reset();
	fx->fake_mem = osal_mem_test_fake_ops();
	const stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
	assert_int_equal(stream_factory_cfg.fact_cap, 16);
	assert_int_equal(stream_create_factory(&fx->factory, &stream_factory_cfg, fx->fake_mem), STREAM_FACTORY_STATUS_OK);
	stream_fake_adapter_init_backend(&fx->fake_adapter_backend);
	assert_int_equal(
		stream_fake_adapter_create_provider(
			&fx->fake_adapter_provider,
			&fx->fake_adapter_backend,
			fx->fake_mem,
			&fx->ctor_mock_spy
		),
		STREAM_STATUS_OK
	);
	fx->fake_adapter_id = "fake_adapter_id";
	assert_int_equal(
		stream_factory_add_adapter(
			fx->factory,
			fx->fake_adapter_id,
			fx->fake_adapter_provider
		),
		STREAM_FACTORY_STATUS_OK
	);

	*state = fx;
	return 0;
}

static int teardown_stream_factory_create_stream(void **state)
{
	test_stream_factory_create_stream_fixture_t *fx = (test_stream_factory_create_stream_fixture_t *)(*state);
	stream_destroy(&fx->stream);
	stream_destroy_factory(&fx->factory);
	osal_free(fx);
	return 0;
}

static void test_stream_factory_create_stream(void **state)
{
	test_stream_factory_create_stream_fixture_t *fx = (test_stream_factory_create_stream_fixture_t *)(*state);
	const test_stream_factory_create_stream_case_t *tc = fx->tc;

	// ARRANGE
	const void *args_arg = (void *)(uintptr_t)0xDEADC0DEu;
	stream_adapter_id_t adapter_id_arg = fx->fake_adapter_id;
	if (tc->scenario == STREAM_FACTORY_CREATE_STREAM_SCENARIO_ADAPTER_PROVIDER_NOT_FOUND) { adapter_id_arg = "not_registered_adapter_id"; }
	if (tc->scenario == STREAM_FACTORY_CREATE_STREAM_SCENARIO_BACKEND_CTOR_OOM) { fx->ctor_mock_spy.next_ret = STREAM_STATUS_OOM; }
	if (tc->scenario == STREAM_FACTORY_CREATE_STREAM_SCENARIO_STREAM_HANDLE_OOM) { fake_memory_fail_only_on_call(1); }
	stream_t *out_snapshot = fx->stream;

	// ACT
	stream_factory_status_t ret =
		stream_get_internal_api()
			->stream_factory_create_stream(
				fx->factory,
				adapter_id_arg,
				args_arg,&fx->stream
			);

	// ASSERT
	switch (tc->scenario) {
		case STREAM_FACTORY_CREATE_STREAM_SCENARIO_ADAPTER_PROVIDER_NOT_FOUND:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_NOT_FOUND);
			assert_int_equal(fx->ctor_mock_spy.call_count, 0);
			assert_ptr_equal(fx->stream, out_snapshot);
			break;
		case STREAM_FACTORY_CREATE_STREAM_SCENARIO_BACKEND_CTOR_OOM:
			assert_int_equal(fx->ctor_mock_spy.call_count, 1);
			assert_ptr_equal(fx->ctor_mock_spy.last_args_arg, args_arg);
			assert_non_null(fx->ctor_mock_spy.last_out_backend_arg);
			/* fall through */
		case STREAM_FACTORY_CREATE_STREAM_SCENARIO_STREAM_HANDLE_OOM:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_OOM);
			assert_ptr_equal(fx->stream, out_snapshot);
			break;
		case STREAM_FACTORY_CREATE_STREAM_SCENARIO_OK:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_OK);
			assert_int_equal(fx->ctor_mock_spy.call_count, 1);
			assert_ptr_equal(fx->ctor_mock_spy.last_args_arg, args_arg);
			assert_non_null(fx->ctor_mock_spy.last_out_backend_arg);
			assert_ptr_equal(stream_get_backend(fx->stream), &fx->fake_adapter_backend);
			assert_ptr_equal(stream_get_mem(fx->stream), fx->fake_adapter_provider->mem);
			assert_ptr_equal(stream_get_vtbl(fx->stream), fx->fake_adapter_provider->vtbl);
			break;
		default: fail();
	}
}

static const test_stream_factory_create_stream_case_t CASE_STREAM_FACTORY_CREATE_STREAM_ADAPTER_PROVIDER_NOT_FOUND = {
	.name = "stream_factory_create_stream_adapter_provider_not_found",
	.scenario = STREAM_FACTORY_CREATE_STREAM_SCENARIO_ADAPTER_PROVIDER_NOT_FOUND,
};

static const test_stream_factory_create_stream_case_t CASE_STREAM_FACTORY_CREATE_STREAM_BACKEND_CTOR_OOM = {
	.name = "stream_factory_create_stream_backend_ctor_oom",
	.scenario = STREAM_FACTORY_CREATE_STREAM_SCENARIO_BACKEND_CTOR_OOM,
};

static const test_stream_factory_create_stream_case_t CASE_STREAM_FACTORY_CREATE_STREAM_STREAM_HANDLE_OOM = {
	.name = "stream_factory_create_stream_stream_handle_oom",
	.scenario = STREAM_FACTORY_CREATE_STREAM_SCENARIO_STREAM_HANDLE_OOM,
};

static const test_stream_factory_create_stream_case_t CASE_STREAM_FACTORY_CREATE_STREAM_OK = {
	.name = "stream_factory_create_stream_ok",
	.scenario = STREAM_FACTORY_CREATE_STREAM_SCENARIO_OK,
};

#define STREAM_FACTORY_CREATE_STREAM_CASES(X) \
X(CASE_STREAM_FACTORY_CREATE_STREAM_ADAPTER_PROVIDER_NOT_FOUND) \
X(CASE_STREAM_FACTORY_CREATE_STREAM_BACKEND_CTOR_OOM) \
X(CASE_STREAM_FACTORY_CREATE_STREAM_STREAM_HANDLE_OOM) \
X(CASE_STREAM_FACTORY_CREATE_STREAM_OK)

#define MAKE_STREAM_FACTORY_CREATE_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_factory_create_stream, case_sym)

static const struct CMUnitTest stream_factory_create_stream_tests[] = {
	STREAM_FACTORY_CREATE_STREAM_CASES(MAKE_STREAM_FACTORY_CREATE_STREAM_TEST)
};

#undef STREAM_FACTORY_CREATE_STREAM_CASES
#undef MAKE_STREAM_FACTORY_CREATE_STREAM_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(stream_factory_create_stream_tests, NULL, NULL);
	return failed;
}
/** @endcond */
