/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_fs_stream_cr.c
 * @ingroup fs_stream_unit_tests
 * @brief Unit tests implementation for fs_stream_cr.c.
 *
 * See also:
 * - @ref testing_foundation_fs_stream_cr_unit "fs_stream_cr.c unit tests page"
 * - @ref specifications_fs_stream "fs_stream specifications"
 */

#include "fs_stream/cr/fs_stream_cr_api.h"
#include "stream/cr/stream_cr_api.h"

#include "fs_stream/tests/fs_stream_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"
#include "osal/file/test/osal_file_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**********************************************************************************************************************
 * @brief Test `fs_stream_vtbl()`.
 *
 * See contract:
 * - @ref specifications_fs_stream_vtbl "fs_stream_vtbl() specifications".
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_fs_stream_vtbl "fs_stream_vtbl() unit tests section"
 */
static void test_fs_stream_vtbl(void **state) {
	(void)state;

	// ACT
	const stream_vtbl_t *ret = fs_stream_vtbl();

	// ASSERT
	assert_non_null(ret);
	assert_non_null(ret->read);
	assert_non_null(ret->write);
	assert_non_null(ret->flush);
	assert_non_null(ret->close);
}

/**********************************************************************************************************************
 * @brief Test `fs_stream_default_cfg()`.
 *
 * See contract:
 * - @ref specifications_fs_stream_default_cfg "fs_stream_default_cfg() specifications".
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_fs_stream_default_cfg "fs_stream_default_cfg() unit tests section"
 */
static void test_fs_stream_default_cfg(void **state) {
	(void)state;

	// ACT
	fs_stream_cfg_t ret = fs_stream_default_cfg();

	// ASSERT
	assert_int_equal(ret.reserved, 0);
}

/**********************************************************************************************************************
 * @brief Test `fs_stream_default_env()`.
 *
 * See contract:
 * - @ref specifications_fs_stream_default_env "fs_stream_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_fs_stream_default_env "fs_stream_default_env() unit tests section"
 */
