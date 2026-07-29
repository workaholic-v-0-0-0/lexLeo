/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_logger_adapter.h
 * @ingroup test_support_fake_logger_adapter
 * @brief Fake logger adapter support for unit tests.
 *
 * @details
 * bbb
 */

#ifndef LEXLEO_FAKE_LOGGER_ADAPTER_H
#define LEXLEO_FAKE_LOGGER_ADAPTER_H

#include "logger/adapters/logger_adapters_types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const logger_vtbl_t *g_fake_logger_adapter_vtbl;

typedef struct fake_logger_adapter_t {

	/* cfg */
	logger_status_t next_log_ret;

	/* spy */
	size_t log_call_count;
	const char *last_log_message;

} fake_logger_adapter_t;

void fake_logger_adapter_reset_instance(fake_logger_adapter_t *fake_logger_adapter);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FAKE_LOGGER_ADAPTER_H */
