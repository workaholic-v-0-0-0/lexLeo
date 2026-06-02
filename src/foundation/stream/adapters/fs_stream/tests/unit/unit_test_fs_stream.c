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
 * See also:
 * - @ref testing_foundation_fs_stream_unit "fs_stream unit tests page"
 * - @ref specifications_fs_stream "fs_stream specifications"
 */

#include "fs_stream/cr/fs_stream_cr_api.h"

#include "stream/cr/stream_cr_api.h"

#include "osal/file/test/osal_file_fake_provider.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "stream/tests/stream_white_box_tests_access.h"

#include "lexleo_cmocka.h"

/**
 * @brief Test `fs_stream_default_cfg()`.
 *
 * See contract:
 * - @ref specifications_fs_stream_default_cfg "fs_stream_default_cfg() specifications"
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_default_cfg "fs_stream_default_cfg() unit tests section"
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
 * Doubles:
 * - dummy `osal_file_ops_t *`
 * - dummy `osal_mem_ops_t *`
 * - dummy `stream_env_t`
 *
 * See contract:
 * - @ref specifications_fs_stream_default_env "fs_stream_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_default_env "fs_stream_default_env() unit tests section"
 */
static void test_fs_stream_default_env(void **state) {
	(void)state;

	const osal_file_ops_t *dummy_file_ops_p = (const osal_file_ops_t *)(uintptr_t)0x1234u;
	const osal_mem_ops_t *dummy_mem_ops_p = (const osal_mem_ops_t *)(uintptr_t)0x2345u;
	const osal_mem_ops_t dummy_port_mem_ops = {0};
	const osal_mem_ops_t *dummy_port_mem_ops_p = &dummy_port_mem_ops;

	fs_stream_env_t ret =
		fs_stream_default_env(dummy_file_ops_p, dummy_mem_ops_p, dummy_port_mem_ops_p);

	assert_ptr_equal(ret.file_ops, dummy_file_ops_p);
	assert_ptr_equal(ret.adapter_mem_ops, dummy_mem_ops_p);
	assert_memory_equal(ret.port_mem_ops, dummy_port_mem_ops_p, sizeof(ret.port_mem_ops));
}

