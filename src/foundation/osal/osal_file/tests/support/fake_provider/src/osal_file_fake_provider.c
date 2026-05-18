/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_file_fake_provider.c
 * @ingroup osal_file_tests_group
 * @brief Fake file operations provider for tests requiring `osal_file`
 * dependencies.
 */

#include "osal/file/test/osal_file_fake_provider.h"

static const osal_file_ops_t OSAL_FILE_FAKE_OPS = {
	.open = fake_file_open,
	.read = fake_file_read,
	.write = fake_file_write,
	.flush = fake_file_flush,
	.close = fake_file_close,
	.gets = fake_file_gets,
	.mkdir = fake_file_mkdir
};

const osal_file_ops_t *osal_file_test_fake_ops(void)
{
  return &OSAL_FILE_FAKE_OPS;
}
