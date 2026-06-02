/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_fake_provider.c
 * @ingroup stream_tests_group
 * @brief Fake stream provider implementation for stream tests.
 */

#include "stream/tests/stream_fake_provider.h"

#include "stream/cr/stream_cr_api.h"

#include "policy/lexleo_assert.h"

typedef struct stream_provider_ctrl_t {
	stream_factory_t *fact;
	stream_file_creator_t *file_creator;
	stream_buffer_creator_t *buffer_creator;
	stream_io_creator_t *io_creator;
} stream_provider_ctrl_t;

static stream_provider_ctrl_t g_stream_provider_ctrl = {0};

stream_status_t stream_test_fake_provider_setup(
	const osal_mem_ops_t *mem,
	stream_key_t buffer_key,
	stream_key_t file_key,
	stream_key_t io_key
) {
	LEXLEO_ASSERT(mem);

	stream_test_fake_provider_teardown();

	stream_factory_t *fake_stream_factory = NULL;
	stream_adapter_desc_t fake_buffer_desc = {0};
	stream_adapter_desc_t fake_file_desc = {0};
	stream_adapter_desc_t fake_io_desc = {0};

	stream_status_t stream_status = STREAM_STATUS_OK;
	stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
	stream_status =
		stream_create_factory(
			&fake_stream_factory,
			&stream_factory_cfg,
			mem
		);
	if (stream_status != STREAM_STATUS_OK) {
		return stream_status;
	}

	fake_stream_create_buffer_desc(&fake_buffer_desc, buffer_key);
	fake_stream_create_file_desc(&fake_file_desc, file_key);
	fake_stream_create_io_desc(&fake_io_desc, io_key);

	stream_status =
		stream_factory_add_adapter(
			fake_stream_factory,
			&fake_buffer_desc
		);
	if (stream_status != STREAM_STATUS_OK) {
		stream_destroy_factory(&fake_stream_factory);
		return stream_status;
	}

	stream_status =
		stream_factory_add_adapter(
			fake_stream_factory,
			&fake_file_desc
		);
	if (stream_status != STREAM_STATUS_OK) {
		stream_destroy_factory(&fake_stream_factory);
		return stream_status;
	}

	stream_status =
		stream_factory_add_adapter(
			fake_stream_factory,
			&fake_io_desc
		);
	if (stream_status != STREAM_STATUS_OK) {
		stream_destroy_factory(&fake_stream_factory);
		return stream_status;
	}

	stream_status =
		stream_create_buffer_creator(
			&g_stream_provider_ctrl.buffer_creator,
			fake_stream_factory,
			buffer_key,
			mem
		);
	if (stream_status != STREAM_STATUS_OK) {
		stream_destroy_factory(&fake_stream_factory);
		return stream_status;
	}

	stream_status =
		stream_create_file_creator(
			&g_stream_provider_ctrl.file_creator,
			fake_stream_factory,
			file_key,
			mem
		);
	if (stream_status != STREAM_STATUS_OK) {
		stream_destroy_buffer_creator(
			&g_stream_provider_ctrl.buffer_creator
		);
		stream_destroy_factory(&fake_stream_factory);
		return stream_status;
	}

	stream_status =
		stream_create_io_creator(
			&g_stream_provider_ctrl.io_creator,
			fake_stream_factory,
			io_key,
			mem
		);
	if (stream_status != STREAM_STATUS_OK) {
		stream_destroy_buffer_creator(
			&g_stream_provider_ctrl.buffer_creator
		);
		stream_destroy_file_creator(
			&g_stream_provider_ctrl.file_creator
		);
		stream_destroy_factory(&fake_stream_factory);
		return stream_status;
	}

	g_stream_provider_ctrl.fact = fake_stream_factory;

	return STREAM_STATUS_OK;
}

void stream_test_fake_provider_teardown(void)
{
	stream_destroy_file_creator(&g_stream_provider_ctrl.file_creator);
	stream_destroy_buffer_creator(&g_stream_provider_ctrl.buffer_creator);
	stream_destroy_io_creator(&g_stream_provider_ctrl.io_creator);
	stream_destroy_factory(&g_stream_provider_ctrl.fact);
}

const stream_factory_t *stream_test_fake_stream_factory(void)
{
	return g_stream_provider_ctrl.fact;
}

const stream_buffer_creator_t *stream_test_fake_buffer_creator(void)
{
	return g_stream_provider_ctrl.buffer_creator;
}

const stream_file_creator_t *stream_test_fake_file_creator(void)
{
	return g_stream_provider_ctrl.file_creator;
}

const stream_io_creator_t *stream_test_fake_io_creator(void)
{
	return g_stream_provider_ctrl.io_creator;
}