/**
 * @brief Scenarios for `fs_stream_create_stream()`.
 *
 * Doubles:
 * - fake_memory
 * - fake_file
 *
 * See contract:
 * - @ref specifications_fs_stream_create_stream "fs_stream_create_stream() specifications"
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_create_stream "fs_stream_create_stream() unit tests section"
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
	 * WHEN `args != NULL` but `args->mode == NULL`
	 * AND `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_CREATE_STREAM_SCENARIO_MODE_NULL,

	/**
	 * WHEN `args != NULL` but `args->mode` is not supported by the
	 * `fs_stream` contract
	 * AND `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	FS_CREATE_STREAM_SCENARIO_MODE_UNSUPPORTED,

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

typedef enum {
	OUT_CHECK_NONE,
	OUT_EXPECT_NULL,
	OUT_EXPECT_NON_NULL,
	OUT_EXPECT_UNCHANGED
} out_expect_t;

typedef struct {
	const char *name;

	// arrange
	fs_create_stream_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based (scenario == FS_CREATE_STREAM_SCENARIO_OOM)
	osal_file_status_t open_fail_status; // e.g. OSAL_FILE_STATUS_NOENT

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_fs_stream_create_stream_case_t;

typedef struct {
	// runtime resources
	stream_t *out;
	OSAL_FILE *fake_file;

	// injection
	fs_stream_env_t env;

	stream_file_creator_args_t args;
	fs_stream_cfg_t cfg;

	// fake file backing
	uint8_t backing[64];

	const test_fs_stream_create_stream_case_t *tc;
} test_fs_stream_create_stream_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_fs_stream_create_stream(void **state)
{
	const test_fs_stream_create_stream_case_t *tc =
		(const test_fs_stream_create_stream_case_t *)(*state);

	test_fs_stream_create_stream_fixture_t *fx =
		(test_fs_stream_create_stream_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	const osal_mem_ops_t *real_mem = osal_mem_default_ops();
	fx->fake_file = fake_file_create_fake(real_mem);
	assert_non_null(fx->fake_file);
	fake_file_prepare_next_open_file(fx->fake_file);

	fake_memory_reset();
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OPEN_FAIL) {
		fake_file_prepare_next_open_status(tc->open_fail_status);
	}

	fx->cfg = fs_stream_default_cfg();

	// DI
	fx->env =
		fs_stream_default_env(
			osal_file_test_fake_ops(),
			osal_mem_test_fake_ops(),
			osal_mem_test_fake_ops()
		);

	fx->args.path = "crazy_injection.txt";
	fx->args.mode = "wb";

	*state = fx;
	return 0;
}

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

	fake_file_reset();

	if (fx->tc->scenario == FS_CREATE_STREAM_SCENARIO_OK) {
		fake_file_destroy_fake(fx->fake_file);
		fx->fake_file = NULL;
	}

	osal_free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_fs_stream_create_stream(void **state)
{
	test_fs_stream_create_stream_fixture_t *fx =
		(test_fs_stream_create_stream_fixture_t *)(*state);
	const test_fs_stream_create_stream_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t st = STREAM_STATUS_INVALID;
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_t **out_arg = &fx->out;
	const stream_file_creator_args_t *args_arg = &fx->args;
	const fs_stream_cfg_t *cfg_arg = &fx->cfg;
	const fs_stream_env_t *env_arg = &fx->env;

	// invalid args
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OUT_NULL) {
		out_arg = NULL;
	}
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_ARGS_NULL) {
		args_arg = NULL;
	}
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_CFG_NULL) {
		cfg_arg = NULL;
	}
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_ENV_NULL) {
		env_arg = NULL;
	}
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_PATH_NULL) {
		fx->args.path = NULL;
	}
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_PATH_EMPTY) {
		fx->args.path = "";
	}
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_MODE_NULL) {
		fx->args.mode = NULL;
	}
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_MODE_UNSUPPORTED) {
		fx->args.mode = "unsupported_mode";
	}

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
		assert_true(
			fake_file_buffered_len(fx->fake_file) == sizeof(msg) - 1
		);
		assert_memory_equal(
			fake_file_buffered_backing(fx->fake_file),
			msg,
			sizeof(msg) - 1
		);
		assert_int_equal(stream_flush(fx->out), STREAM_STATUS_OK);
		assert_true(
			fake_file_sink_len(fx->fake_file) == sizeof(msg) - 1
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

static const test_fs_stream_create_stream_case_t CASE_FS_OUT_NULL = {
	.name = "fs_create_stream_out_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_fs_stream_create_stream_case_t CASE_FS_ARGS_NULL = {
	.name = "fs_create_stream_args_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_ARGS_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_CFG_NULL = {
	.name = "fs_create_stream_cfg_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_CFG_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_ENV_NULL = {
	.name = "fs_create_stream_env_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_ENV_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_PATH_NULL = {
	.name = "fs_create_stream_path_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_PATH_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_PATH_EMPTY = {
	.name = "fs_create_stream_path_empty",
	.scenario = FS_CREATE_STREAM_SCENARIO_PATH_EMPTY,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_MODE_NULL = {
	.name = "fs_create_stream_mode_null",
	.scenario = FS_CREATE_STREAM_SCENARIO_MODE_NULL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_MODE_UNSUPPORTED = {
	.name = "fs_create_stream_mode_unsupported",
	.scenario = FS_CREATE_STREAM_SCENARIO_MODE_UNSUPPORTED,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_OOM_1 = {
	.name = "fs_create_stream_oom_1",
	.scenario = FS_CREATE_STREAM_SCENARIO_OOM,
	.fail_call_idx = 1,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_OPEN_FAIL_NOT_FOUND = {
	.name = "fs_create_stream_open_fail_not_found",
	.scenario = FS_CREATE_STREAM_SCENARIO_OPEN_FAIL,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_NOENT,

	.expected_ret = STREAM_STATUS_IO_ERROR,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_fs_stream_create_stream_case_t CASE_FS_OK = {
	.name = "fs_create_stream_ok",
	.scenario = FS_CREATE_STREAM_SCENARIO_OK,
	.fail_call_idx = 0,
	.open_fail_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_CREATE_STREAM_CASES(X) \
X(CASE_FS_OUT_NULL) \
X(CASE_FS_ARGS_NULL) \
X(CASE_FS_CFG_NULL) \
X(CASE_FS_ENV_NULL) \
X(CASE_FS_PATH_NULL) \
X(CASE_FS_PATH_EMPTY) \
X(CASE_FS_MODE_NULL) \
X(CASE_FS_MODE_UNSUPPORTED) \
X(CASE_FS_OOM_1) \
X(CASE_FS_OPEN_FAIL_NOT_FOUND) \
X(CASE_FS_OK)

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
 * Doubles:
 * - fake_memory for adapter and port allocation
 * - dummy `osal_file_ops_t *`
 *
 * See contract:
 * - @ref specifications_fs_stream_create_desc "fs_stream_create_desc() specifications"
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_create_desc "fs_stream_create_desc() unit tests section"
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

typedef enum {
	DESC_CHECK_NONE,
    DESC_EXPECT_EMPTY,
    DESC_EXPECT_VALID
} fs_stream_desc_expect_t;

typedef struct {
	const char *name;

	// arrange
	fs_stream_create_desc_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based (scenario == FS_STREAM_CREATE_DESC_SCENARIO_OOM)

	// assert
	stream_status_t expected_ret;
	fs_stream_desc_expect_t desc_expect;
} test_fs_stream_create_desc_case_t;

typedef struct {
	// runtime resources
	stream_adapter_desc_t out;

	// injection
	fs_stream_env_t env;

	stream_key_t key;
	fs_stream_cfg_t cfg;

	const test_fs_stream_create_desc_case_t *tc;
} test_fs_stream_create_desc_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_fs_stream_create_desc(void **state)
{
	const test_fs_stream_create_desc_case_t *tc =
		(const test_fs_stream_create_desc_case_t *)(*state);

	test_fs_stream_create_desc_fixture_t *fx =
		(test_fs_stream_create_desc_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// DI
	fx->env =
		fs_stream_default_env(
			(const osal_file_ops_t *)(uintptr_t)0x1234u,
			osal_mem_test_fake_ops(),
			osal_mem_test_fake_ops()
		);

	fx->cfg = fs_stream_default_cfg();

	fx->key = "fs";

	*state = fx;
	return 0;
}

static int teardown_fs_stream_create_desc(void **state)
{
	test_fs_stream_create_desc_fixture_t *fx =
		(test_fs_stream_create_desc_fixture_t *)(*state);

	if (fx->out.ud && fx->out.ud_dtor) {
		fx->out.ud_dtor(fx->out.ud, fx->env.adapter_mem_ops);
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	osal_free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

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
	const osal_mem_ops_t *mem_arg = fx->env.adapter_mem_ops;

	// invalid args
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_OUT_NULL) {
		out_arg = NULL;
	}
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_KEY_NULL) {
		key_arg = NULL;
	}
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY) {
		key_arg = "";
	}
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_CFG_NULL) {
		cfg_arg = NULL;
	}
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_ENV_NULL) {
		env_arg = NULL;
	}
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_MEM_NULL) {
		mem_arg = NULL;
	}

    if (tc->desc_expect == DESC_EXPECT_EMPTY && out_arg != NULL) {
        fx->out.key = (stream_key_t)(uintptr_t)0xDEADC0DEu;
		fx->out.ctor = (stream_ctor_fn_t)(uintptr_t)0xDEADC0DEu;
		fx->out.ud = (void *)(uintptr_t)0xDEADC0DEu;
		fx->out.ud_dtor = (ud_dtor_fn_t)(uintptr_t)0xDEADC0DEu;
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
		assert_true(fx->out.ctor != (stream_ctor_fn_t)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.ctor);
    	assert_true(fx->out.ud != (void *)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.ud);
		assert_true(fx->out.ud_dtor != (ud_dtor_fn_t)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.ud_dtor);
	}
	else {
		assert_true(tc->desc_expect == DESC_CHECK_NONE);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

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

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_OK = {
	.name = "fs_stream_create_desc_ok",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_OK,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_OK,
	.desc_expect = DESC_EXPECT_VALID
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_CREATE_DESC_CASES(X) \
X(CASE_FS_STREAM_CREATE_DESC_OUT_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_KEY_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_KEY_EMPTY) \
X(CASE_FS_STREAM_CREATE_DESC_CFG_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_ENV_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_MEM_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_OOM_1) \
X(CASE_FS_STREAM_CREATE_DESC_OK)

#define FS_STREAM_MAKE_CREATE_DESC_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_create_desc, case_sym)

static const struct CMUnitTest fs_stream_create_desc_fs_stream_tests[] = {
	FS_STREAM_CREATE_DESC_CASES(FS_STREAM_MAKE_CREATE_DESC_TEST)
};

#undef FS_STREAM_CREATE_DESC_CASES
#undef FS_STREAM_MAKE_CREATE_DESC_TEST

/** @endcond */

