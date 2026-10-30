/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_stream_fs_stream_no_double.c
 * @ingroup stream_integration_tests
 * @brief Integration tests implementation for stream / fs_stream without test doubles.
 *
 * See contracts:
 * - @ref specifications_stream "stream specifications"
 * - @ref specifications_fs_stream "fs_stream specifications"
 *
 * See test description:
 * - @ref testing_foundation_stream_integration_stream_fs_stream_no_double "stream / fs_stream integration tests without test doubles page"
 */

#include "fs_stream/cr/fs_stream_cr_api.h"

#include "stream/borrowers/stream_borrowers_api.h"
#include "stream/owners/stream_owners_api.h"
#include "stream/cr/stream_cr_api.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/str/osal_str.h"

#include "lexleo_cmocka.h"

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream` / `fs_stream` integration without test doubles.
 *
 * See contracts:
 * - @ref specifications_stream "stream specifications".
 * - @ref specifications_fs_stream "fs_stream specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_integration_stream_fs_stream_no_double "`stream` / `fs_stream` integration tests without test doubles".
 */
typedef enum {
	STREAM_FS_STREAM_NO_DOUBLE_SCENARIO_CREATE_VIA_CREATOR_WRITE_FLUSH_READ = 0,
	STREAM_FS_STREAM_NO_DOUBLE_SCENARIO_CREATE_VIA_CR_WRITE_FLUSH_READ
} stream_fs_stream_no_double_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	stream_fs_stream_no_double_scenario_t scenario;
} test_stream_fs_stream_no_double_case_t;

static void test_stream_fs_stream_no_double(void **state) {
	const test_stream_fs_stream_no_double_case_t *tc = (const test_stream_fs_stream_no_double_case_t *)(*state);

	// output stream creation
	const osal_mem_ops_t *mem_ops = osal_mem_default_ops();
	assert_non_null(mem_ops);
	stream_env_t stream_env = stream_default_env(fs_stream_vtbl(), mem_ops);
	stream_t *stream = NULL;
	stream_factory_t *stream_factory = NULL;
	stream_regular_file_creator_t *stream_regular_file_creator = NULL;
	const char *path = "./file_path.txt";
	switch (tc->scenario) {
		case STREAM_FS_STREAM_NO_DOUBLE_SCENARIO_CREATE_VIA_CR_WRITE_FLUSH_READ: {
			assert_int_equal(stream_create(&stream, &stream_env), STREAM_STATUS_OK);
			fs_stream_env_t fs_stream_env = fs_stream_default_env(osal_file_default_ops(), mem_ops);
			fs_stream_t *fs_stream = NULL;
			assert_int_equal(fs_stream_create(&fs_stream, &fs_stream_env), FS_STREAM_STATUS_OK);
			fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
			const stream_regular_file_creator_args_t stream_regular_file_creator_args = {path, "wb"};
			assert_int_equal(fs_stream_complete_default_init(fs_stream, &fs_stream_cfg, &stream_regular_file_creator_args), FS_STREAM_STATUS_OK);
			assert_int_equal(stream_complete_default_init(stream,(void *)fs_stream), STREAM_STATUS_OK);
			break;
		}
		case STREAM_FS_STREAM_NO_DOUBLE_SCENARIO_CREATE_VIA_CREATOR_WRITE_FLUSH_READ: {
			stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
			assert_int_equal(stream_create_factory(&stream_factory, &stream_factory_cfg, mem_ops), STREAM_FACTORY_STATUS_OK);
			stream_adapter_provider_t *adapter_provider = NULL;
			fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
			fs_stream_env_t fs_stream_env = fs_stream_default_env(osal_file_default_ops(), mem_ops);
			assert_int_equal(fs_stream_create_adapter_provider(&adapter_provider, &fs_stream_cfg, &fs_stream_env), FS_STREAM_STATUS_OK);
			stream_adapter_id_t adapter_id = "fs";
			assert_int_equal(stream_factory_add_adapter(stream_factory, adapter_id, adapter_provider), STREAM_FACTORY_STATUS_OK);
			assert_int_equal(stream_create_regular_file_creator(&stream_regular_file_creator, stream_factory, adapter_id, mem_ops), STREAM_FACTORY_STATUS_OK);
			assert_int_equal(stream_regular_file_creator_create(stream_regular_file_creator, path, "wb", &stream), STREAM_STATUS_OK);
			break;
		}
		default: fail();
	}

	// ARRANGE write
	size_t ret = 0;
	stream_status_t st = STREAM_STATUS_OK;
	const char *message_to_be_written = "message";
	size_t message_to_be_written_len = osal_strlen(message_to_be_written) + 1;

	// ACT write
	ret = stream_write(stream, message_to_be_written, message_to_be_written_len, &st);

	// ASSERT write
	assert_int_equal(ret, message_to_be_written_len);
	assert_int_equal(st, STREAM_STATUS_OK);

	// ACT flush
	st = stream_flush(stream);

	// ASSERT flush
	assert_int_equal(st, STREAM_STATUS_OK);

	// output stream destruction
	stream_destroy(&stream);

	// input stream construction
	switch (tc->scenario) {
		case STREAM_FS_STREAM_NO_DOUBLE_SCENARIO_CREATE_VIA_CR_WRITE_FLUSH_READ: {
			assert_int_equal(stream_create(&stream, &stream_env), STREAM_STATUS_OK);
			fs_stream_env_t fs_stream_env = fs_stream_default_env(osal_file_default_ops(), mem_ops);
			fs_stream_t *fs_stream = NULL;
			assert_int_equal(fs_stream_create(&fs_stream, &fs_stream_env), FS_STREAM_STATUS_OK);
			fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
			const stream_regular_file_creator_args_t stream_regular_file_creator_args = {path, "rb"};
			assert_int_equal(fs_stream_complete_default_init(fs_stream, &fs_stream_cfg, &stream_regular_file_creator_args), FS_STREAM_STATUS_OK);
			assert_int_equal(stream_complete_default_init(stream,(void *)fs_stream), STREAM_STATUS_OK);
			break;
		}
		case STREAM_FS_STREAM_NO_DOUBLE_SCENARIO_CREATE_VIA_CREATOR_WRITE_FLUSH_READ: {
			assert_int_equal(stream_regular_file_creator_create(stream_regular_file_creator, path, "rb", &stream), STREAM_STATUS_OK);
			break;
		}
		default: fail();
	}

	// ARRANGE read
	char buf[256];

	// ACT read
	ret = stream_read(stream, buf, message_to_be_written_len, &st);

	// ASSERT read
	assert_int_equal(ret, message_to_be_written_len);
	assert_int_equal(st, STREAM_STATUS_OK);
	assert_memory_equal(buf, message_to_be_written, message_to_be_written_len);

	// input stream destruction
	stream_destroy(&stream);

	// creator and factory destruction
	stream_destroy_regular_file_creator(&stream_regular_file_creator);
	stream_destroy_factory(&stream_factory);
}

static const test_stream_fs_stream_no_double_case_t CASE_STREAM_FS_STREAM_NO_DOUBLE_CREATE_VIA_CREATOR_WRITE_FLUSH_READ = {
	.name = "stream_fs_stream_no_double_create_via_creator_write_flush_read",
	.scenario = STREAM_FS_STREAM_NO_DOUBLE_SCENARIO_CREATE_VIA_CREATOR_WRITE_FLUSH_READ,
};

static const test_stream_fs_stream_no_double_case_t CASE_STREAM_FS_STREAM_NO_DOUBLE_CREATE_VIA_CR_WRITE_FLUSH_READ = {
	.name = "stream_fs_stream_no_double_create_via_cr_write_flush_read",
	.scenario = STREAM_FS_STREAM_NO_DOUBLE_SCENARIO_CREATE_VIA_CR_WRITE_FLUSH_READ,
};

static const struct CMUnitTest stream_fs_stream_no_double_tests[] = {
	cmocka_unit_test_prestate(
		test_stream_fs_stream_no_double,
		(void*)&CASE_STREAM_FS_STREAM_NO_DOUBLE_CREATE_VIA_CREATOR_WRITE_FLUSH_READ
	),
	cmocka_unit_test_prestate(
		test_stream_fs_stream_no_double,
		(void*)&CASE_STREAM_FS_STREAM_NO_DOUBLE_CREATE_VIA_CR_WRITE_FLUSH_READ
	),
};

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {

	int failed = 0;
	failed += cmocka_run_group_tests(stream_fs_stream_no_double_tests, NULL, NULL);

	return failed;
}
/** @endcond */
