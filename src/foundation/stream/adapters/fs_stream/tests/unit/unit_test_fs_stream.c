/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_fs_stream.c
 * @ingroup fs_stream_unit_tests
 * @brief Unit tests implementation for the `fs_stream` adapter.
 *
 * @details
 * This file implements the unit-level validation of the `fs_stream` adapter
 * contracts.
 *
 * Covered surfaces:
 * - CR helpers: `fs_stream_default_cfg()`, `fs_stream_default_env()`
 * - direct creation: `fs_stream_create_stream()`
 * - CR descriptor helper: `fs_stream_create_desc()`
 * - descriptor constructor usage through `stream_adapter_desc_t::ctor`
 * - file-backed runtime behavior through the public `stream` API
 *
 * Test strategy:
 * - direct validation of default CR helper return values
 * - parametric scenario-based testing
 * - explicit validation of argument checking and output-handle preservation
 * - allocator fault injection through `fake_memory`
 * - backend file fault injection through `fake_file`
 * - public `stream` borrower API checks on successfully created streams
 *
 * See also:
 * - @ref testing_foundation_fs_stream_unit "fs_stream unit tests page"
 * - @ref specifications_fs_stream "fs_stream specifications"
 */

#include "fs_stream/cr/fs_stream_cr_api.h"

#include "stream/borrowers/stream.h"
#include "stream/lifecycle/stream_lifecycle.h"

#include "osal/file/test/osal_file_fake_provider.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**
 * @brief Test `fs_stream_default_cfg()`.
 *
 * fs_stream_cfg_t fs_stream_default_cfg(void);
 *
 * Success:
 * - Returns a value-initialized `fs_stream_cfg_t`.
 * - `ret.reserved == 0`.
 *
 * Failure:
 * - None.
 *
 * See also:
 * - @ref testing_foundation_fs_stream_unit_default_cfg "fs_stream_default_cfg() unit tests section"
 * - @ref specifications_fs_stream_default_cfg "fs_stream_default_cfg() specifications"
 */
static void test_fs_stream_default_cfg(void **state)
{
	(void)state;

	fs_stream_cfg_t ret = fs_stream_default_cfg();

	assert_int_equal(ret.reserved, 0);
}

/**
 * @brief Test `fs_stream_default_env()`.
 *
 * fs_stream_env_t fs_stream_default_env(
 *     const osal_file_env_t *file_env,
 *     const osal_file_ops_t *file_ops,
 *     const stream_env_t *port_env );
 *
 * Success:
 * - `ret.file_env == *file_env`.
 * - `ret.file_ops == file_ops`.
 * - `ret.port_env == *port_env`.
 *
 * Failure:
 * - None.
 *
 * Doubles:
 * - dummy `osal_file_env_t`
 * - dummy `osal_file_ops_t *`
 * - dummy `stream_env_t`
 *
 * See also:
 * - @ref testing_foundation_fs_stream_unit_default_env "fs_stream_default_env() unit tests section"
 * - @ref specifications_fs_stream_default_env "fs_stream_default_env() specifications".
 */
static void test_fs_stream_default_env(void **state) {
	(void)state;

	const osal_file_env_t dummy_file_env = {0};
	const osal_file_env_t *dummy_file_env_p = &dummy_file_env;

	const osal_file_ops_t *dummy_file_ops_p = (const osal_file_ops_t *)(uintptr_t)0x1234u;

	const stream_env_t dummy_port_env = {0};
	const stream_env_t *dummy_port_env_p = &dummy_port_env;

	fs_stream_env_t ret =
		fs_stream_default_env(dummy_file_env_p, dummy_file_ops_p, dummy_port_env_p);

	assert_memory_equal(&ret.file_env, dummy_file_env_p, sizeof(ret.file_env));
	assert_ptr_equal(ret.file_ops, dummy_file_ops_p);
	assert_memory_equal(&ret.port_env, dummy_port_env_p, sizeof(ret.port_env));
}

/**
 * @brief Scenarios for `fs_stream_create_stream()`.
 *
 * stream_status_t fs_stream_create_stream(
 *     stream_t **out,
 *     const fs_stream_args_t *args,
 *     const fs_stream_cfg_t *cfg,
 *     const fs_stream_env_t *env );
 *
 * Invalid arguments:
 * - `out`, `args`, `cfg`, `env` must not be NULL.
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
 *     - STREAM_STATUS_OOM on allocation failure
 *     - STREAM_STATUS_IO_ERROR when OSAL file operations fail
 * - Leaves `*out` unchanged if `out` is not NULL.
 *
 * Doubles:
 * - fake_memory
 * - fake_file
 *
 * See also:
 * - @ref testing_foundation_fs_stream_unit_create_stream "fs_stream_create_stream() unit tests section"
 * - @ref specifications_fs_stream_create_stream "fs_stream_create_stream() specifications"
 *
 * The scenarios below define the test oracle for `fs_stream_create_stream()`.
 */