/** @cond INTERNAL */
#define FS_STREAM_WRITE_TEST_MSG "payload"
#define FS_STREAM_WRITE_TEST_MSG_LEN (sizeof(FS_STREAM_WRITE_TEST_MSG) - 1u)
/** @endcond */

/**
 * @brief Scenarios for `fs_stream_write()`, exercised through `stream_write()`.
 *
 * @details
 * These tests create a valid `fs_stream` instance and call
 * `stream_write(s, buf, n, st)` to exercise the private `fs_stream_write()`
 * callback.
 *
 * Unless stated otherwise:
 * - `s` is created by `fs_stream_create_stream()` in `"wb"` mode, with
 *   `fake_file` prepared as the opened OSAL file and injected through
 *   `fs_stream_env_t`;
 * - `buf != NULL` and `st != NULL`;
 * - `n == FS_STREAM_WRITE_TEST_MSG_LEN`;
 * - the backend uses `fake_file` as its injected OSAL file dependency.
 * - the fake file buffered backing is initially empty;
 *
 * The oracle combines:
 * - public return/status observation through `stream_write()`;
 * - adapter-to-OSAL interaction checks through `fake_file` spies.
 *
 * Doubles:
 * - fake_memory
 * - fake_file
 *
 * See adapter contract:
 * - @ref specifications_fs_stream_write "fs_stream_write() specifications"
 *
 * See public port contract:
 * - @ref specifications_stream_write "stream_write() specifications"
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_write "fs_stream_write() unit tests section"
 */
typedef enum {

	/**
	 * WHEN `stream_write(s, buf, n, st)` is called under the default valid
	 * preconditions
	 * AND the injected OSAL file write operation reports `OSAL_FILE_STATUS_IO`
	 * AND the injected OSAL file write operation returns `0`
	 * EXPECT:
	 * - returns `0`
	 * - sets `*st = STREAM_STATUS_IO_ERROR`
	 * - calls the injected OSAL file write operation exactly once
	 * - leaves the fake file buffered backing empty
	 */
	FS_STREAM_WRITE_SCENARIO_OSAL_FILE_STATUS_IO = 0,

	/**
	 * WHEN `stream_write(s, buf, n, NULL)` is called under the default valid
	 * preconditions
	 * AND the injected OSAL file write operation reports `OSAL_FILE_STATUS_IO`
	 * AND the injected OSAL file write operation returns `0`
	 * EXPECT:
	 * - returns `0`
	 * - calls the injected OSAL file write operation exactly once
	 * - leaves the fake file buffered backing empty
	 */
	FS_STREAM_WRITE_SCENARIO_OSAL_FILE_STATUS_IO_ST_NULL,

	/**
	 * WHEN `stream_write(s, buf, n, st)` is called with
	 * `buf == FS_STREAM_WRITE_TEST_MSG`
	 * AND `n == FS_STREAM_WRITE_TEST_MSG_LEN`
	 * AND the injected OSAL file write operation reports `OSAL_FILE_STATUS_OK`
	 * EXPECT:
	 * - returns `FS_STREAM_WRITE_TEST_MSG_LEN`
	 * - sets `*st = STREAM_STATUS_OK`
	 * - writes `FS_STREAM_WRITE_TEST_MSG` into the fake file buffered backing
	 * - the fake file buffered backing contains exactly
	 *   `FS_STREAM_WRITE_TEST_MSG`
	 */
	FS_STREAM_WRITE_SCENARIO_OK,

	/**
	 * WHEN `stream_write(s, buf, n, NULL)` is called with
	 * `buf == FS_STREAM_WRITE_TEST_MSG`
	 * AND `n == FS_STREAM_WRITE_TEST_MSG_LEN`
	 * AND the injected OSAL file write operation reports `OSAL_FILE_STATUS_OK`
	 * EXPECT:
	 * - returns `FS_STREAM_WRITE_TEST_MSG_LEN`
	 * - writes `FS_STREAM_WRITE_TEST_MSG` into the fake file buffered backing
	 * - the fake file buffered backing contains exactly
	 *   `FS_STREAM_WRITE_TEST_MSG`
	 */
	FS_STREAM_WRITE_SCENARIO_OK_ST_NULL

} fs_stream_write_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;

	// arrange
	fs_stream_write_scenario_t scenario;
	osal_file_status_t osal_file_write_status; // e.g. OSAL_FILE_STATUS_IO
	const uint8_t *buf_arg;
	size_t n_arg;
	bool st_is_null;

	// assert
	stream_status_t expected_st;
	size_t expected_ret;
	const uint8_t *expected_buffered;
	size_t expected_buffered_len;
} test_fs_stream_write_case_t;

