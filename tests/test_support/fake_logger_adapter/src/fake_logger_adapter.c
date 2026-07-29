/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_logger_adapter.c
 * @ingroup test_support_fake_logger_adapter
 * @brief Fake logger adapter implementation.
 */

#include "lexleo/test/fake_logger_adapter.h"

#include "osal/mem/osal_mem.h"

#include "lexleo_cmocka.h"

static logger_status_t fake_logger_adapter_log(
	void *backend,
	const char *message
) {
	assert_non_null(backend);
	fake_logger_adapter_t *fake_logger_adapter = (fake_logger_adapter_t *)backend;

	fake_logger_adapter->log_call_count++;
	fake_logger_adapter->last_log_message = message;

	return fake_logger_adapter->next_log_ret;
}

static logger_status_t fake_logger_adapter_destroy(void *backend)
{
	assert_non_null(backend);
	fake_logger_adapter_t *fake_logger_adapter = (fake_logger_adapter_t *)backend;

	fake_logger_adapter->destroy_call_count++;

	return fake_logger_adapter->next_destroy_ret;
}

const logger_vtbl_t g_fake_logger_adapter_vtbl_impl = {
	.log = fake_logger_adapter_log,
	.destroy = fake_logger_adapter_destroy
};

const logger_vtbl_t *g_fake_logger_adapter_vtbl = &g_fake_logger_adapter_vtbl_impl;

void fake_logger_adapter_reset_instance(
	fake_logger_adapter_t *fake_logger_adapter
) {
	assert_non_null(fake_logger_adapter);
	osal_memset(fake_logger_adapter,0, sizeof(fake_logger_adapter));
}
