/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_lexleo_vm_cr_stream_creators.c
 * @ingroup lexleo_vm_integration_tests
 * @brief Integration tests for LexLeo VM stream creator wiring.
 *
 * @details
 * This file validates the Composition Root wiring responsible for creating,
 * using, and destroying the default stream factory and owner-facing stream
 * creators attached to LexLeo VM handles.
 *
 * See also:
 * - @ref specifications_lexleo_vm_complete_default_init
 * - @ref specifications_lexleo_vm_destroy
 * - @ref testing_lexleo_vm_cr_stream_creators_integration
 */

#include "lexleo_vm/cr/lexleo_vm_cr_api.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/stdio/osal_stdio_ops.h"
#include "osal/file/osal_file_ops.h"
#include "osal/str/osal_str_ops.h"
#include "osal/time/osal_time_ops.h"

#include "stream/owners/stream_owners_api.h"

#include "lexleo_vm/tests/lexleo_vm_white_box_tests_access.h"

#include "osal/str/osal_str.h"

#include "lexleo_cmocka.h"

/**
 * @brief Tests the default stream creator lifecycle.
 *
 * See contracts:
 * - @ref specifications_lexleo_vm_create
 * - @ref specifications_lexleo_vm_complete_default_init
 * - @ref specifications_lexleo_vm_destroy
 *
 * See test description:
 * - @ref testing_lexleo_vm_cr_stream_creators_integration_lifecycle
 */
static void test_stream_creators_lifecycle_ok(void **state)
{
	(void)state;

	/* SETUP */
	lexleo_vm_env_t vm_env =
		lexleo_vm_default_env(
			osal_mem_default_ops(),
			osal_stdio_default_ops(),
			osal_file_default_ops(),
			osal_str_default_ops(),
			osal_time_default_ops(),
			(stream_t*)0xDEADBEEF,
			(stream_t*)0xDEADBEEF,
			(stream_t*)0xDEADBEEF,
			(logger_t*)0xDEADBEEF
		);
	lexleo_vm_cfg_t vm_cfg = lexleo_vm_default_cfg();
	lexleo_vm_t *vm = NULL;
	assert_int_equal(
		lexleo_vm_create(
			&vm,
			&vm_cfg,
			&vm_env
		),
		LEXLEO_VM_STATUS_OK
	);

	/* ACT */
	lexleo_vm_status_t ret = lexleo_vm_complete_default_init(vm);
	assert_non_null(vm);
	lexleo_vm_destroy(&vm);
	assert_null(vm);

	/* ASSERT */
	assert_true(ret == LEXLEO_VM_STATUS_OK);

	/* TEARDOWN */
}

/**
 * @brief Smoke-tests stdio stream creation through VM stream creators.
 *
 * See contracts:
 * - @ref specifications_lexleo_vm_create
 * - @ref specifications_lexleo_vm_complete_default_init
 * - @ref specifications_stream_io_creator_create
 * - @ref specifications_stream_write
 * - @ref specifications_stream_flush
 * - @ref specifications_stream_destroy
 *
 * See test description:
 * - @ref testing_lexleo_vm_cr_stream_creators_integration_stdio
 */
static void test_stream_creators_smoke_stdio_stream(void **state) {
	(void)state;

	/* SETUP */
	lexleo_vm_env_t vm_env =
		lexleo_vm_default_env(
			osal_mem_default_ops(),
			osal_stdio_default_ops(),
			osal_file_default_ops(),
			osal_str_default_ops(),
			osal_time_default_ops(),
			(stream_t*)0xDEADBEEF,
			(stream_t*)0xDEADBEEF,
			(stream_t*)0xDEADBEEF,
			(logger_t*)0xDEADBEEF
		);
	lexleo_vm_cfg_t vm_cfg = lexleo_vm_default_cfg();
	lexleo_vm_t *vm = NULL;
	assert_int_equal(
		lexleo_vm_create(
			&vm,
			&vm_cfg,
			&vm_env
		),
		LEXLEO_VM_STATUS_OK
	);
	assert_int_equal(
		lexleo_vm_complete_default_init(vm),
		LEXLEO_VM_STATUS_OK
	);

	/* ARRANGE */
	stream_io_creator_t *stream_io_creator = lexleo_vm_get_stream_io_creator(vm);
	assert_non_null(stream_io_creator);
	stream_t *stream = NULL;
	stream_status_t stream_io_creator_create_ret = STREAM_STATUS_OK;
	size_t stream_write_ret = 0;
	const char *write_buf = "To be written in stdout";
	size_t write_buf_len = osal_strlen(write_buf);
	stream_status_t write_status = STREAM_STATUS_OK;
	stream_status_t stream_flush_ret = STREAM_STATUS_OK;
	stream_status_t stream_destroy_ret = STREAM_STATUS_OK;

	/* ACT */
	stream_io_creator_create_ret =
		stream_io_creator_create(
			stream_io_creator,
			STREAM_IO_OUTPUT,
			&stream
		);
	assert_non_null(stream);
	stream_write_ret =
		stream_write(
			stream,
			write_buf,
			write_buf_len,
			&write_status
		);
	stream_flush_ret = stream_flush(stream);
	stream_destroy_ret = stream_destroy(&stream);
	assert_null(stream);

	/* ASSERT */
	assert_int_equal(
		stream_io_creator_create_ret,
		STREAM_STATUS_OK
	);
	assert_true(stream_write_ret == write_buf_len);
	assert_true(stream_flush_ret == STREAM_STATUS_OK);
	assert_int_equal(
		stream_destroy_ret,
		STREAM_STATUS_OK
	);
	assert_int_equal(
		write_status,
		STREAM_STATUS_OK
	);

	/* TEARDOWN */
	lexleo_vm_destroy(&vm);
	assert_null(vm);
}

