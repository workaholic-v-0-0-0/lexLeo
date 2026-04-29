/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_stdio_fake_provider.c
 * @ingroup osal_stdio_tests_group
 * @brief Fake provider implementation for the `osal_stdio` module tests.
 *
 * @details
 * This file exposes an injectable `osal_stdio_ops_t` table wired to the
 * `fake_stdio` test backend.
 */

#include "osal/stdio/test/osal_stdio_fake_provider.h"

const osal_stdio_ops_t *osal_stdio_test_fake_ops(void)
{
	static const osal_stdio_ops_t FAKE_STDIO_OPS = {
		.get_stdin = fake_stdio_stdin,
		.get_stdout = fake_stdio_stdout,
		.get_stderr = fake_stdio_stderr,
		.read = fake_stdio_read,
		.write = fake_stdio_write,
		.flush = fake_stdio_flush,
		.error = fake_stdio_error,
		.eof = fake_stdio_eof
	};
	return &FAKE_STDIO_OPS;
}
