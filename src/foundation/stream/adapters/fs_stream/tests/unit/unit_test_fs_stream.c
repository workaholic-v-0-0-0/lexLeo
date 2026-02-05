/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_fs_stream.c
 * @ingroup fs_stream_unit_tests
 * @brief Unit tests validating the contracts of the fs_stream API.
 */

#include "fs_stream/cr/fs_stream_cr_api.h"

#include "stream/lifecycle/stream_lifecycle.h"
#include "stream/borrowers/stream.h"
#include "osal/file/test/osal_file_fake_provider.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

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
 * @ingroup fs_stream_unit_tests
 *
 * ---------------------------------------------------------------------------
 * CONTRACT — fs_stream_create_stream()
 * ---------------------------------------------------------------------------
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
 * - Leaves `*out` unchanged.
 *
 * Doubles:
 * - fake_memory
 * - fake_file
 */

typedef enum {
	FS_CREATE_STREAM_SCENARIO_OK = 0,
	FS_CREATE_STREAM_SCENARIO_OUT_NULL,
	FS_CREATE_STREAM_SCENARIO_ARGS_NULL,
	FS_CREATE_STREAM_SCENARIO_CFG_NULL,
	FS_CREATE_STREAM_SCENARIO_ENV_NULL,
    FS_CREATE_STREAM_SCENARIO_PATH_NULL,
    FS_CREATE_STREAM_SCENARIO_PATH_EMPTY,
    FS_CREATE_STREAM_SCENARIO_FLAGS_ZERO,
	FS_CREATE_STREAM_SCENARIO_OOM,
	FS_CREATE_STREAM_SCENARIO_OPEN_FAIL,
} fs_create_stream_scenario_t;

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
	osal_file_status_t open_fail_status; // e.g. OSAL_FILE_NOENT

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_fs_stream_create_stream_case_t;

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

