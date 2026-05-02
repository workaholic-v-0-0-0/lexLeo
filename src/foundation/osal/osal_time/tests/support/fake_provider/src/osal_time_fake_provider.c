/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_time_fake_provider.c
 * @ingroup osal_time_tests_group
 * @brief Fake time provider implementation for `osal_time` tests.
 *
 * @details
 * This file implements the test-only OSAL time operations provider backed by
 * the `fake_time` test module.
 */

#include "osal/time/test/osal_time_fake_provider.h"

const osal_time_ops_t *osal_time_test_fake_ops(void)
{
	static const osal_time_ops_t OPS = {
		.now = fake_time_now
	};
	return &OPS;
}