typedef enum {
	/**
	 * WHEN `fs_stream_create_stream(out, args, cfg, env)` is called with valid
	 * arguments and OSAL file opening succeeds
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - stores a non-NULL stream handle in `*out`
	 * - the produced stream is ready for normal runtime use
	 */
	FS_CREATE_STREAM_SCENARIO_OK = 0,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - no stream handle is produced
	 */
	FS_CREATE_STREAM_SCENARIO_OUT_NULL,

	/**
	 * WHEN `args == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_CREATE_STREAM_SCENARIO_ARGS_NULL,

	/**
	 * WHEN `cfg == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_CREATE_STREAM_SCENARIO_CFG_NULL,

	/**
	 * WHEN `env == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_CREATE_STREAM_SCENARIO_ENV_NULL,

	/**
	 * WHEN `args != NULL` but `args->path == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_CREATE_STREAM_SCENARIO_PATH_NULL,

	/**
	 * WHEN `args != NULL` but `args->path` is an empty string
	 * AND `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_CREATE_STREAM_SCENARIO_PATH_EMPTY,

	/**
	 * WHEN `args != NULL` but `args->flags == 0`
	 * AND `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_CREATE_STREAM_SCENARIO_FLAGS_ZERO,

	/**
	 * WHEN allocation required by `fs_stream_create_stream()` fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_OOM`
	 * - leaves `*out` unchanged
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during stream creation.
	 */
	FS_CREATE_STREAM_SCENARIO_OOM,

	/**
	 * WHEN `fs_stream_create_stream(out, args, cfg, env)` is called with valid
	 * arguments but OSAL file opening fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_IO_ERROR`
	 * - leaves `*out` unchanged
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_file` to fail the
	 *   open operation.
	 */
	FS_CREATE_STREAM_SCENARIO_OPEN_FAIL,
} fs_create_stream_scenario_t;

/** @cond INTERNAL */

/**
 * @brief Expected state of the output handle after the call under test.
 *
 * @details
 * This helper enum is used by parametric tests to express the expected
 * postcondition on an output pointer managed by the test fixture.
 *
 * Notes:
 * - `OUT_CHECK_NONE` means no postcondition is asserted on the output handle.
 * - `OUT_EXPECT_NULL` means the output handle is expected to be `NULL`
 *   after the call.
 * - `OUT_EXPECT_NON_NULL` means the output handle is expected to be
 *   non-`NULL` after the call.
 * - `OUT_EXPECT_UNCHANGED` means the output handle is expected to preserve
 *   its pre-call value, typically verified with a sentinel pointer.
 */
typedef enum {
    OUT_CHECK_NONE,
    OUT_EXPECT_NULL,
    OUT_EXPECT_NON_NULL,
    OUT_EXPECT_UNCHANGED
} out_expect_t;

/**
 * @brief One parametric test case for `fs_stream_create_stream()`.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 * - `open_fail_status` is used by `fake_file` to inject the OSAL status
 *   returned by the failing open operation.
 */
