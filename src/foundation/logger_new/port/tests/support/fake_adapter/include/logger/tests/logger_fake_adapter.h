/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_fake_adapter.h
 * @ingroup logger_tests_group
 * @brief Programmable logger adapter test double.
 *
 * @details
 * Declares the fake logger adapter backend, its configurable operation
 * results and spies used by logger unit tests.
 */

#ifndef LEXLEO_LOGGER_FAKE_ADAPTER_H
#define LEXLEO_LOGGER_FAKE_ADAPTER_H

#include "logger/adapters/logger_adapters_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dispatch table for the fake logger adapter.
 */
extern const logger_vtbl_t logger_fake_adapter_vtbl;

/**
 * @brief Programmable fake logger adapter backend.
 *
 * @details
 * Stores configuration controlling the values returned by fake backend
 * operations together with spies recording their invocations.
 */
typedef struct logger_fake_adapter_backend_t {

	/** Number of log operation calls. */
	size_t log_call_count;

	/** Status returned by the next log operation. */
	logger_status_t next_log_ret;

	/** Backend passed to the most recent log operation. */
	void *last_log_backend;

	/** Message passed to the most recent log operation. */
	const char *last_log_message;

	/** Number of destroy operation calls. */
	size_t destroy_call_count;

	/** Status returned by the next destroy operation. */
	logger_status_t next_destroy_ret;

	/** Backend passed to the most recent destroy operation. */
	void *last_destroy_backend;

} logger_fake_adapter_backend_t;

/**
 * @brief Reset a fake adapter backend to its default state.
 *
 * @param[in,out] b Fake adapter backend to reset.
 */
void logger_fake_adapter_init_backend(
	logger_fake_adapter_backend_t *b
);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_FAKE_ADAPTER_H */
