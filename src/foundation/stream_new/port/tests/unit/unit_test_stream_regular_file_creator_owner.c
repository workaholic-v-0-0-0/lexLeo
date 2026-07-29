/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_stream_regular_file_creator_owner.c
 * @ingroup stream_unit_tests
 * @brief Unit tests implementation for stream_regular_file_creator_owner.
 *
 * @details
 * This file implements the unit-level validation of unit_test_stream_regular_file_creator_owner.
 *
 * See also:
 * - @ref testing_foundation_stream_regular_file_creator_owner_unit "stream regular file creator owner unit tests"
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
 * @brief Test scenarios for `stream_regular_file_creator_create()`.
 *
 * See contract:
 * - @ref specifications_stream_regular_file_creator_create "stream_regular_file_creator_create() specifications".
 * - @ref specifications_stream_destroy "stream_destroy() specifications"
 * - @ref specifications_stream_create_regular_file_creator "stream_create_regular_file_creator() specifications".
 * - @ref specifications_stream_destroy_regular_file_creator "stream_destroy_regular_file_creator() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_regular_file_creator_create "`stream_regular_file_creator_create()` unit tests section"
 */
typedef enum {
	STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_NULL = 0,
	STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_EMPTY,
	STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_TOO_LONG,
	STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_OOM,
	STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_OK,
} stream_regular_file_creator_create_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	stream_regular_file_creator_create_scenario_t scenario;
} test_stream_regular_file_creator_create_case_t;

typedef struct {
	// SUT resources.
	stream_t *stream;

	// Test infrastructure resources.
	const osal_mem_ops_t *fake_mem;
	stream_regular_file_creator_t *regular_file_creator;
	stream_factory_t *factory;
	stream_fake_adapter_backend_t fake_adapter_backend;
	stream_adapter_provider_t *fake_adapter_provider;
	stream_fake_adapter_backend_ctor_mock_spy_t ctor_mock_spy;
	stream_adapter_id_t fake_adapter_id;

	const test_stream_regular_file_creator_create_case_t *tc;
} test_stream_regular_file_creator_create_fixture_t;

static int setup_stream_regular_file_creator_create(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_regular_file_creator_create, state, tc, fx);

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
	assert_int_equal(
		stream_create_regular_file_creator(
			&fx->regular_file_creator,
			fx->factory,
			fx->fake_adapter_id,
			fx->fake_mem
		),
		STREAM_FACTORY_STATUS_OK
	);

	*state = fx;
	return 0;
}

static int teardown_stream_regular_file_creator_create(void **state)
{
	test_stream_regular_file_creator_create_fixture_t *fx = (test_stream_regular_file_creator_create_fixture_t *)(*state);
	stream_destroy(&fx->stream);
	stream_destroy_regular_file_creator(&fx->regular_file_creator);
	stream_destroy_factory(&fx->factory);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
	osal_free(fx);
	return 0;
}

static void test_stream_regular_file_creator_create(void **state) {
	test_stream_regular_file_creator_create_fixture_t *fx = (test_stream_regular_file_creator_create_fixture_t *)(*state);
	const test_stream_regular_file_creator_create_case_t *tc = fx->tc;

	// ARRANGE
	const char *arg_path = NULL;
	const char *null_path = NULL;
	const char *empty_path = "";
	char too_long_path[257];
	for (size_t i = 0; i < 256; i++) {
		too_long_path[i] = 'a';
	}
	const char *valid_path = "valid_path";
	too_long_path[256] = '\0';
	switch (tc->scenario) {
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_NULL: arg_path = null_path; break;
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_EMPTY: arg_path = empty_path; break;
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_TOO_LONG: arg_path = too_long_path; break;
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_OOM: fake_memory_fail_only_on_call(1); arg_path = valid_path; break;
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_OK: arg_path = valid_path; break;
		default: fail();
	}
	stream_t *out_snapshot = fx->stream;

	// ACT
	stream_status_t ret =
		stream_regular_file_creator_create(
			fx->regular_file_creator,
			arg_path,
			"wb",
			&fx->stream
		);

	// ASSERT
	switch (tc->scenario) {
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_NULL:
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_EMPTY:
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_TOO_LONG:
			assert_int_equal(ret, STREAM_STATUS_INVALID);
			assert_ptr_equal(fx->stream, out_snapshot);
			break;
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_OOM:
			assert_int_equal(ret, STREAM_STATUS_OOM);
			assert_ptr_equal(fx->stream, out_snapshot);
			break;
		case STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_OK:
			assert_int_equal(ret, STREAM_STATUS_OK);
			assert_true(fx->stream != out_snapshot);
			assert_non_null(fx->stream);
			break;
		default: fail();
	}
}

static const test_stream_regular_file_creator_create_case_t CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_PATH_NULL = {
	.name = "stream_regular_file_creator_create_path_null",
	.scenario = STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_NULL,
};

static const test_stream_regular_file_creator_create_case_t CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_PATH_EMPTY = {
	.name = "stream_regular_file_creator_create_path_empty",
	.scenario = STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_EMPTY,
};

static const test_stream_regular_file_creator_create_case_t CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_PATH_TOO_LONG = {
	.name = "stream_regular_file_creator_create_path_too_long",
	.scenario = STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_PATH_TOO_LONG,
};

static const test_stream_regular_file_creator_create_case_t CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_OOM = {
	.name = "stream_regular_file_creator_create_oom",
	.scenario = STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_OOM,
};

static const test_stream_regular_file_creator_create_case_t CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_OK = {
	.name = "stream_regular_file_creator_create_ok",
	.scenario = STREAM_REGULAR_FILE_CREATOR_CREATE_SCENARIO_OK,
};

#define STREAM_REGULAR_FILE_CREATOR_CREATE_CASES(X) \
X(CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_PATH_NULL) \
X(CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_PATH_EMPTY) \
X(CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_PATH_TOO_LONG) \
X(CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_OOM) \
X(CASE_STREAM_REGULAR_FILE_CREATOR_CREATE_OK)

#define MAKE_STREAM_REGULAR_FILE_CREATOR_CREATE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_regular_file_creator_create, case_sym)

static const struct CMUnitTest stream_regular_file_creator_create_tests[] = {
	STREAM_REGULAR_FILE_CREATOR_CREATE_CASES(MAKE_STREAM_REGULAR_FILE_CREATOR_CREATE_TEST)
};

#undef STREAM_REGULAR_FILE_CREATOR_CREATE_CASES
#undef MAKE_STREAM_REGULAR_FILE_CREATOR_CREATE_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(stream_regular_file_creator_create_tests, NULL, NULL);
	return failed;
}
/** @endcond */
