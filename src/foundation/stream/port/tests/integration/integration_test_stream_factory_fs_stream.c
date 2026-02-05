/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_stream_factory_fs_stream.c
 * @ingroup stream_integration_tests
 * @brief Integration tests for stream_factory_create_stream() with the
 * fs_stream adapter.
 *
 * This test suite verifies that the stream factory correctly interacts with
 * the fs_stream adapter registered through the Composition Root API.
 * It validates argument checking, adapter lookup through the factory,
 * propagation of backend errors, and successful creation of a usable
 * stream instance.
 */

#include "stream/cr/stream_factory_cr_api.h"
#include "fs_stream/cr/fs_stream_cr_api.h"

#include "stream/lifecycle/stream_lifecycle.h"
#include "stream/borrowers/stream.h"
#include "osal/file/test/osal_file_fake_provider.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "lexleo_cmocka_xmacro_helpers.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * @ingroup stream_integration_tests
 *
 * ---------------------------------------------------------------------------
 * CONTRACT — stream_factory_create_stream() / fs_stream integration
 * ---------------------------------------------------------------------------
 *
 * stream_status_t stream_factory_create_stream(
 *     const stream_factory_t *f,
 *     stream_key_t key,
 *     const void *args,
 *     stream_t **out );
 *
 * Invalid arguments:
 * - `f`, `key`, `args`, and `out` must not be NULL.
 * - `key` must not be NULL and must not be an empty string.
 * - `args->path` must not be NULL and must not be an empty string.
 * - `args->flags` must not be zero.
 *
 * Success:
 * - Returns STREAM_STATUS_OK.
 * - Stores a valid stream in `*out`.
 * - The produced stream is ready for normal runtime use.
 * - The produced stream must be destroyed via `stream_destroy()`.
 *
 * Failure:
 * - Returns:
 *     - STREAM_STATUS_INVALID for invalid arguments
 *     - STREAM_STATUS_NOT_FOUND if `key` is not registered
 *     - STREAM_STATUS_IO_ERROR when OSAL file operations fail
 * - Leaves `*out` unchanged if `out` is not NULL.
 *
 * Doubles:
 * - fake_file
 */

typedef enum {
	STREAM_FACT_CREATE_FS_SCENARIO_OK = 0,
	STREAM_FACT_CREATE_FS_SCENARIO_ARGS_NULL,
	STREAM_FACT_CREATE_FS_SCENARIO_OUT_NULL,
	STREAM_FACT_CREATE_FS_SCENARIO_PATH_NULL,
	STREAM_FACT_CREATE_FS_SCENARIO_PATH_EMPTY,
	STREAM_FACT_CREATE_FS_SCENARIO_FLAGS_ZERO,
	STREAM_FACT_CREATE_FS_SCENARIO_OPEN_FAIL,
	STREAM_FACT_CREATE_FS_SCENARIO_FACTORY_NULL,
	STREAM_FACT_CREATE_FS_SCENARIO_KEY_NULL,
	STREAM_FACT_CREATE_FS_SCENARIO_KEY_EMPTY,
	STREAM_FACT_CREATE_FS_SCENARIO_KEY_UNKNOWN,
} stream_fact_create_fs_scenario_t;

typedef enum {
	OUT_CHECK_NONE,
	OUT_EXPECT_NULL,
	OUT_EXPECT_NON_NULL,
	OUT_EXPECT_UNCHANGED
} out_expect_t;