typedef struct {
	const char *name;

	// arrange
	fs_create_stream_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based (scenario == FS_CREATE_STREAM_SCENARIO_OOM)
	osal_file_status_t open_fail_status; // e.g. OSAL_FILE_NOENT

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_fs_stream_create_stream_case_t;

/**
 * @brief Runtime fixture for `fs_stream_create_stream()` tests.
 *
 * Holds:
 * - the stream handle under test,
 * - the injected adapter environment,
 * - the adapter arguments and configuration,
 * - the fake file backing storage,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_t *out;

	// injection
	fs_stream_env_t env;

	fs_stream_args_t args;
	fs_stream_cfg_t cfg;

	// fake file backing
	uint8_t backing[64];

	const test_fs_stream_create_stream_case_t *tc;
} test_fs_stream_create_stream_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `fs_stream_create_stream()` tests.
 */
static int setup_fs_stream_create_stream(void **state)
{
	const test_fs_stream_create_stream_case_t *tc =
		(const test_fs_stream_create_stream_case_t *)(*state);

	test_fs_stream_create_stream_fixture_t *fx =
		(test_fs_stream_create_stream_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	fake_memory_reset();
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	osal_memset(fx->backing, 0, sizeof(fx->backing));
	fake_file_set_backing(fx->backing, sizeof(fx->backing), 0);
	fake_file_set_pos(0);
	fake_file_fail_disable();

	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OPEN_FAIL) {
		fake_file_fail_enable(FAKE_FILE_OP_OPEN, 1, tc->open_fail_status);
	}

	// DI
	fx->env.file_env.mem = osal_mem_test_fake_ops();
	fx->env.file_ops = osal_file_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	fx->args.path = "crazy_injection.txt";
	fx->args.flags = OSAL_FILE_READ | OSAL_FILE_WRITE | OSAL_FILE_CREATE | OSAL_FILE_TRUNC;
	fx->args.autoclose = true;

	fx->cfg.reserved = 0; /* Reserved for future use. */

	*state = fx;
	return 0;
}

/**
 * @brief Release the `fs_stream_create_stream()` test fixture and verify memory invariants.
 */
static int teardown_fs_stream_create_stream(void **state)
{
	test_fs_stream_create_stream_fixture_t *fx =
		(test_fs_stream_create_stream_fixture_t *)(*state);

	if (fx->out) {
		stream_destroy(&fx->out);
		fx->out = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `fs_stream_create_stream()`.
 */
static void test_fs_stream_create_stream(void **state)
{
	test_fs_stream_create_stream_fixture_t *fx =
		(test_fs_stream_create_stream_fixture_t *)(*state);
	const test_fs_stream_create_stream_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t st = STREAM_STATUS_INVALID;
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_t **out_arg = &fx->out;
	const fs_stream_args_t *args_arg = &fx->args;
	const fs_stream_cfg_t *cfg_arg = &fx->cfg;
	const fs_stream_env_t *env_arg = &fx->env;

	// invalid args
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_ARGS_NULL) args_arg = NULL;
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_CFG_NULL) cfg_arg = NULL;
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_ENV_NULL) env_arg = NULL;
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_PATH_NULL) fx->args.path = NULL;
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_PATH_EMPTY) fx->args.path = "";
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_FLAGS_ZERO) fx->args.flags = (uint32_t)0;

    // ensure OUT_EXPECT_UNCHANGED is meaningful
    if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
        fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu; // sentinel
    }

    stream_t *out_arg_snapshot = fx->out;

	// ACT
	ret = fs_stream_create_stream(out_arg, args_arg, cfg_arg, env_arg);

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
		default: assert_true(false);
	}

	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OK) {
		assert_non_null(fx->out);
		const char msg[] = "hello";
		size_t w = stream_write(fx->out, msg, sizeof(msg) - 1, &st);
		assert_int_equal((int)w, (int)(sizeof(msg) - 1));
		assert_true(st == STREAM_STATUS_OK);
		assert_int_equal(fake_file_backing_len(), sizeof(msg) - 1);
		assert_memory_equal(fx->backing, msg, sizeof(msg) - 1);
		assert_int_equal(stream_flush(fx->out), STREAM_STATUS_OK);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_fs_stream_create_stream_case_t CASE_FS_OK = {
	.name = "fs_create_stream_ok",
	.scenario = FS_CREATE_STREAM_SCENARIO_OK,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

static const test_fs_stream_create_stream_case_t CASE_FS_OUT_NULL = {
	.name = "fs_create_stream_out_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_fs_stream_create_stream_case_t CASE_FS_ARGS_NULL = {
	.name = "fs_create_stream_args_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_ARGS_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_CFG_NULL = {
	.name = "fs_create_stream_cfg_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_CFG_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_ENV_NULL = {
	.name = "fs_create_stream_env_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_ENV_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_PATH_NULL = {
	.name = "fs_create_stream_path_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_PATH_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_PATH_EMPTY = {
	.name = "fs_create_stream_path_empty",
	.scenario = FS_CREATE_STREAM_SCENARIO_PATH_EMPTY,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_FLAGS_ZERO = {
	.name = "fs_create_stream_flags_zero",
	.scenario = FS_CREATE_STREAM_SCENARIO_FLAGS_ZERO,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_OOM_1 = {
	.name = "fs_create_stream_oom_1",
	.scenario = FS_CREATE_STREAM_SCENARIO_OOM,
	.fail_call_idx = 1,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_OPEN_FAIL_NOT_FOUND = {
	.name = "fs_create_stream_open_fail_not_found",
	.scenario = FS_CREATE_STREAM_SCENARIO_OPEN_FAIL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_NOENT,

	.expected_ret = STREAM_STATUS_IO_ERROR,
	.out_expect = OUT_EXPECT_UNCHANGED
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_CREATE_STREAM_CASES(X) \
X(CASE_FS_OK) \
X(CASE_FS_OUT_NULL) \
X(CASE_FS_ARGS_NULL) \
X(CASE_FS_PATH_NULL) \
X(CASE_FS_PATH_EMPTY) \
X(CASE_FS_FLAGS_ZERO) \
X(CASE_FS_CFG_NULL) \
X(CASE_FS_ENV_NULL) \
X(CASE_FS_OOM_1) \
X(CASE_FS_OPEN_FAIL_NOT_FOUND)

#define FS_STREAM_MAKE_CREATE_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_create_stream, case_sym)

static const struct CMUnitTest fs_stream_stream_create_stream_tests[] = {
	FS_STREAM_CREATE_STREAM_CASES(FS_STREAM_MAKE_CREATE_STREAM_TEST)
};

#undef FS_STREAM_CREATE_STREAM_CASES
#undef FS_STREAM_MAKE_CREATE_STREAM_TEST

/** @endcond */

/**
 * @brief Scenarios for `fs_stream_create_desc()`.
 *
 * stream_status_t fs_stream_create_desc(
 *     stream_adapter_desc_t *out,
 *     stream_key_t key,
 *     const fs_stream_cfg_t *cfg,
 *     const fs_stream_env_t *env,
 *     const osal_mem_ops_t *mem );
 *
 * Invalid arguments:
 * - `out`, `key`, `cfg`, `env`, `mem` must not be NULL.
 * - `key` must not be an empty string.
 *
 * Success:
 * - Returns STREAM_STATUS_OK.
 * - Stores a valid adapter descriptor in `*out`.
 * - The produced descriptor must later be released via `out->ud_dtor()`.
 *
 * Failure:
 * - Returns:
 *     - STREAM_STATUS_INVALID for invalid arguments
 *     - STREAM_STATUS_OOM on allocation failure
 * - If `out` is not NULL, resets `*out` to an empty descriptor.
 *
 * Doubles:
 * - fake_memory
 *
 * See also:
 * - @ref testing_foundation_fs_stream_unit_create_desc "fs_stream_create_desc() unit tests section"
 * - @ref specifications_fs_stream_create_desc "fs_stream_create_desc() specifications"
 *
 * The scenarios below define the test oracle for `fs_stream_create_desc()`.
 */
typedef enum {
	/**
	 * WHEN `fs_stream_create_desc(out, key, cfg, env, mem)` is called with
	 * valid arguments
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - stores a valid adapter descriptor in `*out`
	 * - the produced descriptor is eligible for later destruction via
	 *   `out->ud_dtor()`
	 */
	FS_STREAM_CREATE_DESC_SCENARIO_OK = 0,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - no descriptor is produced
	 */
	FS_STREAM_CREATE_DESC_SCENARIO_OUT_NULL,

	/**
	 * WHEN `key == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	FS_STREAM_CREATE_DESC_SCENARIO_KEY_NULL,

	/**
	 * WHEN `key` is an empty string and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	FS_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY,

	/**
	 * WHEN `cfg == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	FS_STREAM_CREATE_DESC_SCENARIO_CFG_NULL,

	/**
	 * WHEN `env == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	FS_STREAM_CREATE_DESC_SCENARIO_ENV_NULL,

	/**
	 * WHEN `mem == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	FS_STREAM_CREATE_DESC_SCENARIO_MEM_NULL,

	/**
	 * WHEN allocation required by `fs_stream_create_desc()` fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_OOM`
	 * - resets `*out` to an empty descriptor
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during descriptor construction.
	 */
	FS_STREAM_CREATE_DESC_SCENARIO_OOM
} fs_stream_create_desc_scenario_t;

/** @cond INTERNAL */

/**
 * @brief Expected state of the descriptor output after the call under test.
 */
typedef enum {
	DESC_CHECK_NONE,
    DESC_EXPECT_EMPTY,
    DESC_EXPECT_VALID
} desc_expect_t;

/**
 * @brief One parametric test case for `fs_stream_create_desc()`.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 */
typedef struct {
	const char *name;

	// arrange
	fs_stream_create_desc_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based (scenario == FS_STREAM_CREATE_DESC_SCENARIO_OOM)

	// assert
	stream_status_t expected_ret;
	desc_expect_t desc_expect;
} test_fs_stream_create_desc_case_t;

/**
 * @brief Runtime fixture for `fs_stream_create_desc()` tests.
 *
 * Holds:
 * - the descriptor output under test,
 * - the injected adapter environment,
 * - the injected descriptor allocator,
 * - the adapter key and configuration,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_adapter_desc_t out;

	// injection
	fs_stream_env_t env;
	const osal_mem_ops_t *mem;

	stream_key_t key;
	fs_stream_cfg_t cfg;

	const test_fs_stream_create_desc_case_t *tc;
} test_fs_stream_create_desc_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `fs_stream_create_desc()` tests.
 */
static int setup_fs_stream_create_desc(void **state)
{
	const test_fs_stream_create_desc_case_t *tc =
		(const test_fs_stream_create_desc_case_t *)(*state);

	test_fs_stream_create_desc_fixture_t *fx =
		(test_fs_stream_create_desc_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// DI
	fx->env.file_env.mem = osal_mem_test_fake_ops();
	fx->env.file_ops = osal_file_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();
	fx->mem = osal_mem_test_fake_ops();

	fx->key = "fs";

	fx->cfg.reserved = 0; /* Reserved for future use. */

	*state = fx;
	return 0;
}

/**
 * @brief Release the `fs_stream_create_desc()` test fixture and verify memory invariants.
 */
static int teardown_fs_stream_create_desc(void **state)
{
	test_fs_stream_create_desc_fixture_t *fx =
		(test_fs_stream_create_desc_fixture_t *)(*state);

	if (fx->out.ud_dtor) fx->out.ud_dtor(fx->out.ud, fx->mem);

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `fs_stream_create_desc()`.
 */
static void test_fs_stream_create_desc(void **state)
{
	test_fs_stream_create_desc_fixture_t *fx =
		(test_fs_stream_create_desc_fixture_t *)(*state);
	const test_fs_stream_create_desc_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_adapter_desc_t *out_arg = &fx->out;
	stream_key_t key_arg = fx->key;
	const fs_stream_cfg_t *cfg_arg = &fx->cfg;
	const fs_stream_env_t *env_arg = &fx->env;
	const osal_mem_ops_t *mem_arg = fx->mem;

	// invalid args
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_KEY_NULL) key_arg = NULL;
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY) key_arg = "";
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_CFG_NULL) cfg_arg = NULL;
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_ENV_NULL) env_arg = NULL;
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_MEM_NULL) mem_arg = NULL;

    if (tc->desc_expect == DESC_EXPECT_EMPTY && out_arg != NULL) {
        fx->out.key = (stream_key_t)(uintptr_t)0xDEADC0DEu;
		fx->out.ctor = (void *)(uintptr_t)0xDEADC0DEu;
		fx->out.ud = (void *)(uintptr_t)0xDEADC0DEu;
		fx->out.ud_dtor = (void *)(uintptr_t)0xDEADC0DEu;
    }

	// ACT
	ret = fs_stream_create_desc(out_arg, key_arg, cfg_arg, env_arg, mem_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);

	if (tc->desc_expect == DESC_EXPECT_EMPTY) {
	    assert_null(fx->out.key);
    	assert_null(fx->out.ctor);
    	assert_null(fx->out.ud);
    	assert_null(fx->out.ud_dtor);
	}
	else if (tc->desc_expect == DESC_EXPECT_VALID) {
		assert_true(fx->out.key != (stream_key_t)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.key);
		assert_true(*fx->out.key != '\0');
		assert_true(fx->out.ctor != (void *)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.ctor);
    	assert_true(fx->out.ud != (void *)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.ud);
		assert_true(fx->out.ud_dtor != (void *)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.ud_dtor);
	}
	else {
		assert_true(tc->desc_expect == DESC_CHECK_NONE);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_OK = {
	.name = "fs_stream_create_desc_ok",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_OK,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_OK,
	.desc_expect = DESC_EXPECT_VALID
};

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_OUT_NULL = {
	.name = "fs_stream_create_desc_out_null",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_CHECK_NONE
};

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_KEY_NULL = {
	.name = "fs_stream_create_desc_key_null",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_KEY_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_KEY_EMPTY = {
	.name = "fs_stream_create_desc_key_empty",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_CFG_NULL = {
	.name = "fs_stream_create_desc_cfg_null",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_CFG_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_ENV_NULL = {
	.name = "fs_stream_create_desc_env_null",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_ENV_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_MEM_NULL = {
	.name = "fs_stream_create_desc_mem_null",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_MEM_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_OOM_1 = {
	.name = "fs_stream_create_desc_oom_1",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_OOM,
	.fail_call_idx = 1,

	.expected_ret = STREAM_STATUS_OOM,
	.desc_expect = DESC_EXPECT_EMPTY
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_CREATE_DESC_CASES(X) \
X(CASE_FS_STREAM_CREATE_DESC_OK) \
X(CASE_FS_STREAM_CREATE_DESC_OUT_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_KEY_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_KEY_EMPTY) \
X(CASE_FS_STREAM_CREATE_DESC_CFG_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_ENV_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_MEM_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_OOM_1)

#define FS_STREAM_MAKE_CREATE_DESC_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_create_desc, case_sym)

static const struct CMUnitTest create_desc_fs_stream_tests[] = {
	FS_STREAM_CREATE_DESC_CASES(FS_STREAM_MAKE_CREATE_DESC_TEST)
};

#undef FS_STREAM_CREATE_DESC_CASES
#undef FS_STREAM_MAKE_CREATE_DESC_TEST

/**
 * @brief Scenarios for `fs_stream_create_desc()` descriptor constructor usage.
 *
 * stream_status_t desc.ctor(
 *     const void *ud,
 *     const fs_stream_args_t *args,
 *     stream_t **out );
 *
 * Invalid arguments:
 * - `args` and `out` must not be NULL.
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
 *     - STREAM_STATUS_OOM on allocation failure
 *     - STREAM_STATUS_IO_ERROR when OSAL file operations fail
 * - Leaves `*out` unchanged if `out` is not NULL.
 *
 * Doubles:
 * - fake_memory
 * - fake_file
 *
 * See also:
 * - @ref testing_foundation_fs_stream_unit_desc_ctor "fs_stream descriptor constructor unit tests section"
 * - @ref specifications_fs_stream_create_desc "fs_stream_create_desc() specifications"
 *
 * The scenarios below define the test oracle for descriptor constructor usage
 * through `fs_stream_create_desc()`.
 */
typedef enum {
	/**
	 * WHEN `desc.ctor(ud, args, out)` is called with valid arguments and OSAL
	 * file opening succeeds
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - stores a non-NULL stream handle in `*out`
	 * - the produced stream is ready for normal runtime use
	 */
	FS_STREAM_DESC_CTOR_SCENARIO_OK = 0,

	/**
	 * WHEN `args == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_STREAM_DESC_CTOR_SCENARIO_ARGS_NULL,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - no stream handle is produced
	 */
	FS_STREAM_DESC_CTOR_SCENARIO_OUT_NULL,

	/**
	 * WHEN `args != NULL` but `args->path == NULL`
	 * AND `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_STREAM_DESC_CTOR_SCENARIO_PATH_NULL,

	/**
	 * WHEN `args != NULL` but `args->path` is an empty string
	 * AND `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_STREAM_DESC_CTOR_SCENARIO_PATH_EMPTY,

	/**
	 * WHEN `args != NULL` but `args->flags == 0`
	 * AND `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_STREAM_DESC_CTOR_SCENARIO_FLAGS_ZERO,

	/**
	 * WHEN allocation required by `desc.ctor()` fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_OOM`
	 * - leaves `*out` unchanged
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during stream creation.
	 */
	FS_STREAM_DESC_CTOR_SCENARIO_OOM,

	/**
	 * WHEN `desc.ctor(ud, args, out)` is called with valid arguments but OSAL
	 * file opening fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_IO_ERROR`
	 * - leaves `*out` unchanged
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_file` to fail the
	 *   open operation.
	 */
	FS_STREAM_DESC_CTOR_SCENARIO_OPEN_FAIL,
} fs_stream_desc_ctor_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for descriptor constructor usage through
 * `fs_stream_create_desc()`.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 * - `open_fail_status` is used by `fake_file` to inject the OSAL status
 *   returned by the failing open operation.
 */
typedef struct {
	const char *name;

	// arrange
	fs_stream_desc_ctor_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based
	osal_file_status_t open_fail_status; // e.g. OSAL_FILE_NOENT

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_fs_stream_desc_ctor_case_t;

/**
 * @brief Runtime fixture for descriptor constructor usage tests through
 * `fs_stream_create_desc()`.
 *
 * Holds:
 * - the descriptor under test,
 * - the stream handle produced by `desc.ctor()`,
 * - the injected descriptor dependencies,
 * - the constructor call arguments,
 * - the fake file backing storage,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// descriptor under test
	stream_adapter_desc_t desc;

	// runtime resource produced by desc.ctor()
	stream_t *out;

	// dependencies / descriptor inputs
	fs_stream_env_t env;
	const osal_mem_ops_t *mem;
	fs_stream_cfg_t cfg;

	// ctor call inputs
	fs_stream_args_t args;

	// fake file backing
	uint8_t backing[64];

	// reference to test case
	const test_fs_stream_desc_ctor_case_t *tc;
} test_fs_stream_desc_ctor_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for descriptor
 * constructor usage tests through `fs_stream_create_desc()`.
 */
static int setup_fs_stream_desc_ctor(void **state)
{
	const test_fs_stream_desc_ctor_case_t *tc =
		(const test_fs_stream_desc_ctor_case_t *)(*state);

	test_fs_stream_desc_ctor_fixture_t *fx =
		(test_fs_stream_desc_ctor_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	fake_memory_reset();

	osal_memset(fx->backing, 0, sizeof(fx->backing));
	fake_file_set_backing(fx->backing, sizeof(fx->backing), 0);
	fake_file_set_pos(0);
	fake_file_fail_disable();

	if (tc->scenario == FS_STREAM_DESC_CTOR_SCENARIO_OPEN_FAIL) {
		fake_file_fail_enable(FAKE_FILE_OP_OPEN, 1, tc->open_fail_status);
	}

	// DI
	fx->env.file_env.mem = osal_mem_test_fake_ops();
	fx->env.file_ops = osal_file_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();
	fx->mem = osal_mem_test_fake_ops();

	fx->cfg.reserved = 0; /* Reserved for future use. */

	stream_status_t st = fs_stream_create_desc(&fx->desc, "fs", &fx->cfg, &fx->env, fx->mem);
	assert_true(st == STREAM_STATUS_OK);

	if (tc->scenario == FS_STREAM_DESC_CTOR_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	fx->args.path = "crazy_injection.txt";
	fx->args.flags = OSAL_FILE_READ | OSAL_FILE_WRITE | OSAL_FILE_CREATE | OSAL_FILE_TRUNC;
	fx->args.autoclose = true;

	*state = fx;
	return 0;
}

/**
 * @brief Release the descriptor constructor usage test fixture through
 * `fs_stream_create_desc()` and verify memory invariants.
 */
static int teardown_fs_stream_desc_ctor(void **state)
{
	test_fs_stream_desc_ctor_fixture_t *fx =
		(test_fs_stream_desc_ctor_fixture_t *)(*state);

	if (fx->out) {
		stream_destroy(&fx->out);
		fx->out = NULL;
	}

	if (fx->desc.ud_dtor) fx->desc.ud_dtor(fx->desc.ud, fx->mem);

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for descriptor constructor
 * usage through `fs_stream_create_desc()`.
 */
static void test_fs_stream_desc_ctor(void **state)
{
	test_fs_stream_desc_ctor_fixture_t *fx =
		(test_fs_stream_desc_ctor_fixture_t *)(*state);
	const test_fs_stream_desc_ctor_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t st = STREAM_STATUS_INVALID;
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_t **out_arg = &fx->out;
	const fs_stream_args_t *args_arg = &fx->args;

	// invalid args
	if (tc->scenario == FS_STREAM_DESC_CTOR_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == FS_STREAM_DESC_CTOR_SCENARIO_ARGS_NULL) args_arg = NULL;
	if (tc->scenario == FS_STREAM_DESC_CTOR_SCENARIO_PATH_NULL) fx->args.path = NULL;
	if (tc->scenario == FS_STREAM_DESC_CTOR_SCENARIO_PATH_EMPTY) fx->args.path = "";
	if (tc->scenario == FS_STREAM_DESC_CTOR_SCENARIO_FLAGS_ZERO) fx->args.flags = (uint32_t)0;

    // ensure OUT_EXPECT_UNCHANGED is meaningful
    if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
        fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu; // sentinel
    }

    stream_t *out_arg_snapshot = fx->out;

	// ACT
	ret = fx->desc.ctor(fx->desc.ud, args_arg, out_arg);

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
		default: assert_true(false);
	}

	if (tc->scenario == FS_STREAM_DESC_CTOR_SCENARIO_OK) {
		assert_non_null(fx->out);
		const char msg[] = "hello";
		size_t w = stream_write(fx->out, msg, sizeof(msg) - 1, &st);
		assert_int_equal((int)w, (int)(sizeof(msg) - 1));
		assert_true(st == STREAM_STATUS_OK);
		assert_int_equal(fake_file_backing_len(), sizeof(msg) - 1);
		assert_memory_equal(fx->backing, msg, sizeof(msg) - 1);
		assert_int_equal(stream_flush(fx->out), STREAM_STATUS_OK);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_fs_stream_desc_ctor_case_t CASE_FS_STREAM_DESC_CTOR_OK = {
	.name = "fs_stream_desc_ctor_create_stream_ok",
	.scenario = FS_STREAM_DESC_CTOR_SCENARIO_OK,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

static const test_fs_stream_desc_ctor_case_t CASE_FS_STREAM_DESC_CTOR_ARGS_NULL = {
	.name = "fs_stream_desc_ctor_create_stream_args_null",
	.scenario = FS_STREAM_DESC_CTOR_SCENARIO_ARGS_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_desc_ctor_case_t CASE_FS_STREAM_DESC_CTOR_OUT_NULL = {
	.name = "fs_stream_desc_ctor_create_stream_out_null",
	.scenario = FS_STREAM_DESC_CTOR_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_fs_stream_desc_ctor_case_t CASE_FS_STREAM_DESC_CTOR_PATH_NULL = {
	.name = "fs_stream_desc_ctor_create_stream_path_null",
	.scenario = FS_STREAM_DESC_CTOR_SCENARIO_PATH_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_desc_ctor_case_t CASE_FS_STREAM_DESC_CTOR_PATH_EMPTY = {
	.name = "fs_stream_desc_ctor_create_stream_path_empty",
	.scenario = FS_STREAM_DESC_CTOR_SCENARIO_PATH_EMPTY,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_desc_ctor_case_t CASE_FS_STREAM_DESC_CTOR_FLAGS_ZERO = {
	.name = "fs_stream_desc_ctor_create_stream_flags_zero",
	.scenario = FS_STREAM_DESC_CTOR_SCENARIO_FLAGS_ZERO,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_desc_ctor_case_t CASE_FS_STREAM_DESC_CTOR_OOM_1 = {
	.name = "fs_stream_desc_ctor_create_stream_oom_1",
	.scenario = FS_STREAM_DESC_CTOR_SCENARIO_OOM,
	.fail_call_idx = 1,
	.open_fail_status = OSAL_FILE_OK,

	.expected_ret = STREAM_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_desc_ctor_case_t CASE_FS_STREAM_DESC_CTOR_OPEN_FAIL = {
	.name = "fs_stream_desc_ctor_create_stream_open_fail",
	.scenario = FS_STREAM_DESC_CTOR_SCENARIO_OPEN_FAIL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_NOENT,

	.expected_ret = STREAM_STATUS_IO_ERROR,
	.out_expect = OUT_EXPECT_UNCHANGED
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_DESC_CTOR_CASES(X) \
X(CASE_FS_STREAM_DESC_CTOR_OK) \
X(CASE_FS_STREAM_DESC_CTOR_ARGS_NULL) \
X(CASE_FS_STREAM_DESC_CTOR_OUT_NULL) \
X(CASE_FS_STREAM_DESC_CTOR_PATH_NULL) \
X(CASE_FS_STREAM_DESC_CTOR_PATH_EMPTY) \
X(CASE_FS_STREAM_DESC_CTOR_FLAGS_ZERO) \
X(CASE_FS_STREAM_DESC_CTOR_OOM_1) \
X(CASE_FS_STREAM_DESC_CTOR_OPEN_FAIL)

#define FS_STREAM_MAKE_DESC_CTOR_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_desc_ctor, case_sym)

static const struct CMUnitTest desc_ctor_fs_stream_tests[] = {
	FS_STREAM_DESC_CTOR_CASES(FS_STREAM_MAKE_DESC_CTOR_TEST)
};

#undef FS_STREAM_DESC_CTOR_CASES
#undef FS_STREAM_MAKE_DESC_CTOR_TEST

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	static const struct CMUnitTest fs_stream_tests[] = {
		cmocka_unit_test(test_fs_stream_default_cfg),
		cmocka_unit_test(test_fs_stream_default_env),
	};

	int failed = 0;
	failed += cmocka_run_group_tests(fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(fs_stream_stream_create_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(create_desc_fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(desc_ctor_fs_stream_tests, NULL, NULL);
	return failed;
}

/** @endcond */
