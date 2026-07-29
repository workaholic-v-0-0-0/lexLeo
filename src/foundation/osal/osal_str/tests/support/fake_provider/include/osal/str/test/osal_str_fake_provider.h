/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_str_fake_provider.h
 * @ingroup osal_str_tests_group
 * @brief Fake provider for injectable OSAL string operations.
 *
 * @details
 * Exposes an `osal_str_ops_t` table backed by `fake_str`.
 */

#ifndef LEXLEO_OSAL_STR_FAKE_PROVIDER_H
#define LEXLEO_OSAL_STR_FAKE_PROVIDER_H

#include "osal/str/osal_str_ops.h"

#include "lexleo/test/fake_str.h"

#ifdef __cplusplus
extern "C" {
#endif

const osal_str_ops_t *osal_str_test_fake_ops(void);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_OSAL_STR_FAKE_PROVIDER_H */
