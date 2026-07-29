/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_logger.c
 * @ingroup test_support_fake_logger
 * @brief Fake logger construction and reset.
 */

#include "lexleo/test/fake_logger.h"

#include "logger/cr/logger_cr_api.h"
#include "logger/tests/logger_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem_align.h"

#include "lexleo_cmocka.h"

#define FAKE_LOGGER_MEMORY_SIZE 1024
OSAL_ALIGNED_MAX static uint8_t g_fake_logger_memory[FAKE_LOGGER_MEMORY_SIZE];
static size_t g_fake_logger_memory_off;
static void fake_logger_memory_reset(void)
{
	osal_memset(g_fake_logger_memory, 0, FAKE_LOGGER_MEMORY_SIZE);
	g_fake_logger_memory_off = 0;
}

static void *fake_logger_malloc(size_t size)
{
	size_t aligned_size =
		(size % OSAL_ALIGNOF_MAX)
			? (size / OSAL_ALIGNOF_MAX + 1) * OSAL_ALIGNOF_MAX
			: size;
	assert_true(
		aligned_size <= FAKE_LOGGER_MEMORY_SIZE - g_fake_logger_memory_off
	);
	void *ret = g_fake_logger_memory + g_fake_logger_memory_off;
	g_fake_logger_memory_off += aligned_size;
	return ret;
}

static void *fake_logger_calloc(size_t nmemb, size_t size)
{
	return fake_logger_malloc(nmemb * size);
}

static void *fake_logger_realloc(void *ptr, size_t size)
{
	(void)ptr;
	return fake_logger_malloc(size);
}

static void fake_logger_dummy_free(void *ptr) { (void)ptr; }

static const osal_mem_ops_t g_fake_logger_mem_ops = {
	.malloc = fake_logger_malloc,
	.free = fake_logger_dummy_free,
	.calloc = fake_logger_calloc,
	.realloc = fake_logger_realloc
};

fake_logger_t *fake_logger_create_instance(
	fake_logger_adapter_t *fake_logger_adapter
) {
	assert_non_null(fake_logger_adapter);
	logger_t *fake_logger = NULL;
	logger_env_t logger_env = logger_default_env(g_fake_logger_adapter_vtbl, &g_fake_logger_mem_ops);
	assert_int_equal(logger_create(&fake_logger, &logger_env), LOGGER_STATUS_OK);
	logger_inject_backend(fake_logger, fake_logger_adapter);
	assert_int_equal(logger_complete_default_init(fake_logger, NULL), LOGGER_STATUS_OK);
	return fake_logger;
}

void fake_logger_reset(void)
{
	fake_logger_memory_reset();
}

void fake_logger_reset_instance(fake_logger_t *fake_logger)
{
	assert_non_null(fake_logger);
	fake_logger_adapter_t *fake_logger_adapter = logger_get_backend(fake_logger);
	assert_non_null(fake_logger_adapter);
	fake_logger_adapter_reset_instance(fake_logger_adapter);
}