static int setup_fs_stream_create_stream(void **state)
{
	const test_fs_stream_create_stream_case_t *tc =
		(const test_fs_stream_create_stream_case_t *)(*state);

	test_fs_stream_create_stream_fixture_t *fx =
		(test_fs_stream_create_stream_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	fake_memory_reset();
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	memset(fx->backing, 0, sizeof(fx->backing));
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

static const struct CMUnitTest create_fs_stream_tests[] = {
	FS_STREAM_CREATE_STREAM_CASES(FS_STREAM_MAKE_CREATE_STREAM_TEST)
};

#undef FS_STREAM_CREATE_STREAM_CASES
#undef FS_STREAM_MAKE_CREATE_STREAM_TEST

/**
 * @ingroup fs_stream_unit_tests
 *
 * @section contract_fs_stream_create_desc CONTRACT — fs_stream_create_desc()
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
 * - `env->file_env.mem`, `env->file_ops` must not be NULL.
 *
 * Success:
 * - Returns STREAM_STATUS_OK.
 * - Stores a valid adapter descriptor in `*out`.
 * - The produced descriptor must be released via `out->ud_dtor()`.
 *
 * Failure:
 * - Returns:
 *     - STREAM_STATUS_INVALID for invalid arguments
 *     - STREAM_STATUS_OOM on allocation failure
 * - If `out` is not NULL, resets `*out` to an empty descriptor.
 *
 * Doubles:
 * - fake_memory
 */

typedef enum {
	FS_STREAM_CREATE_DESC_SCENARIO_OK = 0,
	FS_STREAM_CREATE_DESC_SCENARIO_OUT_NULL,
	FS_STREAM_CREATE_DESC_SCENARIO_KEY_NULL,
	FS_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY,
	FS_STREAM_CREATE_DESC_SCENARIO_CFG_NULL,
    FS_STREAM_CREATE_DESC_SCENARIO_ENV_NULL,
	FS_STREAM_CREATE_DESC_SCENARIO_MEM_NULL,
	FS_STREAM_CREATE_DESC_SCENARIO_ENV_FILE_ENV_MEM_NULL,
	FS_STREAM_CREATE_DESC_SCENARIO_ENV_FILE_OPS_NULL,
	FS_STREAM_CREATE_DESC_SCENARIO_OOM
} fs_stream_create_desc_scenario_t;

typedef enum {
	DESC_CHECK_NONE,
    DESC_EXPECT_EMPTY,
    DESC_EXPECT_VALID
} desc_expect_t;

typedef struct {
	const char *name;

	// arrange
	fs_stream_create_desc_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based (scenario == FS_STREAM_CREATE_DESC_SCENARIO_OOM)

	// assert
	stream_status_t expected_ret;
	desc_expect_t desc_expect;
} test_fs_stream_create_desc_case_t;

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

static int setup_fs_stream_create_desc(void **state)
{
	const test_fs_stream_create_desc_case_t *tc =
		(const test_fs_stream_create_desc_case_t *)(*state);

	test_fs_stream_create_desc_fixture_t *fx =
		(test_fs_stream_create_desc_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
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
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_ENV_FILE_ENV_MEM_NULL) fx->env.file_env.mem = NULL;
	if (tc->scenario == FS_STREAM_CREATE_DESC_SCENARIO_ENV_FILE_OPS_NULL) fx->env.file_ops = NULL;

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

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_ENV_FILE_ENV_MEM_NULL = {
	.name = "fs_stream_create_desc_env_file_env_mem_null",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_ENV_FILE_ENV_MEM_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_fs_stream_create_desc_case_t CASE_FS_STREAM_CREATE_DESC_ENV_FILE_OPS_NULL = {
	.name = "fs_stream_create_desc_env_file_ops_null",
	.scenario = FS_STREAM_CREATE_DESC_SCENARIO_ENV_FILE_OPS_NULL,
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
X(CASE_FS_STREAM_CREATE_DESC_ENV_FILE_ENV_MEM_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_ENV_FILE_OPS_NULL) \
X(CASE_FS_STREAM_CREATE_DESC_OOM_1)

#define FS_STREAM_MAKE_CREATE_DESC_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_create_desc, case_sym)

static const struct CMUnitTest create_desc_fs_stream_tests[] = {
	FS_STREAM_CREATE_DESC_CASES(FS_STREAM_MAKE_CREATE_DESC_TEST)
};

#undef FS_STREAM_CREATE_DESC_CASES
#undef FS_STREAM_MAKE_CREATE_DESC_TEST

/**
 * @ingroup fs_stream_unit_tests
 *
 * @section contract_fs_stream_desc_ctor CONTRACT — fs_stream_create_desc() / descriptor ctor usage
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
 */

typedef enum {
	FS_STREAM_DESC_CTOR_SCENARIO_OK = 0,
	FS_STREAM_DESC_CTOR_SCENARIO_ARGS_NULL,
	FS_STREAM_DESC_CTOR_SCENARIO_OUT_NULL,
	FS_STREAM_DESC_CTOR_SCENARIO_PATH_NULL,
	FS_STREAM_DESC_CTOR_SCENARIO_PATH_EMPTY,
	FS_STREAM_DESC_CTOR_SCENARIO_FLAGS_ZERO,
	FS_STREAM_DESC_CTOR_SCENARIO_OOM,
	FS_STREAM_DESC_CTOR_SCENARIO_OPEN_FAIL,
} fs_stream_desc_ctor_scenario_t;

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

static int setup_fs_stream_desc_ctor(void **state)
{
	const test_fs_stream_desc_ctor_case_t *tc =
		(const test_fs_stream_desc_ctor_case_t *)(*state);

	test_fs_stream_desc_ctor_fixture_t *fx =
		(test_fs_stream_desc_ctor_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	fake_memory_reset();

	memset(fx->backing, 0, sizeof(fx->backing));
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
	int failed = 0;
	failed += cmocka_run_group_tests(create_fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(create_desc_fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(desc_ctor_fs_stream_tests, NULL, NULL);
	return failed;
}
