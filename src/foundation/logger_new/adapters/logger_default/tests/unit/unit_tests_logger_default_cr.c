/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_tests_logger_default_cr.c
 * @ingroup logger_default_unit_tests
 * @brief Unit tests implementation for logger_default_cr.c.
 *
 * See also:
 * - @ref testing_foundation_logger_default_cr_unit
 * "logger_default_cr.c unit tests page"
 * - @ref specifications_logger_default "logger_default specifications"
 */

#include "logger_default/cr/logger_default_cr_api.h"
#include "logger/cr/logger_cr_api.h"

#include "logger_default/tests/logger_default_white_box_tests_access.h"

#include "stream/test/osal_mem_fake_provider.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**********************************************************************************************************************
 * @brief Test `logger_default_vtbl()`.
 *
 * See contract:
 * - @ref specifications_logger_default_vtbl "logger_default_vtbl() specifications".
 *
 * See test description:
 * - @ref testing_foundation_logger_default_unit_logger_default_vtbl "logger_default_vtbl() unit tests section"
 */
static void test_logger_default_vtbl(void **state) {
	(void)state;

	// ACT
	const stream_vtbl_t *ret = logger_default_vtbl();

	// ASSERT
	assert_non_null(ret);
	assert_non_null(ret->read);
	assert_non_null(ret->write);
	assert_non_null(ret->flush);
	assert_non_null(ret->close);
}
