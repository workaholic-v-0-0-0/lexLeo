/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_stdio_fake_provider.h
 * @ingroup osal_stdio_tests_group
 * @brief Fake provider for injectable `osal_stdio` operations in tests.
 *
 * @details
 * This header exposes a test-only operations table returning fake standard I/O
 * services backed by the `fake_stdio` test support module.
 */

#ifndef LEXLEO_OSAL_STDIO_FAKE_PROVIDER_H
#define LEXLEO_OSAL_STDIO_FAKE_PROVIDER_H

#include "osal/stdio/osal_stdio_ops.h"

#include "lexleo/test/fake_stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

const osal_stdio_ops_t *osal_stdio_test_fake_ops(void);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_OSAL_STDIO_FAKE_PROVIDER_H */
