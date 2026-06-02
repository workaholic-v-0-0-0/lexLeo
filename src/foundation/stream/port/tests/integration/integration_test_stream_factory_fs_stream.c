/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_stream_factory_fs_stream.c
 * @ingroup stream_integration_tests
 * @brief Integration tests implementation for `stream_factory_create_stream()`
 * with the `fs_stream` adapter.
 *
 * @details
 * This file implements integration-level validation of the interaction between
 * the stream factory and the `fs_stream` adapter registered through the
 * Composition Root API.
 *
 * Covered surfaces:
 * - factory-based stream creation: `stream_factory_create_stream()`
 * - integration of factory lookup with `fs_stream` descriptor registration
 * - propagation of backend file-open failures through the created stream path
 *
 * Test strategy:
 * - parametric scenario-based testing
 * - explicit validation of argument checking and output-handle preservation
 * - integration-focused exercising of the public factory API with a real
 *   adapter descriptor produced by `fs_stream_create_desc()`
 * - backend fault injection through `fake_file`
 * - validation that successfully created streams are usable through the
 *   public borrower API
 *
 * Local test doubles:
 * - `fake_file`
 *
 * See also:
 * - @ref testing_foundation_stream_factory_fs_stream_integration "stream factory integration tests page"
 * - @ref specifications_stream "stream specifications"
 */

#include "fs_stream/cr/fs_stream_cr_api.h"

#include "stream/cr/stream_cr_api.h"

#include "stream/tests/stream_white_box_tests_access.h"

#include "osal/file/test/osal_file_fake_provider.h"

#include "osal/mem/osal_mem.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**
 * @brief Scenarios for `stream_factory_create_stream()` / `fs_stream` integration.
 *
 * stream_status_t stream_factory_create_stream(
 *     const stream_factory_t *f,
 *     stream_key_t key,
 *     const void *args,
 *     stream_t **out );
 *
 * Doubles:
 * - `fake_file`
 *
 * See contract:
 * - @ref specifications_stream_factory_create_stream "stream_factory_create_stream() specifications"
 *
 * See test description:
 * - @ref testing_foundation_stream_integration_stream_factory_create_stream_fs_stream "stream_factory_create_stream() / fs_stream integration section"
 *
 * The scenarios below define the test oracle for
 * `stream_factory_create_stream()` with the `fs_stream` adapter.
 */
typedef enum {
	/**
	 * WHEN `stream_factory_create_stream(f, key, args, out)` is called with a
	 * valid factory, a registered `fs_stream` key, valid `fs_stream` arguments,
	 * and backend file open succeeds
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - stores a non-`NULL` stream handle in `*out`
	 * - the produced stream is ready for normal runtime use
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_OK = 0,

	/**
	 * WHEN `args == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_ARGS_NULL,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - no stream handle is produced
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_OUT_NULL,

	/**
	 * WHEN `args != NULL` but `args->path == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_PATH_NULL,

	/**
	 * WHEN `args != NULL` but `args->path` is an empty string
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_PATH_EMPTY,

	/**
	 * WHEN `stream_factory_create_stream(f, key, args, out)` is called with a
	 * valid factory, a registered `fs_stream` key, valid `fs_stream` arguments,
	 * and backend file open fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_IO_ERROR`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_OPEN_FAIL,

	/**
	 * WHEN `f == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_FACTORY_NULL,

	/**
	 * WHEN `args != NULL` but `args->mode == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_MODE_NULL,

	/**
	 * WHEN `args != NULL` but `args->mode` is not a supported mode
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_MODE_INVALID,

	/**
	 * WHEN `key == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_KEY_NULL,

	/**
	 * WHEN `key` is an empty string
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_KEY_EMPTY,

	/**
	 * WHEN `f` designates a valid factory instance but `key` is not registered
	 * in that factory
	 * EXPECT:
	 * - returns `STREAM_STATUS_NOT_FOUND`
	 * - leaves `*out` unchanged
	 */
	STREAM_FACT_CREATE_FS_SCENARIO_KEY_UNKNOWN,
} stream_fact_create_fs_scenario_t;

/** @cond INTERNAL */

/**
 * @brief Expected state of the output handle after the call under test.
 *
 * Notes:
 * - `OUT_CHECK_NONE` disables post-call checks on the output handle.
 * - `OUT_EXPECT_UNCHANGED` is typically verified using a sentinel pointer.
 */
