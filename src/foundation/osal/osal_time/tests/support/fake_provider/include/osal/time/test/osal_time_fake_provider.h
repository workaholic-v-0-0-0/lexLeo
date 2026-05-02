/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_time_fake_provider.h
 * @ingroup osal_time_tests_group
 * @brief Fake time provider for `osal_time` unit tests.
 *
 * @details
 * This header declares a test-only fake provider for OSAL time operations.
 *
 * It exposes an `osal_time_ops_t` implementation backed by the `fake_time`
 * test module, allowing deterministic control over time-dependent behavior
 * in unit tests.
 */

#ifndef LEXLEO_OSAL_TIME_FAKE_PROVIDER_H
#define LEXLEO_OSAL_TIME_FAKE_PROVIDER_H

#include "osal/time/osal_time_ops.h"

#include "lexleo/test/fake_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return a fake OSAL time operations table for testing.
 *
 * @return Pointer to an `osal_time_ops_t` using the fake time backend.
 */
const osal_time_ops_t *osal_time_test_fake_ops(void);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_OSAL_TIME_FAKE_PROVIDER_H */
