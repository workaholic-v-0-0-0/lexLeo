/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 * File:
 * src/foundation/stream/adapters/fs_stream/tests/unit/
 * unit_test_fs_stream.c
 *
 */

// <here> now we can inject fake_file and fake_mem

/* draft



*/

#include "fs_stream_test_api.h"
#include "osal/mem/osal_mem_env.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lexleo_cmocka_xmacro_helpers.h"
#include "osal_mem_fake_provider.h"

//-----------------------------------------------------------------------------
// CONTRACT — fs_stream_create_stream()
//
// stream_status_t fs_stream_create_stream(
// stream_t **out,
//     const fs_stream_args_t *args,
//     const fs_stream_cfg_t *cfg,
//     const fs_stream_env_t *env );
//
// - invalid args (out == NULL || ctx == NULL) => STREAM_STATUS_ERROR
// - any failure => STREAM_STATUS_ERROR and *out == NULL
// - success => STREAM_STATUS_OK and produces a valid stream_t
//
// Doubles:
//     osal_mem_ops_t via fake_memory (OOM + leak checks)
//     osal_file_ops_t via fake_file
//-----------------------------------------------------------------------------

typedef enum {
	FS_CREATE_STREAM_SCENARIO_OK = 0,
	FS_CREATE_STREAM_SCENARIO_OUT_NULL,
	FS_CREATE_STREAM_SCENARIO_ARGS_NULL,
	FS_CREATE_STREAM_SCENARIO_OOM,
	FS_CREATE_STREAM_SCENARIO_OPEN_FAIL,
} fs_create_stream_scenario_t;

typedef struct {
	const char *name;

	// arrange
	fs_create_stream_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based (scenario == FS_CREATE_STREAM_SCENARIO_OOM)
	osal_file_status_t open_fail_status; // e.g. OSAL_FILE_NOENT

	// assert
	stream_status_t expected_ret;
	bool check_out; // whether we can assert on *out
	bool expect_out_null; // valid only if check_out == true
} test_fs_stream_create_stream_case_t;

typedef struct {
	// runtime resources
	stream_t *out;

	// injection
	fs_stream_env_t env;

	fs_stream_create_args_t args;

	// fake file backing
	uint8_t backing[64];

	// reference to test case
	const test_fs_stream_create_stream_case_t *tc;
} test_fs_stream_create_stream_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_fs_create_stream(void **state)
{
	const test_fs_stream_create_stream_case_t *tc =
		(const test_fs_stream_create_stream_case_t *)(*state);

	test_fs_stream_create_stream_fixture_t *fx =
		(test_fs_stream_create_stream_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_file_reset();
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	memset(fx->backing, 0, sizeof(fx->backing));
	fake_file_set_backing(fx->backing, sizeof(fx->backing), 0);
	fake_file_fail_disable();

	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OPEN_FAIL) {
		fake_file_fail_enable(FAKE_FILE_OP_OPEN, 1, tc->open_fail_status);
	}

	// DI
	fx->env.file_env.mem = osal_mem_test_fake_ops();
	fx->env.file_ops = osal_file_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	memset(&fx->args, 0, sizeof(fx->args));
	fx->args.path_utf8 = "crazy_injection.txt";
	fx->args.flags = OSAL_FILE_READ | OSAL_FILE_WRITE | OSAL_FILE_CREATE | OSAL_FILE_TRUNC;

	*state = fx;
	return 0;
}

static int teardown_fs_create_stream(void **state)
{
	test_fs_stream_create_stream_fixture_t *fx =
		(test_fs_stream_create_stream_fixture_t *)(*state);

	if (fx->out) {
		stream_destroy(fx->out);
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
	stream_status_t ret = STREAM_STATUS_ERROR;

	stream_t **out_arg = &fx->out;
	const fs_stream_ctx_t *ctx_arg = &fx->ctx;
	const fs_stream_create_args_t *args_arg = &fx->args;

	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_ARGS_NULL) args_arg = NULL;

	if (out_arg) fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu;

	// ACT
	ret = fs_stream_create_stream(out_arg, ctx_arg, args_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);

	if (tc->check_out && out_arg) {
		if (tc->expect_out_null) assert_null(fx->out);
		else assert_non_null(fx->out);
	}

	if (tc->scenario == FS_CREATE_STREAM_SCENARIO_OK) {
		const char msg[] = "hello";
		char buf[8] = {0};
		assert_int_equal(stream_write(fx->out, msg, sizeof(msg) - 1), (int)(sizeof(msg) - 1));
		assert_int_equal(stream_read(fx->out, buf, sizeof(msg) - 1), (int)(sizeof(msg) - 1));
		assert_memory_equal(buf, msg, sizeof(msg) - 1);
		assert_int_equal(stream_flush(fx->out), 0);
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
	.check_out = true,
	.expect_out_null = false
};







//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define FS_STREAM_CREATE_STREAM_CASES(X) \
X(CASE_FS_OK)

#define FS_STREAM_MAKE_CREATE_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(create_stream, case_sym)

static const struct CMUnitTest create_stream_tests[] = {
	FS_STREAM_CREATE_STREAM_CASES(FS_STREAM_MAKE_CREATE_STREAM_TEST)
};

#undef FS_STREAM_CREATE_STREAM_CASES
#undef FS_STREAM_MAKE_CREATE_STREAM_TEST




//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(create_stream_tests, NULL, NULL);
	return failed;
}
