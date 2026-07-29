/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_stream_fs_stream_osal_file_doubled.c
 * @ingroup stream_integration_tests
 * @brief Integration tests for `stream` with the `fs_stream` adapter and a doubled OSAL file dependency.
 *
 * See also:
 * - @ref testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled
 *   "`stream` / `fs_stream` integration tests with doubled OSAL file dependency page"
 * - @ref specifications_stream "stream specifications"
 * - @ref specifications_fs_stream "fs_stream specifications"
 */

#include "fs_stream/cr/fs_stream_cr_api.h"
#include "stream/cr/stream_cr_api.h"
#include "stream/borrowers/stream_borrowers_api.h"
#include "stream/owners/stream_owners_api.h"

#include "fs_stream/tests/fs_stream_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/file/test/osal_file_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/******************************************************************************************************************************************
 * @brief Smoke test for a `stream_t` backed by the `fs_stream` adapter.
 *
 * See contracts:
 * - @ref specifications_fs_stream_create_adapter_provider "fs_stream_create_adapter_provider() specifications".
 * - @ref specifications_stream_create_factory "stream_create_factory() specifications".
 * - @ref specifications_stream_factory_add_adapter "stream_factory_add_adapter() specifications".
 * - @ref specifications_stream_create_regular_file_creator "stream_create_regular_file_creator() specifications".
 * - @ref specifications_stream_regular_file_creator_create "stream_regular_file_creator_create() specifications".
 * - @ref specifications_stream_read "stream_read() specifications".
 * - @ref specifications_stream_destroy "stream_destroy() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled_smoke "`stream` / `fs_stream` smoke tests section".
 */