typedef struct {
	const char *name;

	// arrange
	stream_fact_create_fs_scenario_t scenario;
	osal_file_status_t open_fail_status; // e.g. OSAL_FILE_NOENT

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_stream_fact_create_fs_case_t;

typedef struct {
	// initialized in setup
	stream_adapter_desc_t desc;
	stream_factory_t *factory;

	// runtime resource produced by stream_factory_create_stream()
	stream_t *out;

	fs_stream_cfg_t fs_stream_cfg;
	fs_stream_env_t fs_stream_env;

	stream_factory_cfg_t stream_factory_cfg;
	stream_env_t stream_env;

	fs_stream_args_t args;

	// fake file backing
	uint8_t backing[64];

	// reference to test case
	const test_stream_fact_create_fs_case_t *tc;
} test_stream_fact_create_fs_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_stream_fact_create_fs(void **state)
{
	const test_stream_fact_create_fs_case_t *tc =
		(const test_stream_fact_create_fs_case_t *)(*state);

	test_stream_fact_create_fs_fixture_t *fx =
		(test_stream_fact_create_fs_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();

	memset(fx->backing, 0, sizeof(fx->backing));
	fake_file_set_backing(fx->backing, sizeof(fx->backing), 0);
	fake_file_set_pos(0);
	fake_file_fail_disable();

	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_OPEN_FAIL) {
		fake_file_fail_enable(FAKE_FILE_OP_OPEN, 1, tc->open_fail_status);
	}

	// DI
	fx->fs_stream_env.file_env.mem = osal_mem_default_ops();
	fx->fs_stream_env.file_ops = osal_file_test_fake_ops();
	fx->fs_stream_env.port_env.mem = osal_mem_default_ops();
	fx->stream_env.mem = osal_mem_default_ops();

	fx->fs_stream_cfg.reserved = 0;
	fx->stream_factory_cfg.fact_cap = 8;

	stream_status_t st;

	st =
		stream_create_factory(
			&fx->factory,
			&fx->stream_factory_cfg,
			&fx->stream_env );
	assert_int_equal(st, STREAM_STATUS_OK);

	st =
		fs_stream_create_desc(
			&fx->desc,
			"fs",
			&fx->fs_stream_cfg,
			&fx->fs_stream_env,
			fx->stream_env.mem );
	assert_int_equal(st, STREAM_STATUS_OK);

	st =
		stream_factory_add_adapter(
			fx->factory,
			&fx->desc );
	assert_int_equal(st, STREAM_STATUS_OK);

	fx->args.path = "crazy_injection.txt";
	fx->args.flags = OSAL_FILE_READ | OSAL_FILE_WRITE | OSAL_FILE_CREATE | OSAL_FILE_TRUNC;
	fx->args.autoclose = true;

	*state = fx;
	return 0;
}

static int teardown_stream_fact_create_fs(void **state)
{
	test_stream_fact_create_fs_fixture_t *fx =
		(test_stream_fact_create_fs_fixture_t *)(*state);

	if (fx->out) {
		stream_destroy(&fx->out);
		fx->out = NULL;
	}

	stream_destroy_factory(&fx->factory);

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_stream_fact_create_fs(void **state)
{
	test_stream_fact_create_fs_fixture_t *fx =
		(test_stream_fact_create_fs_fixture_t *)(*state);
	const test_stream_fact_create_fs_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t st = STREAM_STATUS_INVALID;
	stream_status_t ret = STREAM_STATUS_INVALID;

	const stream_factory_t *factory_arg = fx->factory;
	const fs_stream_args_t *args_arg = &fx->args;
	stream_key_t key_arg = fx->desc.key;
	stream_t **out_arg = &fx->out;

	// invalid args
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_ARGS_NULL) args_arg = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_PATH_NULL) fx->args.path = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_PATH_EMPTY) fx->args.path = "";
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_FLAGS_ZERO) fx->args.flags = (uint32_t)0;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_FACTORY_NULL) factory_arg = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_KEY_NULL) key_arg = NULL;
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_KEY_EMPTY) key_arg = "";
	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_KEY_UNKNOWN) key_arg = "unknown_key";

    // ensure OUT_EXPECT_UNCHANGED is meaningful
    if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
        fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu; // sentinel
    }

    stream_t *out_arg_snapshot = fx->out;

	// ACT
	ret = stream_factory_create_stream(factory_arg, key_arg, args_arg, out_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);

	switch (tc->out_expect) {
		case OUT_CHECK_NONE: break;
		case OUT_EXPECT_NULL: assert_null(fx->out); break;
		case OUT_EXPECT_NON_NULL: assert_non_null(fx->out); break;
		case OUT_EXPECT_UNCHANGED:
			assert_ptr_equal(out_arg_snapshot, fx->out);
			fx->out = NULL; // prevent teardown from destroying sentinel
			break;
		default: fail();
	}

	if (tc->scenario == STREAM_FACT_CREATE_FS_SCENARIO_OK) {
		assert_non_null(fx->out);
		const char msg[] = "hello";
		size_t w = stream_write(fx->out, msg, sizeof(msg) - 1, &st);
		assert_int_equal((int)w, (int)(sizeof(msg) - 1));
		assert_int_equal(st, STREAM_STATUS_OK);
		assert_int_equal(fake_file_backing_len(), sizeof(msg) - 1);
		assert_memory_equal(fx->backing, msg, sizeof(msg) - 1);
		assert_int_equal(stream_flush(fx->out), STREAM_STATUS_OK);
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

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_FLAGS_ZERO = {
	.name = "fs_stream_fact_create_stream_flags_zero",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_FLAGS_ZERO,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_create_fs_case_t CASE_STREAM_FACT_CREATE_FS_OPEN_FAIL = {
	.name = "fs_stream_fact_create_stream_open_fail",
	.scenario = STREAM_FACT_CREATE_FS_SCENARIO_OPEN_FAIL,

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

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STREAM_FACT_CREATE_FS_CASES(X) \
X(CASE_STREAM_FACT_CREATE_FS_OK) \
X(CASE_STREAM_FACT_CREATE_FS_ARGS_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_OUT_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_PATH_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_PATH_EMPTY) \
X(CASE_STREAM_FACT_CREATE_FS_FLAGS_ZERO) \
X(CASE_STREAM_FACT_CREATE_FS_OPEN_FAIL) \
X(CASE_STREAM_FACT_CREATE_FS_FACTORY_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_KEY_NULL) \
X(CASE_STREAM_FACT_CREATE_FS_KEY_EMPTY) \
X(CASE_STREAM_FACT_CREATE_FS_KEY_UNKNOWN)

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
