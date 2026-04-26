/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_mem_fake_provider.c
 * @ingroup osal_mem_tests_group
 * @brief Fake memory operations provider for `osal_mem` tests.
 */

#include "osal/mem/test/osal_mem_fake_provider.h"

static const osal_mem_ops_t OSAL_MEM_FAKE_OPS = {
	.malloc = fake_malloc,
	.free = fake_free,
	.calloc = fake_calloc,
	.realloc = fake_realloc,
};

const osal_mem_ops_t *osal_mem_test_fake_ops(void)
{
  return &OSAL_MEM_FAKE_OPS;
}