static void test_stream_fs_stream_smoke(void **state) {
	(void)state;

	// ARRANGE

	fake_file_t fake_file = {0};
	fake_file_init_instance(&fake_file);
	const uint8_t fake_file_content[FAKE_FILE_BUF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
	size_t fake_file_content_len = 6;
	fake_file_set_sink_backing(&fake_file, fake_file_content, fake_file_content_len);

	fake_file_reset();
	const osal_file_ops_t *fake_file_ops = osal_file_test_fake_ops();
	g_fake_file_ctrl_seq[0].next_open_out = &fake_file;

	const osal_mem_ops_t *mem_ops = osal_mem_default_ops();

	stream_adapter_id_t fs_stream_id = "fs";

	stream_adapter_provider_t *fs_stream_adapter_provider = NULL;
	stream_factory_t *stream_factory = NULL;
	stream_regular_file_creator_t *stream_regular_file_creator = NULL;
	const char *dummy_fake_file_path = "./dummy/path/of/fake/file";
	const char *fake_file_mode = "rb";

	stream_t *stream_backed_by_fs_stream = NULL;

	size_t stream_read_ret = 0;
	uint8_t buf_read_arg[FAKE_FILE_BUF_SIZE] = {0};
	size_t n_read_arg_exceeding_fake_file_content_len = 256;
	uint8_t expected_read_buffer_content[FAKE_FILE_BUF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

	fs_stream_status_t fs_stream_status = FS_STREAM_STATUS_OK;
	stream_factory_status_t stream_factory_status = STREAM_FACTORY_STATUS_OK;
	stream_status_t stream_status = STREAM_STATUS_OK;


	// ACT and ASSERT

	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	fs_stream_env_t fs_stream_env = fs_stream_default_env(fake_file_ops, mem_ops);
	fs_stream_status = fs_stream_create_adapter_provider(&fs_stream_adapter_provider, &fs_stream_cfg, &fs_stream_env);
	assert_int_equal(fs_stream_status, FS_STREAM_STATUS_OK);

	stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
	stream_factory_status = stream_create_factory(&stream_factory, &stream_factory_cfg, mem_ops);
	assert_int_equal(stream_factory_status, STREAM_FACTORY_STATUS_OK);
	stream_factory_status = stream_factory_add_adapter(stream_factory, fs_stream_id, fs_stream_adapter_provider);
	assert_int_equal(stream_factory_status, STREAM_FACTORY_STATUS_OK);

	stream_factory_status =
		stream_create_regular_file_creator(
			&stream_regular_file_creator,
			stream_factory,
			fs_stream_id,
			mem_ops
		);
	assert_int_equal(stream_factory_status, STREAM_FACTORY_STATUS_OK);

	stream_status =
		stream_regular_file_creator_create(
			stream_regular_file_creator,
			dummy_fake_file_path,
			fake_file_mode,
			&stream_backed_by_fs_stream
		);
	assert_int_equal(stream_status, STREAM_STATUS_OK);
	assert_true(
		   g_fake_file_ctrl_seq[0].open_call_count == 1
		&& g_fake_file_ctrl_seq[0].last_open_pathname == dummy_fake_file_path
		&& g_fake_file_ctrl_seq[0].last_open_mode == fake_file_mode
		&& g_fake_file_ctrl_seq[0].last_open_mem_ops == mem_ops
	);

	stream_read_ret =
		stream_read(
			stream_backed_by_fs_stream,
			buf_read_arg,
			n_read_arg_exceeding_fake_file_content_len,
			&stream_status
		);
	assert_int_equal(stream_status, STREAM_STATUS_EOF);
	assert_true(stream_read_ret == 6);
	assert_true(fake_file.read_call_count == 1);
	assert_ptr_equal(fake_file.last_read_ptr, buf_read_arg);
	assert_true(fake_file.last_read_size == 1);
	assert_true(fake_file.last_read_nmemb == n_read_arg_exceeding_fake_file_content_len);
	assert_ptr_equal(fake_file.last_read_stream, fake_file_to_osal_file(&fake_file));
	assert_memory_equal(buf_read_arg, expected_read_buffer_content, FAKE_FILE_BUF_SIZE);

	stream_destroy(&stream_backed_by_fs_stream);
	assert_true(
		   fake_file.close_call_count == 1
		&& fake_file.last_close_stream == fake_file_to_osal_file(&fake_file)
	);
	assert_null(stream_backed_by_fs_stream);

	stream_destroy_regular_file_creator(&stream_regular_file_creator);
	stream_destroy_factory(&stream_factory);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_read()` with a `stream_t` backed by the `fs_stream` adapter.
 *
 * See contracts:
 * - @ref specifications_stream_read "stream_read() specifications".
 * - @ref specifications_fs_stream_read "fs_stream_read() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled_read "`stream_read()` / `fs_stream` integration tests section".
 */
typedef enum {
	STREAM_READ_FS_STREAM_SCENARIO_N_NONZERO_S_NULL = 0,
	STREAM_READ_FS_STREAM_SCENARIO_N_NONZERO_BUF_NULL,
	STREAM_READ_FS_STREAM_SCENARIO_IO_ERROR,
	STREAM_READ_FS_STREAM_SCENARIO_OK_N_ZERO_S_NULL,
	STREAM_READ_FS_STREAM_SCENARIO_OK_N_EXCEEDING,
	STREAM_READ_FS_STREAM_SCENARIO_OK_N_FIT,
	STREAM_READ_FS_STREAM_SCENARIO_OK_NO_MORE_TO_READ,
	STREAM_READ_FS_STREAM_SCENARIO_OK_STILL_TO_READ,
	STREAM_READ_FS_STREAM_SCENARIO_OK_TWO_READS_STILL_TO_READ,
	STREAM_READ_FS_STREAM_SCENARIO_OK_TWO_READS_EXACTLY_TO_EOF,
	STREAM_READ_FS_STREAM_SCENARIO_OK_TWO_READS_SECOND_EXCEEDING,
	STREAM_READ_FS_STREAM_SCENARIO_OK_TWO_READS_SECOND_AT_EOF,
} stream_read_fs_stream_scenario_t;

/** @cond INTERNAL */

typedef struct {

	const char *name;
	stream_read_fs_stream_scenario_t scenario;

	const uint8_t fake_file_content[FAKE_FILE_BUF_SIZE];
	size_t fake_file_buffered_len;
	size_t fake_file_initial_pos;
	osal_file_status_t fake_file_next_read_status;

	bool arg_s_is_null;
	bool arg_buf_is_null;
	const uint8_t initial_arg_buf_content[FAKE_FILE_BUF_SIZE];
	size_t first_read_arg_n;
	stream_status_t initial_arg_st_content;

	size_t expected_first_read_ret;
	uint8_t expected_buf_after_first_read[FAKE_FILE_BUF_SIZE];
	stream_status_t expected_stream_status_after_first_read;

	bool second_read;
	size_t second_read_arg_n;

	size_t expected_second_read_ret;
	uint8_t expected_buf_after_second_read[FAKE_FILE_BUF_SIZE];
	stream_status_t expected_stream_status_after_second_read;

} test_stream_read_fs_stream_case_t;

typedef struct {
	fake_file_t fake_file;
	stream_t *stream;
	uint8_t arg_buf[FAKE_FILE_BUF_SIZE];
	stream_status_t arg_st_content;

	const test_stream_read_fs_stream_case_t *tc;
} test_stream_read_fs_stream_fixture_t;

static int setup_stream_read_fs_stream(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_read_fs_stream, state, tc, fx);

	fake_file_reset();
	fake_file_init_instance(&fx->fake_file);
	const osal_file_ops_t *fake_file_ops = osal_file_test_fake_ops();
	const osal_mem_ops_t *mem_ops = osal_mem_default_ops();
	fs_stream_t *fs_stream = NULL;
	fs_stream_env_t fs_stream_env = fs_stream_default_env(fake_file_ops, mem_ops);
	assert_int_equal(fs_stream_create(&fs_stream, &fs_stream_env), FS_STREAM_STATUS_OK);
	fs_stream_inject_file(fs_stream, fake_file_to_osal_file(&fx->fake_file));
	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	stream_regular_file_creator_args_t stream_regular_file_creator_args = { .path = "./dummy/path/of/fake/file", .mode = "rb" };
	assert_int_equal(fs_stream_complete_default_init(fs_stream, &fs_stream_cfg, &stream_regular_file_creator_args), FS_STREAM_STATUS_OK);
	const stream_env_t stream_env = stream_default_env(fs_stream_vtbl(), mem_ops);
	assert_int_equal(stream_create(&fx->stream, &stream_env), STREAM_STATUS_OK);
	assert_int_equal(stream_complete_default_init(fx->stream, (void *)fs_stream), STREAM_STATUS_OK);
	osal_memcpy(fx->arg_buf, tc->initial_arg_buf_content, FAKE_FILE_BUF_SIZE);
	fx->arg_st_content = tc->initial_arg_st_content;

	*state = fx;
	return 0;
}

static int teardown_stream_read_fs_stream(void **state)
{
	test_stream_read_fs_stream_fixture_t *fx = (test_stream_read_fs_stream_fixture_t *)(*state);
	stream_destroy(&fx->stream);
	osal_free(fx);
	return 0;
}

static void test_stream_read_fs_stream(void **state) {
	test_stream_read_fs_stream_fixture_t *fx = (test_stream_read_fs_stream_fixture_t *)(*state);
	const test_stream_read_fs_stream_case_t *tc = fx->tc;

	// ARRANGE
	fake_file_set_buffered_backing(&fx->fake_file, tc->fake_file_content, tc->fake_file_buffered_len);
	fake_file_set_pos(&fx->fake_file, tc->fake_file_initial_pos);
	size_t ret = 0;
	stream_t *arg_s = fx->stream;
	if (tc->arg_s_is_null) {
		arg_s = NULL;
	}
	void *arg_buf = fx->arg_buf;
	if (tc->arg_buf_is_null) {
		arg_buf = NULL;
	}
	fx->fake_file.next_read_status = tc->fake_file_next_read_status;

	// ACT
	ret = stream_read(arg_s, arg_buf, tc->first_read_arg_n, &fx->arg_st_content);

	// ASSERT
	assert_int_equal(ret, tc->expected_first_read_ret);
	assert_int_equal(fx->arg_st_content, tc->expected_stream_status_after_first_read);
	assert_memory_equal(fx->arg_buf, tc->expected_buf_after_first_read, FAKE_FILE_BUF_SIZE);

	// ARRANGE
	if (!tc->second_read) {
		return;
	}
	osal_memset(fx->arg_buf, 0, FAKE_FILE_BUF_SIZE);

	// ACT
	ret = stream_read(fx->stream, fx->arg_buf, tc->second_read_arg_n, &fx->arg_st_content);

	// ASSERT
	assert_int_equal(ret, tc->expected_second_read_ret);
	assert_int_equal(fx->arg_st_content, tc->expected_stream_status_after_second_read);
	assert_memory_equal(fx->arg_buf, tc->expected_buf_after_second_read, FAKE_FILE_BUF_SIZE);
}

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_FS_STREAM_N_NONZERO_S_NULL = {

	.name = "stream_read_fs_stream_n_non_zero_s_null",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_N_NONZERO_S_NULL,

	.fake_file_content ={0x0a, 0x0b, 0x0c},
	.fake_file_buffered_len = 3,
	.fake_file_initial_pos = 2,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = true,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 1,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_INVALID

};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_FS_STREAM_N_NONZERO_BUF_NULL = {

	.name = "stream_read_fs_stream_n_non_zero_buf_null",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_N_NONZERO_BUF_NULL,

	.fake_file_content ={0x0a, 0x0b, 0x0c},
	.fake_file_buffered_len = 3,
	.fake_file_initial_pos = 2,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = true,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 1,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_INVALID

};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_FS_STREAM_IO_ERROR = {

	.name = "stream_read_fs_stream_io_error",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_IO_ERROR,

	.fake_file_content ={0x0a, 0x0b, 0x0c},
	.fake_file_buffered_len = 3,
	.fake_file_initial_pos = 2,
	.fake_file_next_read_status = OSAL_FILE_STATUS_IO,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 1,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_IO_ERROR,

	.second_read = false

};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_FS_STREAM_N_ZERO_S_NULL = {

	.name = "stream_read_fs_stream_n_zero_s_null",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_OK_N_ZERO_S_NULL,

	.fake_file_content ={0x0a, 0x0b, 0x0c},
	.fake_file_buffered_len = 3,
	.fake_file_initial_pos = 2,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = true,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 0,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_OK_N_EXCEEDING = {

	.name = "stream_read_fs_stream_ok_n_exceeding",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_OK_N_EXCEEDING,

	.fake_file_content ={0x0a, 0x0b, 0x0c},
	.fake_file_buffered_len = 3,
	.fake_file_initial_pos = 2,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0},
	.first_read_arg_n = 2,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 1,
	.expected_buf_after_first_read = {0x0c},
	.expected_stream_status_after_first_read = STREAM_STATUS_EOF,

	.second_read = false

};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_OK_N_FIT = {

	.name = "stream_read_fs_stream_ok_n_fit",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_OK_N_FIT,

	.fake_file_content ={0x0a, 0x0b, 0x0c},
	.fake_file_buffered_len = 3,
	.fake_file_initial_pos = 1,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0},
	.first_read_arg_n = 2,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 2,
	.expected_buf_after_first_read = {0x0b, 0x0c},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = false

};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_OK_NO_MORE_TO_READ = {

	.name = "stream_read_fs_stream_ok_no_more_to_read",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_OK_NO_MORE_TO_READ,

	.fake_file_content ={0x0a, 0x0b, 0x0c},
	.fake_file_buffered_len = 3,
	.fake_file_initial_pos = 3,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 7,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_EOF,

	.second_read = false

};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_OK_STILL_TO_READ = {

	.name = "stream_read_fs_stream_ok_still_to_read",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_OK_STILL_TO_READ,

	.fake_file_content ={0x0a, 0x0b, 0x0c},
	.fake_file_buffered_len = 3,
	.fake_file_initial_pos = 0,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0},
	.first_read_arg_n = 2,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 2,
	.expected_buf_after_first_read = {0x0a, 0x0b},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = false

};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_OK_TWO_READS_STILL_TO_READ = {

	.name = "stream_read_fs_stream_ok_two_reads_still_to_read",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_OK_TWO_READS_STILL_TO_READ,

	.fake_file_content ={0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_buffered_len = 6,
	.fake_file_initial_pos = 1,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0},
	.first_read_arg_n = 2,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 2,
	.expected_buf_after_first_read = {0x0b, 0x0c},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = true,
	.second_read_arg_n = 2,

	.expected_second_read_ret = 2,
	.expected_buf_after_second_read = {0x0d, 0x0e},
	.expected_stream_status_after_second_read = STREAM_STATUS_OK
};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_OK_TWO_READS_EXACTLY_TO_EOF = {

	.name = "stream_read_fs_stream_ok_two_reads_exactly_to_eof",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_OK_TWO_READS_EXACTLY_TO_EOF,

	.fake_file_content ={0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_buffered_len = 6,
	.fake_file_initial_pos = 1,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0},
	.first_read_arg_n = 2,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 2,
	.expected_buf_after_first_read = {0x0b, 0x0c},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = true,
	.second_read_arg_n = 3,

	.expected_second_read_ret = 3,
	.expected_buf_after_second_read = {0x0d, 0x0e, 0x0f},
	.expected_stream_status_after_second_read = STREAM_STATUS_OK
};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_OK_TWO_READS_SECOND_EXCEEDING = {

	.name = "stream_read_fs_stream_ok_two_reads_second_exceeding",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_OK_TWO_READS_SECOND_EXCEEDING,

	.fake_file_content ={0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_buffered_len = 6,
	.fake_file_initial_pos = 1,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0},
	.first_read_arg_n = 3,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 3,
	.expected_buf_after_first_read = {0x0b, 0x0c, 0x0d},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = true,
	.second_read_arg_n = 3,

	.expected_second_read_ret = 2,
	.expected_buf_after_second_read = {0x0e, 0x0f},
	.expected_stream_status_after_second_read = STREAM_STATUS_EOF
};

static const test_stream_read_fs_stream_case_t CASE_STREAM_READ_OK_TWO_READS_SECOND_AT_EOF = {

	.name = "stream_read_fs_stream_ok_two_reads_second_at_eof",
	.scenario = STREAM_READ_FS_STREAM_SCENARIO_OK_TWO_READS_SECOND_AT_EOF,

	.fake_file_content ={0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_buffered_len = 6,
	.fake_file_initial_pos = 1,
	.fake_file_next_read_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0},
	.first_read_arg_n = 5,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 5,
	.expected_buf_after_first_read = {0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = true,
	.second_read_arg_n = 7,

	.expected_second_read_ret = 0,
	.expected_buf_after_second_read = {0},
	.expected_stream_status_after_second_read = STREAM_STATUS_EOF
};

#define STREAM_READ_FS_STREAM_CASES(X) \
X(CASE_STREAM_READ_FS_STREAM_N_NONZERO_S_NULL) \
X(CASE_STREAM_READ_FS_STREAM_N_NONZERO_BUF_NULL) \
X(CASE_STREAM_READ_FS_STREAM_N_ZERO_S_NULL) \
X(CASE_STREAM_READ_FS_STREAM_IO_ERROR) \
X(CASE_STREAM_READ_OK_N_EXCEEDING) \
X(CASE_STREAM_READ_OK_N_FIT) \
X(CASE_STREAM_READ_OK_NO_MORE_TO_READ) \
X(CASE_STREAM_READ_OK_STILL_TO_READ) \
X(CASE_STREAM_READ_OK_TWO_READS_STILL_TO_READ) \
X(CASE_STREAM_READ_OK_TWO_READS_EXACTLY_TO_EOF) \
X(CASE_STREAM_READ_OK_TWO_READS_SECOND_EXCEEDING) \
X(CASE_STREAM_READ_OK_TWO_READS_SECOND_AT_EOF)

#define MAKE_STREAM_READ_FS_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_read_fs_stream, case_sym)

static const struct CMUnitTest stream_read_fs_stream_tests[] = {
	STREAM_READ_FS_STREAM_CASES(MAKE_STREAM_READ_FS_STREAM_TEST)
};

#undef STREAM_READ_FS_STREAM_CASES
#undef MAKE_STREAM_READ_FS_STREAM_TEST

/** @endcond */

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_write()` with a `stream_t` backed by the `fs_stream` adapter.
 *
 * See contracts:
 * - @ref specifications_stream_write "stream_write() specifications".
 * - @ref specifications_fs_stream_write "fs_stream_write() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled_write "`stream_write()` / `fs_stream` integration tests section".
 */
typedef enum {
	STREAM_WRITE_FS_STREAM_SCENARIO_N_NONZERO_S_NULL = 0,
	STREAM_WRITE_FS_STREAM_SCENARIO_N_NONZERO_BUF_NULL,
	STREAM_WRITE_FS_STREAM_SCENARIO_IO_ERROR,
	STREAM_WRITE_FS_STREAM_SCENARIO_OK_N_ZERO_S_NULL,
	STREAM_WRITE_FS_STREAM_SCENARIO_OK_ONE_WRITE,
	STREAM_WRITE_FS_STREAM_SCENARIO_OK_ONE_WRITE_EXCEEDING,
	STREAM_WRITE_FS_STREAM_SCENARIO_OK_TWO_WRITES
} stream_write_fs_stream_scenario_t;

typedef struct {

	const char *name;
	stream_write_fs_stream_scenario_t scenario;

	const uint8_t initial_fake_file_buffered_backing[FAKE_FILE_BUF_SIZE];
	size_t fake_file_initial_buffered_len;
	size_t fake_file_initial_pos;
	osal_file_status_t fake_file_next_write_status;

	bool arg_s_is_null;
	bool arg_buf_is_null;
	const uint8_t first_write_arg_buf[FAKE_FILE_BUF_SIZE];
	size_t first_write_arg_n;
	stream_status_t initial_arg_st_content;

	size_t expected_first_write_ret;
	uint8_t expected_fake_file_buffered_backing_after_first_write[FAKE_FILE_BUF_SIZE];
	size_t expected_fake_file_buffered_backing_len_after_first_write;
	stream_status_t expected_stream_status_after_first_write;

	bool second_write;
	const uint8_t second_write_arg_buf[FAKE_FILE_BUF_SIZE];
	size_t second_write_arg_n;

	size_t expected_second_write_ret;
	uint8_t expected_fake_file_buffered_backing_after_second_write[FAKE_FILE_BUF_SIZE];
	size_t expected_fake_file_buffered_backing_len_after_second_write;
	stream_status_t expected_stream_status_after_second_write;

} test_stream_write_fs_stream_case_t;

typedef struct {
	fake_file_t fake_file;
	stream_t *stream;
	stream_status_t arg_st_content;

	const test_stream_write_fs_stream_case_t *tc;
} test_stream_write_fs_stream_fixture_t;

static int setup_stream_write_fs_stream(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_write_fs_stream, state, tc, fx);

	fake_file_reset();
	fake_file_init_instance(&fx->fake_file);
	const osal_file_ops_t *fake_file_ops = osal_file_test_fake_ops();
	const osal_mem_ops_t *mem_ops = osal_mem_default_ops();
	fs_stream_t *fs_stream = NULL;
	fs_stream_env_t fs_stream_env = fs_stream_default_env(fake_file_ops, mem_ops);
	assert_int_equal(fs_stream_create(&fs_stream, &fs_stream_env), FS_STREAM_STATUS_OK);
	fs_stream_inject_file(fs_stream, fake_file_to_osal_file(&fx->fake_file));
	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	stream_regular_file_creator_args_t stream_regular_file_creator_args = { .path = "./dummy/path/of/fake/file", .mode = "wb" };
	assert_int_equal(fs_stream_complete_default_init(fs_stream, &fs_stream_cfg, &stream_regular_file_creator_args), FS_STREAM_STATUS_OK);
	const stream_env_t stream_env = stream_default_env(fs_stream_vtbl(), mem_ops);
	assert_int_equal(stream_create(&fx->stream, &stream_env), STREAM_STATUS_OK);
	assert_int_equal(stream_complete_default_init(fx->stream, (void *)fs_stream), STREAM_STATUS_OK);
	fx->arg_st_content = tc->initial_arg_st_content;

	*state = fx;
	return 0;
}

static int teardown_stream_write_fs_stream(void **state)
{
	test_stream_write_fs_stream_fixture_t *fx = (test_stream_write_fs_stream_fixture_t *)(*state);
	stream_destroy(&fx->stream);
	osal_free(fx);
	return 0;
}

static void test_stream_write_fs_stream(void **state) {
	test_stream_write_fs_stream_fixture_t *fx = (test_stream_write_fs_stream_fixture_t *)(*state);
	const test_stream_write_fs_stream_case_t *tc = fx->tc;

	// ARRANGE
	fake_file_set_buffered_backing(&fx->fake_file, tc->initial_fake_file_buffered_backing, tc->fake_file_initial_buffered_len);
	fake_file_set_pos(&fx->fake_file, tc->fake_file_initial_pos);
	size_t ret = 0;
	stream_t *arg_s = fx->stream;
	if (tc->arg_s_is_null) {
		arg_s = NULL;
	}
	const void *arg_buf = tc->first_write_arg_buf;
	if (tc->arg_buf_is_null) {
		arg_buf = NULL;
	}
	fx->fake_file.next_write_status = tc->fake_file_next_write_status;

	// ACT
	ret = stream_write(arg_s, arg_buf, tc->first_write_arg_n, &fx->arg_st_content);

	// ASSERT
	assert_int_equal(ret, tc->expected_first_write_ret);
	assert_int_equal(fx->arg_st_content, tc->expected_stream_status_after_first_write);
	assert_int_equal(fx->fake_file.buffered_len, tc->expected_fake_file_buffered_backing_len_after_first_write);
	assert_memory_equal(
		fx->fake_file.buffered_backing,
		tc->expected_fake_file_buffered_backing_after_first_write,
		tc->expected_fake_file_buffered_backing_len_after_first_write
	);

	// ARRANGE
	if (!tc->second_write) {
		return;
	}

	// ACT
	ret = stream_write(fx->stream, tc->second_write_arg_buf, tc->second_write_arg_n, &fx->arg_st_content);

	// ASSERT
	assert_int_equal(ret, tc->expected_second_write_ret);
	assert_int_equal(fx->arg_st_content, tc->expected_stream_status_after_second_write);
	assert_int_equal(fx->fake_file.buffered_len, tc->expected_fake_file_buffered_backing_len_after_second_write);
	assert_memory_equal(
		fx->fake_file.buffered_backing,
		tc->expected_fake_file_buffered_backing_after_second_write,
		tc->expected_fake_file_buffered_backing_len_after_second_write
	);
}

static const test_stream_write_fs_stream_case_t CASE_STREAM_WRITE_N_NONZERO_S_NULL = {

	.name = "stream_write_fs_stream_n_nonzero_s_null",
	.scenario = STREAM_WRITE_FS_STREAM_SCENARIO_N_NONZERO_S_NULL,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.fake_file_initial_pos = 2,
	.fake_file_next_write_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = true,
	.arg_buf_is_null = false,
	.first_write_arg_buf = {0x01, 0x02, 0x03, 0x04},
	.first_write_arg_n = 2,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_write_ret = 0,
	.expected_fake_file_buffered_backing_after_first_write = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.expected_fake_file_buffered_backing_len_after_first_write = 6,
	.expected_stream_status_after_first_write = STREAM_STATUS_INVALID,

	.second_write = false,

};

static const test_stream_write_fs_stream_case_t CASE_STREAM_WRITE_N_NONZERO_BUF_NULL = {

	.name = "stream_write_fs_stream_n_nonzero_buf_null",
	.scenario = STREAM_WRITE_FS_STREAM_SCENARIO_N_NONZERO_BUF_NULL,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.fake_file_initial_pos = 2,
	.fake_file_next_write_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = true,
	.first_write_arg_buf = {0x01, 0x02, 0x03, 0x04},
	.first_write_arg_n = 2,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_write_ret = 0,
	.expected_fake_file_buffered_backing_after_first_write = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.expected_fake_file_buffered_backing_len_after_first_write = 6,
	.expected_stream_status_after_first_write = STREAM_STATUS_INVALID,

	.second_write = false,

};

static const test_stream_write_fs_stream_case_t CASE_STREAM_WRITE_IO_ERROR = {

	.name = "stream_write_fs_stream_io_error",
	.scenario = STREAM_WRITE_FS_STREAM_SCENARIO_IO_ERROR,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.fake_file_initial_pos = 2,
	.fake_file_next_write_status = OSAL_FILE_STATUS_IO,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.first_write_arg_buf = {0x01, 0x02, 0x03, 0x04},
	.first_write_arg_n = 2,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_write_ret = 0,
	.expected_fake_file_buffered_backing_after_first_write = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.expected_fake_file_buffered_backing_len_after_first_write = 6,
	.expected_stream_status_after_first_write = STREAM_STATUS_IO_ERROR,

	.second_write = false,

};

static const test_stream_write_fs_stream_case_t CASE_STREAM_WRITE_OK_N_ZERO_S_NULL = {

	.name = "stream_write_fs_stream_ok_n_zero_s_null",
	.scenario = STREAM_WRITE_FS_STREAM_SCENARIO_OK_N_ZERO_S_NULL,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.fake_file_initial_pos = 2,
	.fake_file_next_write_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = true,
	.arg_buf_is_null = false,
	.first_write_arg_buf = {0x01, 0x02, 0x03, 0x04},
	.first_write_arg_n = 0,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_write_ret = 0,
	.expected_fake_file_buffered_backing_after_first_write = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.expected_fake_file_buffered_backing_len_after_first_write = 6,
	.expected_stream_status_after_first_write = STREAM_STATUS_OK,

	.second_write = false,

};

static const test_stream_write_fs_stream_case_t CASE_STREAM_WRITE_OK_ONE_WRITE = {

	.name = "stream_write_fs_stream_ok_one_write",
	.scenario = STREAM_WRITE_FS_STREAM_SCENARIO_OK_ONE_WRITE,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.fake_file_initial_pos = 2,
	.fake_file_next_write_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.first_write_arg_buf = {0x01, 0x02, 0x03, 0x04},
	.first_write_arg_n = 3,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_write_ret = 3,
	.expected_fake_file_buffered_backing_after_first_write = {0x0a, 0x0b, 0x01, 0x02, 0x03, 0x0f},
	.expected_fake_file_buffered_backing_len_after_first_write = 6,
	.expected_stream_status_after_first_write = STREAM_STATUS_OK,

	.second_write = false,

};

static const test_stream_write_fs_stream_case_t CASE_STREAM_WRITE_OK_ONE_WRITE_EXCEEDING = {

	.name = "stream_write_fs_stream_ok_one_write_exceeding",
	.scenario = STREAM_WRITE_FS_STREAM_SCENARIO_OK_ONE_WRITE_EXCEEDING,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.fake_file_initial_pos = 2,
	.fake_file_next_write_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.first_write_arg_buf = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
	.first_write_arg_n = 7,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_write_ret = 7,
	.expected_fake_file_buffered_backing_after_first_write = {0x0a, 0x0b, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
	.expected_fake_file_buffered_backing_len_after_first_write = 9,
	.expected_stream_status_after_first_write = STREAM_STATUS_OK,

	.second_write = false,

};

static const test_stream_write_fs_stream_case_t CASE_STREAM_WRITE_OK_TWO_WRITES = {

	.name = "stream_write_fs_stream_ok_two_writes",
	.scenario = STREAM_WRITE_FS_STREAM_SCENARIO_OK_TWO_WRITES,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.fake_file_initial_pos = 2,
	.fake_file_next_write_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.first_write_arg_buf = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
	.first_write_arg_n = 7,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_write_ret = 7,
	.expected_fake_file_buffered_backing_after_first_write = {0x0a, 0x0b, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
	.expected_fake_file_buffered_backing_len_after_first_write = 9,
	.expected_stream_status_after_first_write = STREAM_STATUS_OK,

	.second_write = true,
	.second_write_arg_buf = {0x08, 0x09},
	.second_write_arg_n = 2,

	.expected_second_write_ret = 2,
	.expected_fake_file_buffered_backing_after_second_write = {0x0a, 0x0b, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09},
	.expected_fake_file_buffered_backing_len_after_second_write = 11,
	.expected_stream_status_after_second_write = STREAM_STATUS_OK
};

#define STREAM_WRITE_FS_STREAM_CASES(X) \
X(CASE_STREAM_WRITE_N_NONZERO_S_NULL) \
X(CASE_STREAM_WRITE_N_NONZERO_BUF_NULL) \
X(CASE_STREAM_WRITE_IO_ERROR) \
X(CASE_STREAM_WRITE_OK_N_ZERO_S_NULL) \
X(CASE_STREAM_WRITE_OK_ONE_WRITE) \
X(CASE_STREAM_WRITE_OK_ONE_WRITE_EXCEEDING) \
X(CASE_STREAM_WRITE_OK_TWO_WRITES)

#define MAKE_STREAM_WRITE_FS_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_write_fs_stream, case_sym)

static const struct CMUnitTest stream_write_fs_stream_tests[] = {
	STREAM_WRITE_FS_STREAM_CASES(MAKE_STREAM_WRITE_FS_STREAM_TEST)
};

#undef STREAM_WRITE_FS_STREAM_CASES
#undef MAKE_STREAM_WRITE_FS_STREAM_TEST

/** @endcond */

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_flush()` with a `stream_t` backed by the `fs_stream` adapter.
 *
 * See contracts:
 * - @ref specifications_stream_flush "stream_flush() specifications".
 * - @ref specifications_fs_stream_flush "fs_stream_flush() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_integration_stream_fs_stream_osal_file_doubled_flush "`stream_flush()` / `fs_stream` integration tests section".
 */
typedef enum {
	STREAM_FLUSH_FS_STREAM_SCENARIO_S_NULL = 0,
	STREAM_FLUSH_FS_STREAM_SCENARIO_IO_ERROR,
	STREAM_FLUSH_FS_STREAM_SCENARIO_OK
} stream_flush_fs_stream_scenario_t;

typedef struct {

	const char *name;
	stream_flush_fs_stream_scenario_t scenario;

	const uint8_t initial_fake_file_buffered_backing[FAKE_FILE_BUF_SIZE];
	size_t fake_file_initial_buffered_len;
	const uint8_t initial_fake_file_sink_backing[FAKE_FILE_BUF_SIZE];
	size_t fake_file_initial_sink_len;
	osal_file_status_t fake_file_next_flush_status;

	bool arg_s_is_null;

	size_t expected_file_flush_call_count;
	stream_status_t expected_flush_ret;
	uint8_t expected_fake_file_buffered_backing_after_flush[FAKE_FILE_BUF_SIZE];
	size_t expected_fake_file_buffered_backing_len_after_flush;
	uint8_t expected_fake_file_sink_backing_after_flush[FAKE_FILE_BUF_SIZE];
	size_t expected_fake_file_sink_backing_len_after_flush;

} test_stream_flush_fs_stream_case_t;

typedef struct {
	fake_file_t fake_file;
	stream_t *stream;

	const test_stream_flush_fs_stream_case_t *tc;
} test_stream_flush_fs_stream_fixture_t;

static int setup_stream_flush_fs_stream(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_flush_fs_stream, state, tc, fx);

	fake_file_reset();
	fake_file_init_instance(&fx->fake_file);
	const osal_file_ops_t *fake_file_ops = osal_file_test_fake_ops();
	const osal_mem_ops_t *mem_ops = osal_mem_default_ops();
	fs_stream_t *fs_stream = NULL;
	fs_stream_env_t fs_stream_env = fs_stream_default_env(fake_file_ops, mem_ops);
	assert_int_equal(fs_stream_create(&fs_stream, &fs_stream_env), FS_STREAM_STATUS_OK);
	fs_stream_inject_file(fs_stream, fake_file_to_osal_file(&fx->fake_file));
	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	stream_regular_file_creator_args_t stream_regular_file_creator_args = { .path = "./dummy/path/of/fake/file", .mode = "wb" };
	assert_int_equal(fs_stream_complete_default_init(fs_stream, &fs_stream_cfg, &stream_regular_file_creator_args), FS_STREAM_STATUS_OK);
	const stream_env_t stream_env = stream_default_env(fs_stream_vtbl(), mem_ops);
	assert_int_equal(stream_create(&fx->stream, &stream_env), STREAM_STATUS_OK);
	assert_int_equal(stream_complete_default_init(fx->stream, (void *)fs_stream), STREAM_STATUS_OK);

	*state = fx;
	return 0;
}

static int teardown_stream_flush_fs_stream(void **state)
{
	test_stream_flush_fs_stream_fixture_t *fx = (test_stream_flush_fs_stream_fixture_t *)(*state);
	fx->fake_file.next_flush_status = OSAL_FILE_STATUS_OK;
	stream_destroy(&fx->stream);
	osal_free(fx);
	return 0;
}

static void test_stream_flush_fs_stream(void **state) {
	test_stream_flush_fs_stream_fixture_t *fx = (test_stream_flush_fs_stream_fixture_t *)(*state);
	const test_stream_flush_fs_stream_case_t *tc = fx->tc;

	// ARRANGE
	fake_file_set_sink_backing(&fx->fake_file, tc->initial_fake_file_sink_backing, tc->fake_file_initial_sink_len);
	fake_file_set_buffered_backing(&fx->fake_file, tc->initial_fake_file_buffered_backing, tc->fake_file_initial_buffered_len);
	stream_status_t ret = STREAM_STATUS_OOM;
	stream_t *arg_s = fx->stream;
	if (tc->arg_s_is_null) {
		arg_s = NULL;
	}
	fx->fake_file.next_flush_status = tc->fake_file_next_flush_status;

	// ACT
	ret = stream_flush(arg_s);

	// ASSERT
	assert_int_equal(fx->fake_file.flush_call_count, tc->expected_file_flush_call_count);
	assert_int_equal(ret, tc->expected_flush_ret);
	assert_int_equal(fx->fake_file.buffered_len, tc->expected_fake_file_buffered_backing_len_after_flush);
	assert_memory_equal(
		fx->fake_file.buffered_backing,
		tc->expected_fake_file_buffered_backing_after_flush,
		tc->expected_fake_file_buffered_backing_len_after_flush
	);
	assert_int_equal(fx->fake_file.sink_len, tc->expected_fake_file_sink_backing_len_after_flush);
	assert_memory_equal(
		fx->fake_file.sink_backing,
		tc->expected_fake_file_sink_backing_after_flush,
		tc->expected_fake_file_sink_backing_len_after_flush
	);

}

static const test_stream_flush_fs_stream_case_t CASE_STREAM_FLUSH_S_NULL = {

	.name = "stream_flush_fs_stream_s_null",
	.scenario = STREAM_FLUSH_FS_STREAM_SCENARIO_S_NULL,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.initial_fake_file_sink_backing = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
	.fake_file_initial_sink_len = 7,
	.fake_file_next_flush_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = true,

	.expected_file_flush_call_count = 0,
	.expected_flush_ret = STREAM_STATUS_INVALID,
	.expected_fake_file_buffered_backing_after_flush = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.expected_fake_file_buffered_backing_len_after_flush = 6,
	.expected_fake_file_sink_backing_after_flush = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
	.expected_fake_file_sink_backing_len_after_flush = 7

};

static const test_stream_flush_fs_stream_case_t CASE_STREAM_FLUSH_IO_ERROR = {

	.name = "stream_flush_fs_stream_io_error",
	.scenario = STREAM_FLUSH_FS_STREAM_SCENARIO_IO_ERROR,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.initial_fake_file_sink_backing = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
	.fake_file_initial_sink_len = 7,
	.fake_file_next_flush_status = OSAL_FILE_STATUS_IO,

	.arg_s_is_null = false,

	.expected_file_flush_call_count = 1,
	.expected_flush_ret = STREAM_STATUS_IO_ERROR,
	.expected_fake_file_buffered_backing_after_flush = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.expected_fake_file_buffered_backing_len_after_flush = 6,
	.expected_fake_file_sink_backing_after_flush = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
	.expected_fake_file_sink_backing_len_after_flush = 7

};

static const test_stream_flush_fs_stream_case_t CASE_STREAM_FLUSH_OK = {

	.name = "stream_flush_fs_stream_ok",
	.scenario = STREAM_FLUSH_FS_STREAM_SCENARIO_OK,

	.initial_fake_file_buffered_backing = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.fake_file_initial_buffered_len = 6,
	.initial_fake_file_sink_backing = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
	.fake_file_initial_sink_len = 7,
	.fake_file_next_flush_status = OSAL_FILE_STATUS_OK,

	.arg_s_is_null = false,

	.expected_file_flush_call_count = 1,
	.expected_flush_ret = STREAM_STATUS_OK,
	.expected_fake_file_buffered_backing_after_flush = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.expected_fake_file_buffered_backing_len_after_flush = 6,
	.expected_fake_file_sink_backing_after_flush = {0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
	.expected_fake_file_sink_backing_len_after_flush = 6

};

#define STREAM_FLUSH_FS_STREAM_CASES(X) \
X(CASE_STREAM_FLUSH_S_NULL) \
X(CASE_STREAM_FLUSH_IO_ERROR) \
X(CASE_STREAM_FLUSH_OK)

#define MAKE_STREAM_FLUSH_FS_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_flush_fs_stream, case_sym)

static const struct CMUnitTest stream_flush_fs_stream_tests[] = {
	STREAM_FLUSH_FS_STREAM_CASES(MAKE_STREAM_FLUSH_FS_STREAM_TEST)
};

#undef STREAM_FLUSH_FS_STREAM_CASES
#undef MAKE_STREAM_FLUSH_FS_STREAM_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest stream_fs_stream_integration_tests_non_parametric[] = {
		cmocka_unit_test(test_stream_fs_stream_smoke)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(stream_fs_stream_integration_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(stream_read_fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_write_fs_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_flush_fs_stream_tests, NULL, NULL);

	return failed;
}
/** @endcond */
