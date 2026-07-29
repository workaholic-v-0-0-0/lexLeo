/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_fake_adapter.c
 * @ingroup logger_tests_group
 * @brief Programmable logger adapter test double implementation.
 *
 * @details
 * Implements the fake logger adapter backend operations and spies used by
 * logger unit tests.
 */

#include "logger/tests/logger_fake_adapter.h"

#include "logger/adapters/logger_adapters_types.h"

#include "lexleo_cmocka.h"

void logger_fake_adapter_init_backend(logger_fake_adapter_backend_t *b)
{
	assert_non_null(b);
	b->log_call_count = 0;
	b->next_log_ret = LOGGER_STATUS_OK;
	b->last_log_backend = NULL;
	b->last_log_message = NULL;
	b->destroy_call_count = 0;
	b->next_destroy_ret = LOGGER_STATUS_OK;
	b->last_destroy_backend = NULL;
}

static logger_status_t logger_fake_adapter_log(
    void *backend,
    const char *message
) {
    logger_fake_adapter_backend_t *b = (logger_fake_adapter_backend_t *)backend;
    assert_non_null(b);

    b->log_call_count++;

	b->last_log_backend = backend;
	b->last_log_message = message;

    return b->next_log_ret;
}

static logger_status_t logger_fake_adapter_destroy(void *backend)
{
	if (!backend) return LOGGER_STATUS_NO_BACKEND;
	logger_fake_adapter_backend_t *b = (logger_fake_adapter_backend_t *)backend;

    b->destroy_call_count++;

	b->last_destroy_backend = backend;

    return b->next_destroy_ret;
}

const logger_vtbl_t logger_fake_adapter_vtbl = {
    .log = logger_fake_adapter_log,
    .destroy = logger_fake_adapter_destroy
};