/**
 * @brief Smoke-tests file stream creation through VM stream creators.
 *
 * See contracts:
 * - @ref specifications_lexleo_vm_create
 * - @ref specifications_lexleo_vm_complete_default_init
 * - @ref specifications_stream_regular_file_creator_create
 * - @ref specifications_stream_write
 * - @ref specifications_stream_read
 * - @ref specifications_stream_destroy
 *
 * See test description:
 * - @ref testing_lexleo_vm_cr_stream_creators_integration_file
 */
static void test_stream_creators_smoke_fs_stream(void **state) {
	(void)state;

	/* SETUP */
	lexleo_vm_env_t vm_env =
		lexleo_vm_default_env(
			osal_mem_default_ops(),
			osal_stdio_default_ops(),
			osal_file_default_ops(),
			osal_str_default_ops(),
			osal_time_default_ops(),
			(stream_t*)0xDEADBEEF,
			(stream_t*)0xDEADBEEF,
			(stream_t*)0xDEADBEEF,
			(logger_t*)0xDEADBEEF
		);
	lexleo_vm_cfg_t vm_cfg = lexleo_vm_default_cfg();
	lexleo_vm_t *vm = NULL;
	assert_int_equal(
		lexleo_vm_create(
			&vm,
			&vm_cfg,
			&vm_env
		),
		LEXLEO_VM_STATUS_OK
	);
	assert_int_equal(
		lexleo_vm_complete_default_init(vm),
		LEXLEO_VM_STATUS_OK
	);

	/* ARRANGE */
	stream_file_creator_t *stream_file_creator = lexleo_vm_get_stream_file_creator(vm);
	assert_non_null(stream_file_creator);
	stream_t *stream = NULL;
	stream_status_t stream_file_creator_create_ret_1 = STREAM_STATUS_OK;
	stream_status_t stream_file_creator_create_ret_2 = STREAM_STATUS_OK;
	size_t stream_write_ret = 0;
	const char *write_buf = "To be written in file";
	size_t write_buf_len = osal_strlen(write_buf);
	stream_status_t write_status = STREAM_STATUS_OK;
	size_t stream_read_ret = 0;
	char read_buf[64] = {0};
	stream_status_t read_status = STREAM_STATUS_OK;
	stream_status_t stream_destroy_ret_1 = STREAM_STATUS_OK;
	stream_status_t stream_destroy_ret_2 = STREAM_STATUS_OK;
	stream_file_creator_create_ret_1 =
		stream_file_creator_create(
			stream_file_creator,
			"lexleo_vm_cr_fs_stream_smoke_test.tmp",
			"wb",
			&stream
		);
	assert_non_null(stream);
	stream_write_ret =
		stream_write(
			stream,
			write_buf,
			write_buf_len,
			&write_status
		);
	stream_destroy_ret_1 = stream_destroy(&stream);
	assert_null(stream);
	stream_file_creator_create_ret_2 =
		stream_file_creator_create(
			stream_file_creator,
			"lexleo_vm_cr_fs_stream_smoke_test.tmp",
			"rb",
			&stream
		);
	assert_non_null(stream);
	stream_read_ret =
		stream_read(
			stream,
			read_buf,
			write_buf_len,
			&read_status
		);
	stream_destroy_ret_2 = stream_destroy(&stream);
	assert_null(stream);

	/* ASSERT */
	assert_int_equal(
		stream_file_creator_create_ret_1,
		STREAM_STATUS_OK
	);
	assert_int_equal(
		stream_file_creator_create_ret_2,
		STREAM_STATUS_OK
	);
	assert_true(stream_write_ret == write_buf_len);
	assert_true(stream_read_ret == write_buf_len);
	assert_int_equal(
		stream_destroy_ret_1,
		STREAM_STATUS_OK
	);
	assert_int_equal(
		stream_destroy_ret_2,
		STREAM_STATUS_OK
	);
	assert_int_equal(
		write_status,
		STREAM_STATUS_OK
	);
	assert_int_equal(
		read_status,
		STREAM_STATUS_OK
	);
	assert_string_equal(
		write_buf,
		read_buf
	);

	/* TEARDOWN */
	lexleo_vm_destroy(&vm);
	assert_null(vm);
}

