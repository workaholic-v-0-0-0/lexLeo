/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_time.h
 * @ingroup test_support_fake_time
 * @brief Fake time backend for unit tests.
 *
 * @details
 * This header declares a test double for time-dependent code.
 *
 * Tests can configure the returned time value, force the status returned by
 * `fake_time_now()`, reset the fake state, and observe how many times the fake
 * time source was called.
 */

#ifndef LEXLEO_FAKE_TIME_H
#define LEXLEO_FAKE_TIME_H

#include "osal/time/osal_time_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// FAKE API

osal_time_status_t fake_time_now(osal_time_t *out);

// CFG

void fake_time_reset(void);
void fake_time_set_now_status(osal_time_status_t status);
void fake_time_set_now_out(osal_time_t out);

// SPY

size_t fake_time_get_call_count(void);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FAKE_TIME_H */
