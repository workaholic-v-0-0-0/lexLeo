/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_lexleo_vm_cr_stream_creators.c
 * @ingroup lexleo_vm_unit_tests
 * @brief Unit tests for LexLeo VM default stream creator wiring.
 *
 * @details
 * This file tests failure handling during default owned-resource
 * initialization of the LexLeo VM stream factory and stream creators.
 *
 * See also:
 * - @ref specifications_lexleo_vm_complete_default_init
 * - @ref testing_lexleo_vm_cr_stream_creators_unit
 */

#include "lexleo_vm/cr/lexleo_vm_cr_api.h"

#include "osal/mem/test/osal_mem_fake_provider.h"
#include "osal/stdio/osal_stdio_ops.h"
#include "osal/file/osal_file_ops.h"
#include "osal/str/osal_str_ops.h"
#include "osal/time/osal_time_ops.h"

#include "lexleo_cmocka.h"

/**
 * @brief Tests OOM failure during default stream-resource initialization.
 *
 * Doubles:
 * - fake_memory.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_complete_default_init
 *
 * See test description:
 * - @ref testing_lexleo_vm_cr_stream_creators_unit_complete_default_init_oom
 */
static void test_complete_default_init_oom(void **state)
{
	(void)state;

	/* SETUP */
	fake_memory_reset();
	lexleo_vm_env_t vm_env =
		lexleo_vm_default_env(
			osal_mem_test_fake_ops(),
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
	fake_memory_fail_only_on_call(1);

	/* ARRANGE */

	/* ACT */
	lexleo_vm_status_t ret = lexleo_vm_complete_default_init(vm);

	/* ASSERT */
	assert_true(
		   ret == 	LEXLEO_VM_STATUS_STREAM_FACTORY_INIT_OOM
		|| ret == LEXLEO_VM_STATUS_STREAM_IO_CREATOR_INIT_OOM
		|| ret == LEXLEO_VM_STATUS_STREAM_FILE_CREATOR_INIT_OOM
		|| ret == LEXLEO_VM_STATUS_STREAM_BUFFER_CREATOR_INIT_OOM
	);

	/* TEARDOWN */
	lexleo_vm_destroy(&vm);
	assert_null(vm);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	static const struct CMUnitTest non_parametric_tests[] = {
		cmocka_unit_test(test_complete_default_init_oom)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(non_parametric_tests, NULL, NULL);

	return failed;
}

/** @endcond */
