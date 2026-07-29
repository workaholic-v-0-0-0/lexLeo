/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_stream.c
 * @ingroup test_support_fake_stream
 * @brief Fake stream implementation for unit tests.
 *
 * @details
 * This file implements reusable test support for constructing and resetting
 * `stream` instances backed by fake stream adapters.
 *
 * The module provides a private bounded allocator used to create real
 * `stream`, factory, and creator objects without depending on the production
 * memory provider.
 *
 * Direct fake stream instances are constructed through the production stream
 * creation path and receive caller-supplied fake adapters through the
 * white-box backend injection interface.
 *
 * The module also builds a real stream factory populated with fake
 * dynamic-buffer, regular-file, and standard-stream adapter providers, then
 * exposes corresponding production creators for tests that need to exercise
 * the factory and creator layers.
 *
 * Test-owned allocation storage is reset by `fake_stream_reset()` so each
 * test can start from a deterministic allocation state.
 */

#include "lexleo/test/fake_stream.h"

#include "stream/cr/stream_cr_api.h"
#include "stream/tests/stream_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem_align.h"

#include "lexleo_cmocka.h"

#define FAKE_STREAM_MEMORY_SIZE 1024
OSAL_ALIGNED_MAX static uint8_t g_fake_stream_memory[FAKE_STREAM_MEMORY_SIZE];
static size_t g_fake_stream_memory_off;
static void fake_stream_memory_reset(void)
{
	osal_memset(g_fake_stream_memory, 0, FAKE_STREAM_MEMORY_SIZE);
	g_fake_stream_memory_off = 0;
}

static void *fake_stream_malloc(size_t size)
{
	size_t aligned_size =
		(size % OSAL_ALIGNOF_MAX)
			? (size / OSAL_ALIGNOF_MAX + 1) * OSAL_ALIGNOF_MAX
			: size;
	assert_true(
		aligned_size <= FAKE_STREAM_MEMORY_SIZE - g_fake_stream_memory_off
	);
	void *ret = g_fake_stream_memory + g_fake_stream_memory_off;
	g_fake_stream_memory_off += aligned_size;
	return ret;
}

static void *fake_stream_calloc(size_t nmemb, size_t size)
{
	return fake_stream_malloc(nmemb * size);
}

static void *fake_stream_realloc(void *ptr, size_t size)
{
	(void)ptr;
	return fake_stream_malloc(size);
}

static void fake_stream_dummy_free(void *ptr) { (void)ptr; }

static const osal_mem_ops_t g_fake_stream_mem_ops = {
	.malloc = fake_stream_malloc,
	.free = fake_stream_dummy_free,
	.calloc = fake_stream_calloc,
	.realloc = fake_stream_realloc
};

fake_stream_t *fake_stream_create_instance(
	fake_stream_adapter_t *fake_stream_adapter
) {
	assert_non_null(fake_stream_adapter);
	stream_t *fake_stream = NULL;
	stream_env_t stream_env = stream_default_env(g_fake_stream_adapter_vtbl, &g_fake_stream_mem_ops);
	assert_int_equal(stream_create(&fake_stream, &stream_env), STREAM_STATUS_OK);
	stream_inject_backend(fake_stream, fake_stream_adapter);
	assert_int_equal(stream_complete_default_init(fake_stream, NULL), STREAM_STATUS_OK);
	return fake_stream;
}

void fake_stream_reset_instance(fake_stream_t *fake_stream)
{
	assert_non_null(fake_stream);
	fake_stream_adapter_t *fake_stream_adapter = stream_get_backend(fake_stream);
	assert_non_null(fake_stream_adapter);
	fake_stream_adapter_reset_instance(fake_stream_adapter);
}

stream_dynamic_buffer_creator_t *g_fake_stream_dynamic_buffer_creator = NULL;
stream_regular_file_creator_t *g_fake_stream_regular_file_creator = NULL;
stream_standard_stream_creator_t *g_fake_stream_standard_stream_creator = NULL;

void fake_stream_reset(void)
{
	fake_stream_adapter_reset();
	fake_stream_memory_reset();
	stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
	stream_factory_t *stream_factory = NULL;
	assert_int_equal(
		stream_create_factory(
			&stream_factory,
			&stream_factory_cfg,
			&g_fake_stream_mem_ops
		),
		STREAM_FACTORY_STATUS_OK
	);
	assert_int_equal(
		stream_factory_add_adapter(
			stream_factory,
			"dbs",
			&g_fake_stream_adapter_provider_dynamic_buffer
		),
		STREAM_FACTORY_STATUS_OK
	);
	assert_int_equal(
		stream_create_dynamic_buffer_creator(
			&g_fake_stream_dynamic_buffer_creator,
			stream_factory,
			"dbs",
			&g_fake_stream_mem_ops
		),
		STREAM_FACTORY_STATUS_OK
	);
	assert_int_equal(
		stream_factory_add_adapter(
			stream_factory,
			"fs",
			&g_fake_stream_adapter_provider_regular_file
		),
		STREAM_FACTORY_STATUS_OK
	);
	assert_int_equal(
		stream_create_regular_file_creator(
			&g_fake_stream_regular_file_creator,
			stream_factory,
			"fs",
			&g_fake_stream_mem_ops
		),
		STREAM_FACTORY_STATUS_OK
	);
	assert_int_equal(
		stream_factory_add_adapter(
			stream_factory,
			"stdio",
			&g_fake_stream_adapter_provider_standard_stream
		),
		STREAM_FACTORY_STATUS_OK
	);
	assert_int_equal(
		stream_create_standard_stream_creator(
			&g_fake_stream_standard_stream_creator,
			stream_factory,
			"stdio",
			&g_fake_stream_mem_ops
		),
		STREAM_FACTORY_STATUS_OK
	);
}