static void test_fs_stream_default_env(void **state) {
	(void)state;

	// ARRANGE
	const osal_file_ops_t *dummy_file_ops = (const osal_file_ops_t *)(uintptr_t)0xDEADC0DEu;
	const osal_mem_ops_t *dummy_mem_ops = (const osal_mem_ops_t *)(uintptr_t)0xBADC0FFEu;

	// ACT
	fs_stream_env_t ret = fs_stream_default_env(dummy_file_ops, dummy_mem_ops);

	// ASSERT
	assert_ptr_equal(ret.file_ops, dummy_file_ops);
	assert_ptr_equal(ret.mem_ops, dummy_mem_ops);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `fs_stream_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_fs_stream_create "fs_stream_create() specifications".
 * - @ref specifications_fs_stream_complete_default_init "fs_stream_complete_default_init() specifications".
 * - @ref specifications_fs_stream_close "fs_stream_close() specifications".
 * - @ref specifications_fs_stream_vtbl "fs_stream_vtbl() specifications".
 * - @ref specifications_fs_stream_default_cfg "fs_stream_default_cfg() specifications".
 * - @ref specifications_fs_stream_default_env "fs_stream_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_fs_stream_t_lifecycle "`fs_stream_t` lifecycle unit tests section"
 */
typedef enum {
	FS_STREAM_T_LIFECYCLE_SCENARIO_FS_STREAM_HANDLE_OOM = 0,
	FS_STREAM_T_LIFECYCLE_SCENARIO_OPEN_OOM,
	FS_STREAM_T_LIFECYCLE_SCENARIO_OPEN_FAILS_WITH_PERM_ERROR,
	FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_NULL,
	FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_EMPTY,
	FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_TOO_LONG,
	FS_STREAM_T_LIFECYCLE_SCENARIO_OK_FILE_INJECTED_BY_TEST_INFRA,
	FS_STREAM_T_LIFECYCLE_SCENARIO_OK_FILE_CREATED_BY_DEFAULT_INIT,
	FS_STREAM_T_LIFECYCLE_SCENARIO_CLOSE_FAILS_WITH_IO_ERROR
} fs_stream_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	fs_stream_t_lifecycle_scenario_t scenario;
} test_fs_stream_t_lifecycle_case_t;

typedef struct {
	const test_fs_stream_t_lifecycle_case_t *tc;
} test_fs_stream_t_lifecycle_fixture_t;

static int setup_fs_stream_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(fs_stream_t_lifecycle, state, tc, fx);

	fake_file_reset();
	fake_memory_reset();

	*state = fx;
	return 0;
}

static int teardown_fs_stream_t_lifecycle(void **state)
{
	test_fs_stream_t_lifecycle_fixture_t *fx = (test_fs_stream_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_fs_stream_t_lifecycle(void **state)
{
	test_fs_stream_t_lifecycle_fixture_t *fx = (test_fs_stream_t_lifecycle_fixture_t *)(*state);
	const test_fs_stream_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	fs_stream_t *sentinel = (fs_stream_t *)(uintptr_t)0xDEADC0DEu;
	fs_stream_t *fs_stream = sentinel;
	const fs_stream_env_t fs_stream_env = {
		.file_ops = osal_file_test_fake_ops(),
		.mem_ops = osal_mem_test_fake_ops()
	};
	if (tc->scenario == FS_STREAM_T_LIFECYCLE_SCENARIO_FS_STREAM_HANDLE_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	fs_stream_status_t ret = fs_stream_create(&fs_stream, &fs_stream_env);

	// ASSERT
	if (tc->scenario == FS_STREAM_T_LIFECYCLE_SCENARIO_FS_STREAM_HANDLE_OOM) {
		assert_int_equal(ret, FS_STREAM_STATUS_OOM);
		assert_ptr_equal(fs_stream, sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}
	assert_int_equal(ret, FS_STREAM_STATUS_OK);
	assert_true(fs_stream != sentinel);
	assert_non_null(fs_stream);
	assert_ptr_equal(fs_stream_get_file_ops(fs_stream), fs_stream_env.file_ops);
	assert_ptr_equal(fs_stream_get_mem_ops(fs_stream), fs_stream_env.mem_ops);
	assert_null(fs_stream_get_file(fs_stream));

	// ARRANGE
	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	fake_file_t fake_file_default_init;
	fake_file_t fake_file_injected_by_test_infra;
	fake_file_init_instance(&fake_file_default_init);
	fake_file_init_instance(&fake_file_injected_by_test_infra);
	g_fake_file_ctrl_seq[0].next_open_out = &fake_file_default_init;
	stream_regular_file_creator_args_t args = {
		.path = "valid_path",
		.mode = "wb"
	};
	char too_long_path_storage[257];
	switch (tc->scenario) {
		case FS_STREAM_T_LIFECYCLE_SCENARIO_OPEN_OOM:
			g_fake_file_ctrl_seq[0].next_open_status = OSAL_FILE_STATUS_OOM;
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_OPEN_FAILS_WITH_PERM_ERROR:
			g_fake_file_ctrl_seq[0].next_open_status = OSAL_FILE_STATUS_PERM;
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_NULL:
			args.path = NULL;
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_EMPTY:
			args.path = "";
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_TOO_LONG:
			for (size_t i = 0; i < 256; i++) { too_long_path_storage[i] = '\a'; }
			too_long_path_storage[256] = '\0';
			args.path = too_long_path_storage;
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_OK_FILE_INJECTED_BY_TEST_INFRA:
			fs_stream_inject_file(fs_stream, fake_file_to_osal_file(&fake_file_injected_by_test_infra));
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_OK_FILE_CREATED_BY_DEFAULT_INIT:
		case FS_STREAM_T_LIFECYCLE_SCENARIO_CLOSE_FAILS_WITH_IO_ERROR:
			break;
		default: fail();
	}
	const osal_file_ops_t *file_ops_snapshot = fs_stream_get_file_ops(fs_stream);
	const osal_mem_ops_t *mem_ops_snapshot = fs_stream_get_mem_ops(fs_stream);

	// ACT
	ret = fs_stream_complete_default_init(fs_stream, &fs_stream_cfg, &args);

	// ASSERT
	assert_ptr_equal(fs_stream_get_file_ops(fs_stream), file_ops_snapshot);
	assert_ptr_equal(fs_stream_get_mem_ops(fs_stream), mem_ops_snapshot);
	switch (tc->scenario) {
		case FS_STREAM_T_LIFECYCLE_SCENARIO_OPEN_OOM:
			assert_int_equal(ret, FS_STREAM_STATUS_OOM);
			assert_null(fs_stream_get_file(fs_stream));
			assert_true(g_fake_file_ctrl_seq[0].open_call_count == 1);
			assert_non_null(g_fake_file_ctrl_seq[0].last_open_out);
			assert_ptr_equal(g_fake_file_ctrl_seq[0].last_open_pathname, args.path);
			assert_ptr_equal(g_fake_file_ctrl_seq[0].last_open_mode, args.mode);
			assert_ptr_equal(g_fake_file_ctrl_seq[0].last_open_mem_ops, fs_stream_get_mem_ops(fs_stream));
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_OPEN_FAILS_WITH_PERM_ERROR:
			assert_int_equal(ret, FS_STREAM_STATUS_IO_ERROR);
			assert_null(fs_stream_get_file(fs_stream));
			assert_true(g_fake_file_ctrl_seq[0].open_call_count == 1);
			assert_non_null(g_fake_file_ctrl_seq[0].last_open_out);
			assert_ptr_equal(g_fake_file_ctrl_seq[0].last_open_pathname, args.path);
			assert_ptr_equal(g_fake_file_ctrl_seq[0].last_open_mode, args.mode);
			assert_ptr_equal(g_fake_file_ctrl_seq[0].last_open_mem_ops, fs_stream_get_mem_ops(fs_stream));
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_NULL:
		case FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_EMPTY:
		case FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_TOO_LONG:
			assert_int_equal(ret, FS_STREAM_STATUS_INVALID);
			assert_null(fs_stream_get_file(fs_stream));
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_OK_FILE_INJECTED_BY_TEST_INFRA:
			assert_int_equal(ret, FS_STREAM_STATUS_OK);
			assert_ptr_equal(fs_stream_get_file(fs_stream), &fake_file_injected_by_test_infra);
			assert_true(g_fake_file_ctrl_seq[0].open_call_count == 0);
			break;
		case FS_STREAM_T_LIFECYCLE_SCENARIO_CLOSE_FAILS_WITH_IO_ERROR: // fall through ; handled later
		case FS_STREAM_T_LIFECYCLE_SCENARIO_OK_FILE_CREATED_BY_DEFAULT_INIT:
			assert_int_equal(ret, FS_STREAM_STATUS_OK);
			assert_non_null(fs_stream_get_file(fs_stream));
			assert_ptr_equal(fs_stream_get_file(fs_stream), fake_file_to_osal_file(&fake_file_default_init));
			assert_true(g_fake_file_ctrl_seq[0].open_call_count == 1);
			assert_non_null(g_fake_file_ctrl_seq[0].last_open_out);
			assert_ptr_equal(g_fake_file_ctrl_seq[0].last_open_pathname, args.path);
			assert_ptr_equal(g_fake_file_ctrl_seq[0].last_open_mode, args.mode);
			assert_ptr_equal(g_fake_file_ctrl_seq[0].last_open_mem_ops, fs_stream_get_mem_ops(fs_stream));
			break;
		default: fail();
	}
	assert_ptr_equal(fs_stream_get_file_ops(fs_stream), file_ops_snapshot);
	assert_ptr_equal(fs_stream_get_mem_ops(fs_stream), mem_ops_snapshot);

	// ARRANGE
	OSAL_FILE *file_snapshot = fs_stream_get_file(fs_stream);
	fake_file_t *fake_file_snapshot = osal_file_to_fake_file(file_snapshot);
	if (tc->scenario == FS_STREAM_T_LIFECYCLE_SCENARIO_CLOSE_FAILS_WITH_IO_ERROR) {
		fake_file_snapshot->next_close_status = OSAL_FILE_STATUS_IO;
	}

	// ACT
	ret = fs_stream_vtbl()->close(fs_stream);

	// ASSERT
	if (tc->scenario == FS_STREAM_T_LIFECYCLE_SCENARIO_CLOSE_FAILS_WITH_IO_ERROR) {
		assert_int_equal(ret, STREAM_STATUS_IO_ERROR);
	} else {
		assert_int_equal(ret, STREAM_STATUS_OK);
	}
	if (file_snapshot) {
		assert_int_equal(fake_file_snapshot->close_call_count, 1);
		assert_ptr_equal(fake_file_snapshot->last_close_stream, file_snapshot);
	}
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_fs_stream_t_lifecycle_case_t CASE_FS_STREAM_T_LIFECYCLE_FS_STREAM_HANDLE_OOM = {
	.name = "fs_stream_t_lifecycle_fs_stream_handle_oom",
	.scenario = FS_STREAM_T_LIFECYCLE_SCENARIO_FS_STREAM_HANDLE_OOM,
};

static const test_fs_stream_t_lifecycle_case_t CASE_FS_STREAM_T_LIFECYCLE_OPEN_OOM = {
	.name = "fs_stream_t_lifecycle_open_oom",
	.scenario = FS_STREAM_T_LIFECYCLE_SCENARIO_OPEN_OOM,
};

static const test_fs_stream_t_lifecycle_case_t CASE_FS_STREAM_T_LIFECYCLE_OPEN_FAILS_WITH_PERM_ERROR = {
	.name = "fs_stream_t_lifecycle_open_fails_with_perm_error",
	.scenario = FS_STREAM_T_LIFECYCLE_SCENARIO_OPEN_FAILS_WITH_PERM_ERROR,
};

static const test_fs_stream_t_lifecycle_case_t CASE_FS_STREAM_T_LIFECYCLE_PATH_NULL = {
	.name = "fs_stream_t_lifecycle_path_null",
	.scenario = FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_NULL,
};

static const test_fs_stream_t_lifecycle_case_t CASE_FS_STREAM_T_LIFECYCLE_PATH_EMPTY = {
	.name = "fs_stream_t_lifecycle_path_empty",
	.scenario = FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_EMPTY,
};

static const test_fs_stream_t_lifecycle_case_t CASE_FS_STREAM_T_LIFECYCLE_PATH_TOO_LONG = {
	.name = "fs_stream_t_lifecycle_path_too_long",
	.scenario = FS_STREAM_T_LIFECYCLE_SCENARIO_PATH_TOO_LONG,
};

static const test_fs_stream_t_lifecycle_case_t CASE_FS_STREAM_T_LIFECYCLE_FILE_INJECTED_BY_TEST_INFRA = {
	.name = "fs_stream_t_lifecycle_file_injected_by_test_infra",
	.scenario = FS_STREAM_T_LIFECYCLE_SCENARIO_OK_FILE_INJECTED_BY_TEST_INFRA,
};

static const test_fs_stream_t_lifecycle_case_t CASE_FS_STREAM_T_LIFECYCLE_FILE_CREATED_BY_DEFAULT_INIT = {
	.name = "fs_stream_t_lifecycle_file_created_by_default_init",
	.scenario = FS_STREAM_T_LIFECYCLE_SCENARIO_OK_FILE_CREATED_BY_DEFAULT_INIT,
};

static const test_fs_stream_t_lifecycle_case_t CASE_FS_STREAM_T_LIFECYCLE_CLOSE_FAILS_WITH_IO_ERROR = {
	.name = "fs_stream_t_lifecycle_close_fails_with_io_error",
	.scenario = FS_STREAM_T_LIFECYCLE_SCENARIO_CLOSE_FAILS_WITH_IO_ERROR,
};

#define FS_STREAM_T_LIFECYCLE_CASES(X) \
X(CASE_FS_STREAM_T_LIFECYCLE_FS_STREAM_HANDLE_OOM) \
X(CASE_FS_STREAM_T_LIFECYCLE_OPEN_OOM) \
X(CASE_FS_STREAM_T_LIFECYCLE_OPEN_FAILS_WITH_PERM_ERROR) \
X(CASE_FS_STREAM_T_LIFECYCLE_PATH_NULL) \
X(CASE_FS_STREAM_T_LIFECYCLE_PATH_EMPTY) \
X(CASE_FS_STREAM_T_LIFECYCLE_PATH_TOO_LONG) \
X(CASE_FS_STREAM_T_LIFECYCLE_FILE_INJECTED_BY_TEST_INFRA) \
X(CASE_FS_STREAM_T_LIFECYCLE_FILE_CREATED_BY_DEFAULT_INIT) \
X(CASE_FS_STREAM_T_LIFECYCLE_CLOSE_FAILS_WITH_IO_ERROR)

#define MAKE_FS_STREAM_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_t_lifecycle, case_sym)

static const struct CMUnitTest fs_stream_t_lifecycle_tests[] = {
	FS_STREAM_T_LIFECYCLE_CASES(MAKE_FS_STREAM_T_LIFECYCLE_TEST)
};

#undef FS_STREAM_T_LIFECYCLE_CASES
#undef MAKE_FS_STREAM_T_LIFECYCLE_TEST

/** @endcond */

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_adapter_provider_t` lifecycle.
 *
 * See contract:
 * - @ref specifications_fs_stream_create_adapter_provider "fs_stream_create_adapter_provider".
 * - @ref specifications_stream_destroy_adapter_provider "stream_destroy_adapter_provider() specifications"
 *
 * See test description:
 * - @ref testing_foundation_fs_stream_unit_stream_adapter_provider_t_lifecycle "`stream_adapter_provider_t` lifecycle unit tests section"
 */
typedef enum {
	FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM = 0,
	FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OK
} fs_stream_stream_adapter_provider_t_lifecycle_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	fs_stream_stream_adapter_provider_t_lifecycle_scenario_t scenario;
} test_fs_stream_stream_adapter_provider_t_lifecycle_case_t;

