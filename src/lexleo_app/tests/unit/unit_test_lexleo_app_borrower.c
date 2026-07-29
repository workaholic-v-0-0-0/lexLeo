/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_lexleo_app_borrower.c
 * @ingroup lexleo_app_unit_tests
 * @brief Unit tests implementation for lexleo_app_borrower.
 *
 * @details
 * This file implements the unit-level validation of lexleo_app_borrower.
 *
 * See also:
 * - @ref testing_foundation_lexleo_app_borrower_unit "lexleo_app_borrower.c unit tests page"
 * - @ref specifications_lexleo_app "lexleo_app specifications"
 */

#include "lexleo_app/borrower/lexleo_app_borrower_api.h"
#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_api.h"

#include "lexleo_app/tests/lexleo_app_white_box_tests_access_cr_wrapper.h"
#include "lexleo_app/tests/lexleo_app_white_box_tests_access.h"

#include "lexleo/test/fake_stream.h"

#include "lexleo/test/fake_logger.h"

#include "lexleo/test/fake_vm.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/test/osal_mem_fake_provider.h"
#include "osal/stdio/test/osal_stdio_fake_provider.h"
#include "osal/file/test/osal_file_fake_provider.h"
#include "osal/str/test/osal_str_fake_provider.h"
#include "osal/time/test/osal_time_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/* todo */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	int failed = 0;
	//failed += cmocka_run_group_tests(, NULL, NULL);

	return failed;
}
/** @endcond */