/**
 * @brief Smoke-tests buffer stream creation through VM stream creators.
 *
 * See contracts:
 * - @ref specifications_lexleo_vm_create
 * - @ref specifications_lexleo_vm_complete_default_init
 * - @ref specifications_stream_buffer_creator_create
 * - @ref specifications_stream_write
 * - @ref specifications_stream_read
 * - @ref specifications_stream_destroy
 *
 * See test description:
 * - @ref testing_lexleo_vm_cr_stream_creators_integration_buffer
 */
static void test_stream_creators_smoke_dynamic_buffer_stream(void **state) {
	(void)state;

	/* SETUP */
	lexleo_vm_env_t vm_env =
		lexleo_vm_default_env(
			osal_mem_default_ops(),
			osal_stdio_default_ops(),
			osal_file_default_ops(),
			osal_str_default_ops(),
			osal_time_default_ops(),
			(stream_t*)0xDEADBEEF,
			(stream_t*)0xDEADBEEF,
			(stream_t*)0xDEADBEEF,
			(logger_t*)0xDEADBEEF
		);
	lexleo_vm_cfg_t vm_cfg = lexleo_vm_default_cfg();
	lexleo_vm_t *vm = NULL;
	assert_int_equal(
		lexleo_vm_create(
			&vm,
			&vm_cfg,
			&vm_env
		),
		LEXLEO_VM_STATUS_OK
	);
	assert_int_equal(
		lexleo_vm_complete_default_init(vm),
		LEXLEO_VM_STATUS_OK
	);

	/* ARRANGE */
	stream_buffer_creator_t *stream_buffer_creator = lexleo_vm_get_stream_buffer_creator(vm);
	assert_non_null(stream_buffer_creator);
	stream_t *stream = NULL;
	stream_status_t stream_buffer_creator_create_ret = STREAM_STATUS_OK;
	size_t stream_write_ret = 0;
	const char *write_buf = "To be written in dynamic buffer";
	size_t write_buf_len = osal_strlen(write_buf);
	stream_status_t write_status = STREAM_STATUS_OK;
	size_t stream_read_ret = 0;
	char read_buf[64] = {0};
	stream_status_t read_status = STREAM_STATUS_OK;
	stream_status_t stream_destroy_ret = STREAM_STATUS_OK;

	/* ACT */
	stream_buffer_creator_create_ret =
		stream_buffer_creator_create(
			stream_buffer_creator,
			&stream
		);
	assert_non_null(stream);
	stream_write_ret =
		stream_write(
			stream,
			write_buf,
			write_buf_len,
			&write_status
		);
	stream_read_ret =
		stream_read(
			stream,
			read_buf,
			write_buf_len,
			&read_status
		);
	stream_destroy_ret = stream_destroy(&stream);
	assert_null(stream);

	/* ASSERT */
	assert_int_equal(
		stream_buffer_creator_create_ret,
		STREAM_STATUS_OK
	);
	assert_true(stream_write_ret == write_buf_len);
	assert_int_equal(
		write_status,
		STREAM_STATUS_OK
	);
	assert_true(stream_read_ret == write_buf_len);
	assert_int_equal(
		read_status,
		STREAM_STATUS_OK
	);
	assert_string_equal(write_buf, read_buf);
	assert_int_equal(
		stream_destroy_ret,
		STREAM_STATUS_OK
	);

	/* TEARDOWN */
	lexleo_vm_destroy(&vm);
	assert_null(vm);
}

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	static const struct CMUnitTest non_parametric_tests[] = {
		cmocka_unit_test(test_stream_creators_lifecycle_ok),
		cmocka_unit_test(test_stream_creators_smoke_stdio_stream),
		cmocka_unit_test(test_stream_creators_smoke_fs_stream),
		cmocka_unit_test(test_stream_creators_smoke_dynamic_buffer_stream)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(non_parametric_tests, NULL, NULL);

	return failed;
}

/** @endcond */