typedef struct {
	// runtime resources
	stream_t *s_arg;
	OSAL_FILE *fake_file;

	// injection
	fs_stream_env_t env;

	const test_fs_stream_write_case_t *tc;
} test_fs_stream_write_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_fs_stream_write(void **state)
{
	const test_fs_stream_write_case_t *tc =
		(const test_fs_stream_write_case_t *)(*state);

	test_fs_stream_write_fixture_t *fx =
		(test_fs_stream_write_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	const osal_mem_ops_t *real_mem = osal_mem_default_ops();
	fx->fake_file = fake_file_create_fake(real_mem);
	assert_non_null(fx->fake_file);
	fake_file_prepare_next_open_file(fx->fake_file);
	fake_file_prepare_next_open_status(OSAL_FILE_STATUS_OK);
	const stream_file_creator_args_t args = {
		.path = "dummy_pathname",
		.mode =	"wb"
	};
	const fs_stream_cfg_t cfg = fs_stream_default_cfg();
	fake_memory_reset();
	fx->env =
		fs_stream_default_env(
			osal_file_test_fake_ops(),
			osal_mem_test_fake_ops(),
			osal_mem_test_fake_ops()
		);
	assert_int_equal(
		fs_stream_create_stream(
			&fx->s_arg,
			&args,
			&cfg,
			&fx->env
		),
		STREAM_STATUS_OK
	);

	*state = fx;
	return 0;
}

static int teardown_fs_stream_write(void **state)
{
	test_fs_stream_write_fixture_t *fx =
		(test_fs_stream_write_fixture_t *)(*state);

	if (fx->s_arg) {
		stream_destroy(&fx->s_arg);
		fx->s_arg = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	if (fx->fake_file) {
		fake_file_destroy_fake(fx->fake_file);
		fx->fake_file = NULL;
	}

	fake_file_reset();

	osal_free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_fs_stream_write(void **state) {
	test_fs_stream_write_fixture_t *fx =
		(test_fs_stream_write_fixture_t *)(*state);
	const test_fs_stream_write_case_t *tc = fx->tc;

	// ARRANGE

	size_t ret = 0;

	stream_status_t st = STREAM_STATUS_OK;
	stream_status_t *st_arg = (tc->st_is_null) ? NULL : &st;

	fake_file_set_write_status(fx->fake_file, tc->osal_file_write_status);

	// ACT
	ret =
		stream_write(
			fx->s_arg,
			tc->buf_arg,
			tc->n_arg,
			st_arg
		);

	// ASSERT

	assert_true(
		fake_file_write_call_count(fx->fake_file)
		==
		1
	);
	assert_true(
		fake_file_buffered_len(fx->fake_file)
		==
		tc->expected_buffered_len
	);

	assert_true(ret == tc->expected_ret);
	if (!tc->st_is_null) {
		assert_int_equal(*st_arg, tc->expected_st);
	}

	assert_memory_equal(
		fake_file_buffered_backing(fx->fake_file),
		tc->expected_buffered,
		tc->expected_buffered_len
	);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_fs_stream_write_case_t
CASE_FS_STREAM_WRITE_OSAL_FILE_STATUS_IO = {
	.name = "fs_stream_write_osal_file_status_io",
	.scenario = FS_STREAM_WRITE_SCENARIO_OSAL_FILE_STATUS_IO,
	.osal_file_write_status = OSAL_FILE_STATUS_IO,
	.buf_arg = (const uint8_t *)FS_STREAM_WRITE_TEST_MSG,
	.n_arg = FS_STREAM_WRITE_TEST_MSG_LEN,
	.st_is_null = false,

	.expected_st = STREAM_STATUS_IO_ERROR,
	.expected_ret = 0,
	.expected_buffered = (const uint8_t *)"",
	.expected_buffered_len = 0
};

static const test_fs_stream_write_case_t
CASE_FS_STREAM_WRITE_OSAL_FILE_STATUS_IO_ST_NULL = {
	.name = "fs_stream_write_osal_file_status_io_st_null",
	.scenario = FS_STREAM_WRITE_SCENARIO_OSAL_FILE_STATUS_IO_ST_NULL,
	.osal_file_write_status = OSAL_FILE_STATUS_IO,
	.buf_arg = (const uint8_t *)FS_STREAM_WRITE_TEST_MSG,
	.n_arg = FS_STREAM_WRITE_TEST_MSG_LEN,
	.st_is_null = true,

	.expected_ret = 0,
	.expected_buffered = (const uint8_t *)"",
	.expected_buffered_len = 0
};

static const test_fs_stream_write_case_t
CASE_FS_STREAM_WRITE_OK = {
	.name = "fs_stream_write_ok",
	.scenario = FS_STREAM_WRITE_SCENARIO_OK,
	.osal_file_write_status = OSAL_FILE_STATUS_OK,
	.buf_arg = (const uint8_t *)FS_STREAM_WRITE_TEST_MSG,
	.n_arg = FS_STREAM_WRITE_TEST_MSG_LEN,
	.st_is_null = false,

	.expected_st = STREAM_STATUS_OK,
	.expected_ret = FS_STREAM_WRITE_TEST_MSG_LEN,
	.expected_buffered = (const uint8_t *)FS_STREAM_WRITE_TEST_MSG,
	.expected_buffered_len = FS_STREAM_WRITE_TEST_MSG_LEN
};

static const test_fs_stream_write_case_t
CASE_FS_STREAM_WRITE_OK_ST_NULL = {
	.name = "fs_stream_write_ok_st_null",
	.scenario = FS_STREAM_WRITE_SCENARIO_OK_ST_NULL,
	.osal_file_write_status = OSAL_FILE_STATUS_OK,
	.buf_arg = (const uint8_t *)FS_STREAM_WRITE_TEST_MSG,
	.n_arg = FS_STREAM_WRITE_TEST_MSG_LEN,
	.st_is_null = true,

	.expected_ret = FS_STREAM_WRITE_TEST_MSG_LEN,
	.expected_buffered = (const uint8_t *)FS_STREAM_WRITE_TEST_MSG,
	.expected_buffered_len = FS_STREAM_WRITE_TEST_MSG_LEN
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_WRITE_CASES(X) \
X(CASE_FS_STREAM_WRITE_OSAL_FILE_STATUS_IO) \
X(CASE_FS_STREAM_WRITE_OSAL_FILE_STATUS_IO_ST_NULL) \
X(CASE_FS_STREAM_WRITE_OK) \
X(CASE_FS_STREAM_WRITE_OK_ST_NULL)

#define FS_STREAM_MAKE_WRITE_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_write, case_sym)

static const struct CMUnitTest fs_stream_write_fs_stream_tests[] = {
	FS_STREAM_WRITE_CASES(FS_STREAM_MAKE_WRITE_TEST)
};

#undef FS_STREAM_WRITE_CASES
#undef FS_STREAM_MAKE_WRITE_TEST

/** @endcond */

/** @cond INTERNAL */
#define FS_STREAM_READ_TEST_MSG "payload"
#define FS_STREAM_READ_TEST_MSG_LEN (sizeof(FS_STREAM_READ_TEST_MSG) - 1u)
#define FS_STREAM_READ_TEST_EOF_N (FS_STREAM_READ_TEST_MSG_LEN + 1u)
/** @endcond */

/**
 * @brief Scenarios for `fs_stream_read()`, exercised through `stream_read()`.
 *
 * @details
 * These tests create a valid `fs_stream` instance and call
 * `stream_read(s, buf, n, st)` to exercise the private `fs_stream_read()`
 * callback.
 *
 * Unless stated otherwise:
 * - `s` is created by `fs_stream_create_stream()` in `"rb"` mode, with
 *   `fake_file` prepared as the opened OSAL file and injected through
 *   `fs_stream_env_t`;
 * - `buf != NULL` and `st != NULL`;
 * - `n == FS_STREAM_READ_TEST_MSG_LEN`;
 * - the backend uses `fake_file` as its injected OSAL file dependency.
 * - the fake file buffered backing contains `FS_STREAM_READ_TEST_MSG`;
 *
 * The oracle combines:
 * - public return/status observation through `stream_read()`;
 * - adapter-to-OSAL interaction checks through `fake_file` spies.
 *
 * Doubles:
 * - fake_memory
 * - fake_file
 *
 * See adapter contract:
 * - @ref specifications_fs_stream_read "fs_stream_read() specifications"
 *
 * See public port contract:
 * - @ref specifications_stream_read "stream_read() specifications"
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_read "fs_stream_read() unit tests section"
 */
typedef enum {

	/**
	 * WHEN `stream_read(s, buf, n, st)` is called under the default valid
	 * preconditions
	 * AND the injected OSAL file read operation reports `OSAL_FILE_STATUS_IO`
	 * AND the injected OSAL file read operation returns `0`
	 * EXPECT:
	 * - returns `0`
	 * - sets `*st = STREAM_STATUS_IO_ERROR`
	 * - calls the injected OSAL file read operation exactly once
	 * - leaves buf unchanged
	 */
	FS_STREAM_READ_SCENARIO_OSAL_FILE_STATUS_IO = 0,

	/**
	 * WHEN `stream_read(s, buf, n, NULL)` is called under the default valid
	 * preconditions
	 * AND the injected OSAL file read operation reports `OSAL_FILE_STATUS_IO`
	 * AND the injected OSAL file read operation returns `0`
	 * EXPECT:
	 * - returns `0`
	 * - calls the injected OSAL file read operation exactly once
	 * - leaves buf unchanged
	 */
	FS_STREAM_READ_SCENARIO_OSAL_FILE_STATUS_IO_ST_NULL,

	/**
	 * WHEN `stream_read(s, buf, FS_STREAM_READ_TEST_EOF_N, st)` is called
	 * under the default valid preconditions
	 * AND the injected OSAL file read operation reports `OSAL_FILE_STATUS_EOF`
	 * EXPECT:
	 * - returns `FS_STREAM_READ_TEST_MSG_LEN`
	 * - sets `*st = STREAM_STATUS_EOF`
	 * - calls the injected OSAL file read operation exactly once
	 * - copies exactly `FS_STREAM_READ_TEST_MSG` into `buf`
	 */
	FS_STREAM_READ_SCENARIO_OK_EOF,

	/**
	 * WHEN `stream_read(s, buf, FS_STREAM_READ_TEST_EOF_N, NULL)` is called
	 * under the default valid preconditions
	 * AND the injected OSAL file read operation reports `OSAL_FILE_STATUS_EOF`
	 * EXPECT:
	 * - returns `FS_STREAM_READ_TEST_MSG_LEN`
	 * - calls the injected OSAL file read operation exactly once
	 * - copies exactly `FS_STREAM_READ_TEST_MSG` into `buf`
	 */
	FS_STREAM_READ_SCENARIO_OK_EOF_ST_NULL,

	/**
	 * WHEN `stream_read(s, buf, FS_STREAM_READ_TEST_MSG_LEN, st)` is called
	 * under the default valid preconditions
	 * AND the injected OSAL file read operation reports `OSAL_FILE_STATUS_OK`
	 * EXPECT:
	 * - returns `FS_STREAM_READ_TEST_MSG_LEN`
	 * - sets `*st = STREAM_STATUS_OK`
	 * - calls the injected OSAL file read operation exactly once
	 * - copies exactly `FS_STREAM_READ_TEST_MSG` into `buf`
	 */
	FS_STREAM_READ_SCENARIO_OK,

	/**
	 * WHEN `stream_read(s, buf, FS_STREAM_READ_TEST_MSG_LEN, NULL)` is called
	 * under the default valid preconditions
	 * AND the injected OSAL file read operation reports `OSAL_FILE_STATUS_OK`
	 * EXPECT:
	 * - returns `FS_STREAM_READ_TEST_MSG_LEN`
	 * - calls the injected OSAL file read operation exactly once
	 * - copies exactly `FS_STREAM_READ_TEST_MSG` into `buf`
	 */
	FS_STREAM_READ_SCENARIO_OK_ST_NULL,

} fs_stream_read_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;

	// arrange
	fs_stream_read_scenario_t scenario;
	osal_file_status_t osal_file_read_status; // e.g. OSAL_FILE_STATUS_EOF
	size_t n_arg;
	bool st_is_null;

	// assert
	stream_status_t expected_st;
	size_t expected_ret;
	const uint8_t *expected_buf;
	size_t expected_buf_len;
} test_fs_stream_read_case_t;

typedef struct {
	// runtime resources
	stream_t *s_arg;
	OSAL_FILE *fake_file;
	uint8_t buf[64];

	// injection
	fs_stream_env_t env;

	const test_fs_stream_read_case_t *tc;
} test_fs_stream_read_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_fs_stream_read(void **state)
{
	const test_fs_stream_read_case_t *tc =
		(const test_fs_stream_read_case_t *)(*state);

	test_fs_stream_read_fixture_t *fx =
		(test_fs_stream_read_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	const osal_mem_ops_t *real_mem = osal_mem_default_ops();
	fx->fake_file = fake_file_create_fake(real_mem);
	assert_non_null(fx->fake_file);
	fake_file_set_sink_backing(
		fx->fake_file,
		(const uint8_t *)FS_STREAM_READ_TEST_MSG,
		FS_STREAM_READ_TEST_MSG_LEN
	);
	fake_file_prepare_next_open_file(fx->fake_file);
	fake_file_prepare_next_open_status(OSAL_FILE_STATUS_OK);
	const stream_file_creator_args_t args = {
		.path = "dummy_pathname",
		.mode =	"rb"
	};
	const fs_stream_cfg_t cfg = fs_stream_default_cfg();

	fake_memory_reset();
	fx->env =
		fs_stream_default_env(
			osal_file_test_fake_ops(),
			osal_mem_test_fake_ops(),
			osal_mem_test_fake_ops()
		);
	assert_int_equal(
		fs_stream_create_stream(
			&fx->s_arg,
			&args,
			&cfg,
			&fx->env
		),
		STREAM_STATUS_OK
	);

	*state = fx;
	return 0;
}

static int teardown_fs_stream_read(void **state)
{
	test_fs_stream_read_fixture_t *fx =
		(test_fs_stream_read_fixture_t *)(*state);

	if (fx->s_arg) {
		stream_destroy(&fx->s_arg);
		fx->s_arg = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	if (fx->fake_file) {
		fake_file_destroy_fake(fx->fake_file);
		fx->fake_file = NULL;
	}

	fake_file_reset();

	osal_free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_fs_stream_read(void **state) {
	test_fs_stream_read_fixture_t *fx =
		(test_fs_stream_read_fixture_t *)(*state);
	const test_fs_stream_read_case_t *tc = fx->tc;

	// ARRANGE

	size_t ret = 0;

	stream_status_t st = STREAM_STATUS_OK;
	stream_status_t *st_arg = (tc->st_is_null) ? NULL : &st;

	fake_file_set_read_status(fx->fake_file, tc->osal_file_read_status);

	// ACT
	ret =
		stream_read(
			fx->s_arg,
			fx->buf,
			tc->n_arg,
			st_arg
		);

	// ASSERT

	assert_true(
		fake_file_read_call_count(fx->fake_file)
		==
		1
	);
	assert_true(
		fake_file_pos(fx->fake_file)
		==
		ret
	);

	assert_true(ret == tc->expected_ret);
	if (!tc->st_is_null) {
		assert_int_equal(*st_arg, tc->expected_st);
	}

	if (tc->expected_buf_len > 0) {
		assert_memory_equal(
			fx->buf,
			tc->expected_buf,
			tc->expected_buf_len
		);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_fs_stream_read_case_t
CASE_FS_STREAM_READ_OSAL_FILE_STATUS_IO = {
	.name = "fs_stream_read_osal_file_status_io",
	.scenario = FS_STREAM_READ_SCENARIO_OSAL_FILE_STATUS_IO,
	.osal_file_read_status = OSAL_FILE_STATUS_IO,
	.n_arg = FS_STREAM_READ_TEST_MSG_LEN,
	.st_is_null = false,

	.expected_st = STREAM_STATUS_IO_ERROR,
	.expected_ret = 0,
	.expected_buf = "",
	.expected_buf_len = 0
};

static const test_fs_stream_read_case_t
CASE_FS_STREAM_READ_OSAL_FILE_STATUS_IO_ST_NULL = {
	.name = "fs_stream_read_osal_file_status_io_st_null",
	.scenario = FS_STREAM_READ_SCENARIO_OSAL_FILE_STATUS_IO_ST_NULL,
	.osal_file_read_status = OSAL_FILE_STATUS_IO,
	.n_arg = FS_STREAM_READ_TEST_MSG_LEN,
	.st_is_null = true,

	.expected_ret = 0,
	.expected_buf = "",
	.expected_buf_len = 0
};

static const test_fs_stream_read_case_t
CASE_FS_STREAM_READ_OK_EOF = {
	.name = "fs_stream_read_ok_eof",
	.scenario = FS_STREAM_READ_SCENARIO_OK_EOF,
	.osal_file_read_status = OSAL_FILE_STATUS_EOF,
	.n_arg = FS_STREAM_READ_TEST_EOF_N,
	.st_is_null = false,

	.expected_st = STREAM_STATUS_EOF,
	.expected_ret = FS_STREAM_READ_TEST_MSG_LEN,
	.expected_buf = FS_STREAM_READ_TEST_MSG,
	.expected_buf_len = FS_STREAM_READ_TEST_MSG_LEN
};

static const test_fs_stream_read_case_t
CASE_FS_STREAM_READ_OK_EOF_ST_NULL = {
	.name = "fs_stream_read_ok_eof_st_null",
	.scenario = FS_STREAM_READ_SCENARIO_OK_EOF_ST_NULL,
	.osal_file_read_status = OSAL_FILE_STATUS_EOF,
	.n_arg = FS_STREAM_READ_TEST_EOF_N,
	.st_is_null = true,

	.expected_ret = FS_STREAM_READ_TEST_MSG_LEN,
	.expected_buf = FS_STREAM_READ_TEST_MSG,
	.expected_buf_len = FS_STREAM_READ_TEST_MSG_LEN
};

static const test_fs_stream_read_case_t
CASE_FS_STREAM_READ_OK = {
	.name = "fs_stream_read_ok",
	.scenario = FS_STREAM_READ_SCENARIO_OK,
	.osal_file_read_status = OSAL_FILE_STATUS_OK,
	.n_arg = FS_STREAM_READ_TEST_MSG_LEN,
	.st_is_null = false,

	.expected_st = STREAM_STATUS_OK,
	.expected_ret = FS_STREAM_READ_TEST_MSG_LEN,
	.expected_buf = FS_STREAM_READ_TEST_MSG,
	.expected_buf_len = FS_STREAM_READ_TEST_MSG_LEN
};

static const test_fs_stream_read_case_t
CASE_FS_STREAM_READ_OK_ST_NULL = {
	.name = "fs_stream_read_ok_st_null",
	.scenario = FS_STREAM_READ_SCENARIO_OK_ST_NULL,
	.osal_file_read_status = OSAL_FILE_STATUS_OK,
	.n_arg = FS_STREAM_READ_TEST_MSG_LEN,
	.st_is_null = true,

	.expected_ret = FS_STREAM_READ_TEST_MSG_LEN,
	.expected_buf = FS_STREAM_READ_TEST_MSG,
	.expected_buf_len = FS_STREAM_READ_TEST_MSG_LEN
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_READ_CASES(X) \
X(CASE_FS_STREAM_READ_OSAL_FILE_STATUS_IO) \
X(CASE_FS_STREAM_READ_OSAL_FILE_STATUS_IO_ST_NULL) \
X(CASE_FS_STREAM_READ_OK_EOF) \
X(CASE_FS_STREAM_READ_OK_EOF_ST_NULL) \
X(CASE_FS_STREAM_READ_OK) \
X(CASE_FS_STREAM_READ_OK_ST_NULL)

#define FS_STREAM_MAKE_READ_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_read, case_sym)

static const struct CMUnitTest fs_stream_read_fs_stream_tests[] = {
	FS_STREAM_READ_CASES(FS_STREAM_MAKE_READ_TEST)
};

#undef FS_STREAM_READ_CASES
#undef FS_STREAM_MAKE_READ_TEST

/** @endcond */

/**
 * @brief Scenarios for `fs_stream_flush()`, exercised through `stream_flush()`.
 *
 * @details
 * These tests create a valid `fs_stream` instance and call
 * `stream_flush(s)` to exercise the private `fs_stream_flush()` callback.
 *
 * Unless stated otherwise:
 * - `s` is created by `fs_stream_create_stream()` in `"wb"` mode, with
 *   `fake_file` prepared as the opened OSAL file and injected through
 *   `fs_stream_env_t`;
 * - the backend uses `fake_file` as its injected OSAL file dependency.
 *
 * The oracle combines:
 * - public return/status observation through `stream_flush()`;
 * - adapter-to-OSAL interaction checks through `fake_file` spies.
 *
 * Doubles:
 * - fake_memory
 * - fake_file
 *
 * See adapter contract:
 * - @ref specifications_fs_stream_flush "fs_stream_flush() specifications"
 *
 * See public port contract:
 * - @ref specifications_stream_flush "stream_flush() specifications"
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_flush "fs_stream_flush() unit tests section"
 */
typedef enum {

	/**
	 * WHEN `stream_flush(s)` is called under the default valid preconditions
	 * AND the injected OSAL file flush operation reports `OSAL_FILE_STATUS_IO`
	 * EXPECT:
	 * - returns `STREAM_STATUS_IO_ERROR`
	 * - calls the injected OSAL file flush operation exactly once
	 */
	FS_STREAM_FLUSH_SCENARIO_OSAL_FILE_STATUS_IO = 0,

	/**
	 * WHEN `stream_flush(s)` is called under the default valid preconditions
	 * AND the injected OSAL file flush operation returns `OSAL_FILE_STATUS_OK`
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - calls the injected OSAL file flush operation exactly once
	 */
	FS_STREAM_FLUSH_SCENARIO_OSAL_FILE_STATUS_OK

} fs_stream_flush_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;

	// arrange
	fs_stream_flush_scenario_t scenario;
	osal_file_status_t osal_file_flush_status;

	// assert
	stream_status_t expected_ret;
} test_fs_stream_flush_case_t;

typedef struct {
	// runtime resources
	stream_t *s_arg;
	OSAL_FILE *fake_file;

	// injection
	fs_stream_env_t env;

	const test_fs_stream_flush_case_t *tc;
} test_fs_stream_flush_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_fs_stream_flush(void **state)
{
	const test_fs_stream_flush_case_t *tc =
		(const test_fs_stream_flush_case_t *)(*state);

	test_fs_stream_flush_fixture_t *fx =
		(test_fs_stream_flush_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	const osal_mem_ops_t *real_mem = osal_mem_default_ops();
	fx->fake_file = fake_file_create_fake(real_mem);
	assert_non_null(fx->fake_file);
	fake_file_prepare_next_open_file(fx->fake_file);
	fake_file_prepare_next_open_status(OSAL_FILE_STATUS_OK);
	const stream_file_creator_args_t args = {
		.path = "dummy_pathname",
		.mode =	"wb"
	};
	const fs_stream_cfg_t cfg = fs_stream_default_cfg();
	fake_memory_reset();
	fx->env =
		fs_stream_default_env(
			osal_file_test_fake_ops(),
			osal_mem_test_fake_ops(),
			osal_mem_test_fake_ops()
		);
	assert_int_equal(
		fs_stream_create_stream(
			&fx->s_arg,
			&args,
			&cfg,
			&fx->env
		),
		STREAM_STATUS_OK
	);

	*state = fx;
	return 0;
}

static int teardown_fs_stream_flush(void **state)
{
	test_fs_stream_flush_fixture_t *fx =
		(test_fs_stream_flush_fixture_t *)(*state);

	if (fx->s_arg) {
		fake_file_set_flush_status(fx->fake_file, OSAL_FILE_STATUS_OK);
		stream_destroy(&fx->s_arg);
		fx->s_arg = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	if (fx->fake_file) {
		fake_file_destroy_fake(fx->fake_file);
		fx->fake_file = NULL;
	}

	fake_file_reset();

	osal_free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_fs_stream_flush(void **state) {
	test_fs_stream_flush_fixture_t *fx =
		(test_fs_stream_flush_fixture_t *)(*state);
	const test_fs_stream_flush_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t ret = STREAM_STATUS_OK;
	fake_file_set_flush_status(fx->fake_file, tc->osal_file_flush_status);

	// ACT
	ret = stream_flush(fx->s_arg);

	// ASSERT
	assert_true(
		fake_file_flush_call_count(fx->fake_file)
		==
		1
	);
	assert_true(ret == tc->expected_ret);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_fs_stream_flush_case_t
CASE_FS_STREAM_FLUSH_OSAL_FILE_STATUS_IO = {
	.name = "fs_stream_flush_osal_file_status_io",
	.scenario = FS_STREAM_FLUSH_SCENARIO_OSAL_FILE_STATUS_IO,
	.osal_file_flush_status = OSAL_FILE_STATUS_IO,

	.expected_ret = STREAM_STATUS_IO_ERROR,
};

static const test_fs_stream_flush_case_t
CASE_FS_STREAM_FLUSH_OSAL_FILE_STATUS_OK = {
	.name = "fs_stream_flush_osal_file_status_ok",
	.scenario = FS_STREAM_FLUSH_SCENARIO_OSAL_FILE_STATUS_OK,
	.osal_file_flush_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_OK,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_FLUSH_CASES(X) \
X(CASE_FS_STREAM_FLUSH_OSAL_FILE_STATUS_IO) \
X(CASE_FS_STREAM_FLUSH_OSAL_FILE_STATUS_OK)

#define FS_STREAM_MAKE_FLUSH_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_flush, case_sym)

static const struct CMUnitTest fs_stream_flush_fs_stream_tests[] = {
	FS_STREAM_FLUSH_CASES(FS_STREAM_MAKE_FLUSH_TEST)
};

#undef FS_STREAM_FLUSH_CASES
#undef FS_STREAM_MAKE_FLUSH_TEST

/** @endcond */

/**
 * @brief Scenarios for `fs_stream_close()`, exercised through `stream_destroy()`.
 *
 * @details
 * These tests create a valid `fs_stream` instance and call
 * `stream_destroy(s)` to exercise the private `fs_stream_close()` callback.
 *
 * Unless stated otherwise:
 * - `s` is created by `fs_stream_create_stream()` in `"wb"` mode, with
 *   `fake_file` prepared as the opened OSAL file and injected through
 *   `fs_stream_env_t`;
 * - the backend uses `fake_file` as its injected OSAL file dependency.
 *
 * The oracle combines:
 * - public return/status observation through `stream_destroy()`;
 * - adapter-to-OSAL interaction checks through `fake_file` spies.
 *
 * Doubles:
 * - fake_memory
 * - fake_file
 *
 * See adapter contract:
 * - @ref specifications_fs_stream_close "fs_stream_close() specifications"
 *
 * See public port contract:
 * - @ref specifications_stream_destroy "stream_destroy() specifications"
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_close "fs_stream_close() unit tests section"
 */
typedef enum {

	/**
	 * WHEN `stream_destroy(s)` is called under the default valid preconditions
	 * AND the injected OSAL file close operation reports `OSAL_FILE_STATUS_IO`
	 * EXPECT:
	 * - returns `STREAM_STATUS_IO_ERROR`
	 * - calls the injected OSAL file close operation exactly once
	 */
	FS_STREAM_CLOSE_SCENARIO_OSAL_FILE_STATUS_IO = 0,

	/**
	 * WHEN `stream_destroy(s)` is called under the default valid preconditions
	 * AND the injected OSAL file close operation returns `OSAL_FILE_STATUS_OK`
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - calls the injected OSAL file close operation exactly once
	 */
	FS_STREAM_CLOSE_SCENARIO_OSAL_FILE_STATUS_OK

} fs_stream_close_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;

	// arrange
	fs_stream_close_scenario_t scenario;
	osal_file_status_t osal_file_close_status;

	// assert
	stream_status_t expected_ret;
} test_fs_stream_close_case_t;

typedef struct {
	// runtime resources
	stream_t *s_arg_value;
	OSAL_FILE *fake_file;

	// injection
	fs_stream_env_t env;

	const test_fs_stream_close_case_t *tc;
} test_fs_stream_close_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_fs_stream_close(void **state)
{
	const test_fs_stream_close_case_t *tc =
		(const test_fs_stream_close_case_t *)(*state);

	test_fs_stream_close_fixture_t *fx =
		(test_fs_stream_close_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	const osal_mem_ops_t *real_mem = osal_mem_default_ops();
	fx->fake_file = fake_file_create_fake(real_mem);
	assert_non_null(fx->fake_file);
	fake_file_prepare_next_open_file(fx->fake_file);
	fake_file_prepare_next_open_status(OSAL_FILE_STATUS_OK);
	const stream_file_creator_args_t args = {
		.path = "dummy_pathname",
		.mode =	"wb"
	};
	const fs_stream_cfg_t cfg = fs_stream_default_cfg();
	fake_memory_reset();
	fx->env =
		fs_stream_default_env(
			osal_file_test_fake_ops(),
			osal_mem_test_fake_ops(),
			osal_mem_test_fake_ops()
		);
	assert_int_equal(
		fs_stream_create_stream(
			&fx->s_arg_value,
			&args,
			&cfg,
			&fx->env
		),
		STREAM_STATUS_OK
	);

	*state = fx;
	return 0;
}

static int teardown_fs_stream_close(void **state)
{
	test_fs_stream_close_fixture_t *fx =
		(test_fs_stream_close_fixture_t *)(*state);

	stream_destroy(&fx->s_arg_value);

	fake_file_reset();

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	if (fx->fake_file) {
		fake_file_destroy_fake(fx->fake_file);
		fx->fake_file = NULL;
	}

	osal_free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_fs_stream_close(void **state) {
	test_fs_stream_close_fixture_t *fx =
		(test_fs_stream_close_fixture_t *)(*state);
	const test_fs_stream_close_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t ret = STREAM_STATUS_OK;
	stream_t **s_arg = &fx->s_arg_value;
	fake_file_set_close_status(fx->fake_file, tc->osal_file_close_status);
	void *backend_snapshot = stream_get_backend(fx->s_arg_value);

	// ACT
	ret = stream_destroy(s_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);
	assert_null(fx->s_arg_value);
	assert_true(
		fake_file_close_call_count(fx->fake_file)
		==
		1
	);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_fs_stream_close_case_t
CASE_FS_STREAM_CLOSE_OSAL_FILE_STATUS_IO = {
	.name = "fs_stream_close_osal_file_status_io",
	.scenario = FS_STREAM_CLOSE_SCENARIO_OSAL_FILE_STATUS_IO,
	.osal_file_close_status = OSAL_FILE_STATUS_IO,

	.expected_ret = STREAM_STATUS_IO_ERROR,
};

static const test_fs_stream_close_case_t
CASE_FS_STREAM_CLOSE_OSAL_FILE_STATUS_OK = {
	.name = "fs_stream_close_osal_file_status_ok",
	.scenario = FS_STREAM_CLOSE_SCENARIO_OSAL_FILE_STATUS_OK,
	.osal_file_close_status = OSAL_FILE_STATUS_OK,

	.expected_ret = STREAM_STATUS_OK,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_CLOSE_CASES(X) \
X(CASE_FS_STREAM_CLOSE_OSAL_FILE_STATUS_IO) \
X(CASE_FS_STREAM_CLOSE_OSAL_FILE_STATUS_OK)

#define FS_STREAM_MAKE_CLOSE_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_close, case_sym)

static const struct CMUnitTest fs_stream_close_fs_stream_tests[] = {
	FS_STREAM_CLOSE_CASES(FS_STREAM_MAKE_CLOSE_TEST)
};

#undef FS_STREAM_CLOSE_CASES
#undef FS_STREAM_MAKE_CLOSE_TEST

/** @endcond */

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	static const struct CMUnitTest fs_stream_non_parametric_tests[] = {
		cmocka_unit_test(test_fs_stream_default_cfg),
		cmocka_unit_test(test_fs_stream_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(fs_stream_non_parametric_tests, NULL, NULL);
	failed += cmocka_run_group_tests(fs_stream_stream_create_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(fs_stream_create_desc_fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(fs_stream_write_fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(fs_stream_read_fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(fs_stream_flush_fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(fs_stream_close_fs_stream_tests, NULL, NULL);
	return failed;
}

/** @endcond */
