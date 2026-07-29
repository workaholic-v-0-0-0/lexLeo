/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_str_fake_provider.c
 * @ingroup osal_str_tests_group
 * @brief Fake provider implementation for the `osal_str` module tests.
 *
 * @details
 * This file exposes an injectable `osal_str_ops_t` table wired to the
 * `fake_str` test backend.
 */

#include "osal/str/test/osal_str_fake_provider.h"

const osal_str_ops_t *osal_str_test_fake_ops(void)
{
	static const osal_str_ops_t FAKE_STR_OPS = {
		.strdup = fake_str_strdup
	};
	return &FAKE_STR_OPS;
}