typedef enum {
	OUT_CHECK_NONE,
	OUT_EXPECT_NULL,
	OUT_EXPECT_NON_NULL,
	OUT_EXPECT_UNCHANGED
} out_expect_t;

/**
 * @brief One parametric test case for
 * `stream_factory_create_stream()` / `fs_stream` integration.
 */
typedef struct {
	const char *name;

	// arrange
	stream_fact_create_fs_scenario_t scenario;
	osal_file_status_t open_fail_status; // e.g. OSAL_FILE_NOENT

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_stream_fact_create_fs_case_t;

/**
 * @brief Runtime fixture for
 * `stream_factory_create_stream()` / `fs_stream` integration tests.
 */
typedef struct {
	stream_adapter_desc_t desc;
	stream_factory_t *factory;

	stream_t *out;

	fs_stream_cfg_t fs_stream_cfg;
	fs_stream_env_t fs_stream_env;

	stream_factory_cfg_t stream_factory_cfg;

	stream_file_creator_args_t args;

	OSAL_FILE *fake_file;

	const test_stream_fact_create_fs_case_t *tc;
} test_stream_fact_create_fs_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `stream_factory_create_stream()` / `fs_stream` integration tests.
 */
static int setup_stream_fact_create_fs(void **state)
{
	const test_stream_fact_create_fs_case_t *tc =
		(const test_stream_fact_create_fs_case_t *)(*state);

	test_stream_fact_create_fs_fixture_t *fx =
		(test_stream_fact_create_fs_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();

	const osal_mem_ops_t *real_mem = osal_mem_default_ops();
	fx->fake_file = fake_file_create_fake(real_mem);
	assert_non_null(fx->fake_file);

	fake_file_prepare_next_open_file(fx->fake_file);
	fake_file_prepare_next_open_status(OSAL_FILE_STATUS_OK);

	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_OPEN_FAIL) {
		fake_file_prepare_next_open_status(tc->open_fail_status);
	}

	// DI
	fx->fs_stream_cfg = fs_stream_default_cfg();
	fx->fs_stream_env =
		fs_stream_default_env(
			osal_file_test_fake_ops(),
			real_mem,
			real_mem
		);

	fx->stream_factory_cfg = stream_default_factory_cfg();

	stream_status_t st;

	st =
		stream_create_factory(
			&fx->factory,
			&fx->stream_factory_cfg,
			real_mem
		);
	assert_int_equal(st, STREAM_STATUS_OK);

	st =
		fs_stream_create_desc(
			&fx->desc,
			"fs",
			&fx->fs_stream_cfg,
			&fx->fs_stream_env,
			osal_mem_default_ops()
		);
	assert_int_equal(st, STREAM_STATUS_OK);

	st =
		stream_factory_add_adapter(
			fx->factory,
			&fx->desc
		);
	assert_int_equal(st, STREAM_STATUS_OK);

	fx->args.path = "crazy_injection.txt";
	fx->args.mode = "wb";

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stream_factory_create_stream()` / `fs_stream`
 * integration test fixture.
 */
static int teardown_stream_fact_create_fs(void **state)
{
	test_stream_fact_create_fs_fixture_t *fx =
		(test_stream_fact_create_fs_fixture_t *)(*state);

	if (!fx) {
		return 0;
	}

	stream_destroy(&fx->out);
	stream_destroy_factory(&fx->factory);

	if (fx->fake_file) {
		fake_file_destroy_fake(fx->fake_file);
		fx->fake_file = NULL;
	}

	fake_file_reset();

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute the active parametric test scenario for
 * `stream_factory_create_stream()` / `fs_stream` integration.
 */
static void test_stream_fact_create_fs(void **state)
{
	test_stream_fact_create_fs_fixture_t *fx =
		(test_stream_fact_create_fs_fixture_t *)(*state);
	const test_stream_fact_create_fs_case_t *tc = fx->tc;

	stream_status_t st = STREAM_STATUS_INVALID;
	stream_status_t ret = STREAM_STATUS_INVALID;

	const stream_factory_t *factory_arg = fx->factory;
	const stream_file_creator_args_t *args_arg = &fx->args;
	stream_key_t key_arg = fx->desc.key;
	stream_t **out_arg = &fx->out;

	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_ARGS_NULL) args_arg = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_PATH_NULL) fx->args.path = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_PATH_EMPTY) fx->args.path = "";
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_MODE_NULL) fx->args.mode = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_MODE_INVALID) fx->args.mode = "invalid";
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_FACTORY_NULL) factory_arg = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_KEY_NULL) key_arg = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_KEY_EMPTY) key_arg = "";
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_KEY_UNKNOWN) key_arg = "unknown_key";

	if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
		fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu;
	}

	stream_t *out_arg_snapshot = fx->out;

	ret =
		stream_white_box_factory_create_stream(
			factory_arg,
			key_arg,
			args_arg,
			out_arg
		);

	assert_int_equal(ret, tc->expected_ret);

	switch (tc->out_expect) {
		case OUT_CHECK_NONE:
			break;
		case OUT_EXPECT_NULL:
			assert_null(fx->out);
			break;
		case OUT_EXPECT_NON_NULL:
			assert_non_null(fx->out);
			break;
		case OUT_EXPECT_UNCHANGED:
			assert_ptr_equal(out_arg_snapshot, fx->out);
			fx->out = NULL;
			break;
		default:
			fail();
	}

	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_OK) {
		assert_non_null(fx->out);

		const char msg[] = "hello";

		size_t w = stream_write(fx->out, msg, sizeof(msg) - 1, &st);
		assert_int_equal((int)w, (int)(sizeof(msg) - 1));
		assert_int_equal(st, STREAM_STATUS_OK);

		assert_int_equal(stream_flush(fx->out), STREAM_STATUS_OK);

		assert_int_equal(
			(int)fake_file_sink_len(fx->fake_file),
			(int)(sizeof(msg) - 1)
		);
		assert_memory_equal(
			fake_file_sink_backing(fx->fake_file),
			msg,
			sizeof(msg) - 1
		);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_OK = {
	.name = "fs_stream_fact_create_stream_ok",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_OK,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_ARGS_NULL = {
	.name = "fs_stream_fact_create_stream_args_null",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_ARGS_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_OUT_NULL = {
	.name = "fs_stream_fact_create_stream_out_null",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_OUT_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_PATH_NULL = {
	.name = "fs_stream_fact_create_stream_path_null",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_PATH_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_PATH_EMPTY = {
	.name = "fs_stream_fact_create_stream_path_empty",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_PATH_EMPTY,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_OPEN_FAIL = {
	.name = "fs_stream_fact_create_stream_open_fail",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_OPEN_FAIL,
	.open_fail_status = OSAL_FILE_STATUS_IO,

	.expected_ret = STREAM_STATUS_IO_ERROR,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_FACTORY_NULL = {
	.name = "fs_stream_fact_create_stream_factory_null",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_FACTORY_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_KEY_NULL = {
	.name = "fs_stream_fact_create_stream_key_null",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_KEY_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_KEY_EMPTY = {
	.name = "fs_stream_fact_create_stream_key_empty",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_KEY_EMPTY,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_KEY_UNKNOWN = {
	.name = "fs_stream_fact_create_stream_key_unknown",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_KEY_UNKNOWN,

	.expected_ret = STREAM_STATUS_NOT_FOUND,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_MODE_NULL = {
	.name = "fs_stream_fact_create_stream_mode_null",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_MODE_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_MODE_INVALID = {
	.name = "fs_stream_fact_create_stream_mode_invalid",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_MODE_INVALID,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STREAM_FACT_CREATE_FS_CASES(X) \
X(CASE_STREAM_FACT_CREATE_FS_OK) \
X(CASE_STREAM_FACT_CREATE_FS_ARGS_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_OUT_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_PATH_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_PATH_EMPTY) \
X(CASE_STREAM_FACT_CREATE_FS_OPEN_FAIL) \
X(CASE_STREAM_FACT_CREATE_FS_FACTORY_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_KEY_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_KEY_EMPTY) \
X(CASE_STREAM_FACT_CREATE_FS_KEY_UNKNOWN) \
X(CASE_STREAM_FACT_CREATE_FS_MODE_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_MODE_INVALID)

#define STREAM_MAKE_FACT_CREATE_FS_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_fact_create_fs, case_sym)

static const struct CMUnitTest fact_create_fs_tests[] = {
	STREAM_FACT_CREATE_FS_CASES(STREAM_MAKE_FACT_CREATE_FS_TEST)
};

#undef STREAM_FACT_CREATE_FS_CASES
#undef STREAM_MAKE_FACT_CREATE_FS_TEST

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(fact_create_fs_tests, NULL, NULL);
	return failed;
}

/** @endcond */