typedef struct {
	const test_fs_stream_stream_adapter_provider_t_lifecycle_case_t *tc;
} test_fs_stream_stream_adapter_provider_t_lifecycle_fixture_t;

static int setup_fs_stream_stream_adapter_provider_t_lifecycle(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(fs_stream_stream_adapter_provider_t_lifecycle, state, tc, fx);

	fake_memory_reset();
	fake_file_reset();

	*state = fx;
	return 0;
}

static int teardown_fs_stream_stream_adapter_provider_t_lifecycle(void **state)
{
	test_fs_stream_stream_adapter_provider_t_lifecycle_fixture_t *fx = (test_fs_stream_stream_adapter_provider_t_lifecycle_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_fs_stream_stream_adapter_provider_t_lifecycle(void **state)
{
	test_fs_stream_stream_adapter_provider_t_lifecycle_fixture_t *fx = (test_fs_stream_stream_adapter_provider_t_lifecycle_fixture_t *)(*state);
	const test_fs_stream_stream_adapter_provider_t_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	stream_adapter_provider_t *sentinel = (stream_adapter_provider_t *)(uintptr_t)0xDEADC0DEu;
	stream_adapter_provider_t *stream_adapter_provider = sentinel;
	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	const fs_stream_env_t fs_stream_env = {
		.file_ops = osal_file_test_fake_ops(),
		.mem_ops = osal_mem_test_fake_ops()
	};
	if (tc->scenario == FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM) { fake_memory_fail_only_on_call(1); }

	// ACT
	fs_stream_status_t ret = fs_stream_create_adapter_provider(&stream_adapter_provider, &fs_stream_cfg, &fs_stream_env);

	// ASSERT
	if (tc->scenario == FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM) {
		assert_int_equal(ret, FS_STREAM_STATUS_OOM);
		assert_ptr_equal(stream_adapter_provider, sentinel);
		assert_true(fake_memory_no_leak());
		assert_true(fake_memory_no_invalid_free());
		assert_true(fake_memory_no_double_free());
		return;
	}
	assert_int_equal(ret, FS_STREAM_STATUS_OK);
	assert_true(stream_adapter_provider != sentinel);
	assert_non_null(stream_adapter_provider);
	assert_non_null(stream_adapter_provider->backend_ctor);
	assert_ptr_equal(stream_adapter_provider->vtbl, fs_stream_vtbl());
	assert_non_null(stream_adapter_provider->ud);
	assert_non_null(stream_adapter_provider->ud_dtor);
	assert_ptr_equal(stream_adapter_provider->mem, fs_stream_env.mem_ops);

	// ACT
	stream_destroy_adapter_provider(stream_adapter_provider);

	// ASSERT
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static const test_fs_stream_stream_adapter_provider_t_lifecycle_case_t CASE_FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OOM = {
	.name = "fs_stream_stream_adapter_provider_t_lifecycle_oom",
	.scenario = FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OOM,
};

static const test_fs_stream_stream_adapter_provider_t_lifecycle_case_t CASE_FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OK = {
	.name = "fs_stream_stream_adapter_provider_t_lifecycle_ok",
	.scenario = FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_SCENARIO_OK,
};

#define FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_CASES(X) \
X(CASE_FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OOM) \
X(CASE_FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_OK)

#define MAKE_FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(fs_stream_stream_adapter_provider_t_lifecycle, case_sym)

static const struct CMUnitTest fs_stream_stream_adapter_provider_t_lifecycle_tests[] = {
	FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_CASES(MAKE_FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_TEST)
};

#undef FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_CASES
#undef MAKE_FS_STREAM_STREAM_ADAPTER_PROVIDER_T_LIFECYCLE_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest fs_stream_cr_tests_non_parametric[] = {
		cmocka_unit_test(test_fs_stream_vtbl),
		cmocka_unit_test(test_fs_stream_default_cfg),
		cmocka_unit_test(test_fs_stream_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(fs_stream_cr_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(fs_stream_t_lifecycle_tests, NULL, NULL);
	failed += cmocka_run_group_tests(fs_stream_stream_adapter_provider_t_lifecycle_tests, NULL, NULL);

	return failed;
}
/** @endcond */
