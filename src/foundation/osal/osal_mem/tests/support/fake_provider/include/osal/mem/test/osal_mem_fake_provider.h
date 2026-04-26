/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_mem_fake_provider.h
 * @ingroup osal_mem_tests_group
 * @brief Fake memory operations provider for `osal_mem` tests.
 */

#ifndef LEXLEO_OSAL_MEM_FAKE_PROVIDER_H
#define LEXLEO_OSAL_MEM_FAKE_PROVIDER_H

#include "osal/mem/osal_mem_ops.h"

#include "lexleo/test/fake_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

const osal_mem_ops_t *osal_mem_test_fake_ops(void);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_OSAL_MEM_FAKE_PROVIDER_H */
