/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_stream_regular_file_creator_cr.c
 * @ingroup stream_unit_tests
 * @brief Unit tests implementation for stream_regular_file_creator CR.
 *
 * @details
 * This file implements the unit-level validation of the
 * `stream_regular_file_creator_t` lifecycle exposed by the Composition Root.
 *
 * See also:
 * - @ref testing_foundation_stream_regular_file_creator_cr_unit "stream_regular_file_creator CR unit tests page"
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
 * @brief Test scenarios for `stream_regular_file_creator_t` lifecycle.
 *
 * Doubles:
 * - fake_adapter
 * - fake_memory
 *
 * See contract:
 * - @ref specifications_stream_create_regular_file_creator "stream_create_regular_file_creator() specifications".
 * - @ref specifications_stream_destroy_regular_file_creator "stream_destroy_regular_file_creator() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_regular_file_creator_t_lifecycle "`stream_regular_file_creator_t` lifecycle unit tests section"
 */
typedef enum {
	STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OOM = 0,
	STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OK,
	STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OK_DESTROY_TWICE
} stream_regular_file_creator_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	stream_regular_file_creator_t_lifecycle_scenario_t scenario;
} test_stream_regular_file_creator_t_lifecycle_case_t;

typedef struct {
	const osal_mem_ops_t *real_mem;
	const osal_mem_ops_t *fake_mem;
	stream_regular_file_creator_t *regular_file_creator;
	stream_factory_t *factory;
	stream_fake_adapter_backend_t fake_adapter_backend;
	stream_adapter_provider_t *fake_adapter_provider;
	stream_fake_adapter_backend_ctor_mock_spy_t ctor_mock_spy;
	stream_adapter_id_t fake_adapter_id;

	const test_stream_regular_file_creator_t_lifecycle_case_t *tc;
} test_stream_regular_file_creator_t_lifecycle_fixture_t;

static int setup_stream_regular_file_creator_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_regular_file_creator_t_lifecycle, state, tc, fx);

	fx->real_mem = osal_mem_default_ops();
	const stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
	assert_int_equal(stream_factory_cfg.fact_cap, 16);
	assert_int_equal(stream_create_factory(&fx->factory, &stream_factory_cfg, fx->real_mem), STREAM_FACTORY_STATUS_OK);
	stream_fake_adapter_init_backend(&fx->fake_adapter_backend);
	assert_int_equal(
		stream_fake_adapter_create_provider(
			&fx->fake_adapter_provider,
			&fx->fake_adapter_backend,
			fx->real_mem,
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
	fake_memory_reset();
	fx->fake_mem = osal_mem_test_fake_ops();

	*state = fx;
	return 0;
}

static int teardown_stream_regular_file_creator_t_lifecycle(void **state)
{
	test_stream_regular_file_creator_t_lifecycle_fixture_t *fx = (test_stream_regular_file_creator_t_lifecycle_fixture_t *)(*state);
	stream_destroy_factory(&fx->factory);
	osal_free(fx);
	return 0;
}

static void test_stream_regular_file_creator_t_lifecycle(void **state)
{
	test_stream_regular_file_creator_t_lifecycle_fixture_t *fx = (test_stream_regular_file_creator_t_lifecycle_fixture_t *)(*state);
	const test_stream_regular_file_creator_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	if (tc->scenario == STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OOM) { fake_memory_fail_only_on_call(1); }
	stream_regular_file_creator_t *file_creator_snapshot = fx->regular_file_creator;

	// ACT
	stream_factory_status_t ret = stream_create_regular_file_creator(&fx->regular_file_creator, fx->factory, fx->fake_adapter_id, fx->fake_mem);

	// ASSERT
	switch (tc->scenario) {
		case STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OOM:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_OOM);
			assert_ptr_equal(fx->regular_file_creator, file_creator_snapshot);
			assert_true(fake_memory_no_leak());
			assert_true(fake_memory_no_invalid_free());
			assert_true(fake_memory_no_double_free());
			return;
		case STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OK_DESTROY_TWICE:
		case STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OK:
			assert_int_equal(ret, STREAM_FACTORY_STATUS_OK);
			assert_non_null(fx->regular_file_creator);
			assert_true(fx->regular_file_creator != file_creator_snapshot);
			break;
		default: fail();
	}

	// ACT
	stream_destroy_regular_file_creator(&fx->regular_file_creator);
	if (tc->scenario == STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OK_DESTROY_TWICE) {
		stream_destroy_regular_file_creator(&fx->regular_file_creator);
	}

	// ASSERT
	assert_null(fx->regular_file_creator);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_stream_regular_file_creator_t_lifecycle_case_t CASE_STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_OOM = {
	.name = "stream_regular_file_creator_t_lifecycle_oom",
	.scenario = STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OOM,
};

static const test_stream_regular_file_creator_t_lifecycle_case_t CASE_STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_OK = {
	.name = "stream_regular_file_creator_t_lifecycle_ok",
	.scenario = STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OK,
};

static const test_stream_regular_file_creator_t_lifecycle_case_t CASE_STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_OK_DESTROY_TWICE = {
	.name = "stream_regular_file_creator_t_lifecycle_ok_destroy_twice",
	.scenario = STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_SCENARIO_OK_DESTROY_TWICE,
};

#define STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_CASES(X) \
X(CASE_STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_OOM) \
X(CASE_STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_OK) \
X(CASE_STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_OK_DESTROY_TWICE)

#define MAKE_STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_regular_file_creator_t_lifecycle, case_sym)

static const struct CMUnitTest stream_regular_file_creator_t_lifecycle_tests[] = {
	STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_CASES(MAKE_STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_TEST)
};

#undef STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_CASES
#undef MAKE_STREAM_REGULAR_FILE_CREATOR_T_LIFECYCLE_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(stream_regular_file_creator_t_lifecycle_tests, NULL, NULL);

	return failed;
}
/** @endcond */
