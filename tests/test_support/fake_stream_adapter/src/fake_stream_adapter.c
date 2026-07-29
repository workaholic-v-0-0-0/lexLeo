/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_stream_adapter.c
 * @ingroup test_support_fake_stream_adapter
 * @brief Fake stream adapter implementation for unit tests.
 *
 * @details
 * This file implements fake `stream` adapter providers, configurable fake
 * backends, constructor controls, and helper functions for use in tests.
 *
 * The fake adapter supports controllable read, write, flush, close, and
 * backend-construction behavior, buffered input and output state, sink data
 * inspection, sequential backend configurations, and operation and creator
 * argument spies for dynamic-buffer, regular-file, and standard-stream
 * creators.
 */

#include "lexleo/test/fake_stream_adapter.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"

#include "lexleo_cmocka.h"

static size_t fake_stream_adapter_read(
	void *backend,
	void *buf,
	size_t n,
	stream_status_t *st
) {
	fake_stream_adapter_t *fake_stream_adapter = (fake_stream_adapter_t *)backend;
	assert_non_null(fake_stream_adapter);
	assert_true(
		   fake_stream_adapter->read_pos <= fake_stream_adapter->buffered_len
		&& fake_stream_adapter->next_read_ret <= fake_stream_adapter->buffered_len - fake_stream_adapter->read_pos
		&& fake_stream_adapter->next_read_ret <= n
		&& (
			   fake_stream_adapter->next_read_st_value != STREAM_STATUS_EOF
			|| fake_stream_adapter->end_of_input_has_been_signaled)
	);

	fake_stream_adapter->read_call_count++;
	fake_stream_adapter->last_read_buf = buf;
	fake_stream_adapter->last_read_n = n;
	fake_stream_adapter->last_read_st = st;

	if (fake_stream_adapter->next_read_ret != 0) {
		assert_non_null(buf);
		osal_memcpy(buf, fake_stream_adapter->buffered_backing + fake_stream_adapter->read_pos, fake_stream_adapter->next_read_ret);
	}

	if (
		   fake_stream_adapter->read_pos + fake_stream_adapter->next_read_ret == fake_stream_adapter->buffered_len
		&& n > fake_stream_adapter->buffered_len - fake_stream_adapter->read_pos
		&& fake_stream_adapter->end_of_input_has_been_signaled
	) {
		assert_true(fake_stream_adapter->next_read_st_value == STREAM_STATUS_EOF);
		fake_stream_adapter->end_of_input_has_been_encountered = true;
	}

	fake_stream_adapter->read_pos += fake_stream_adapter->next_read_ret;

	if (fake_stream_adapter->last_read_st) {
		*fake_stream_adapter->last_read_st = fake_stream_adapter->next_read_st_value;
	}
	return fake_stream_adapter->next_read_ret;
}

static size_t fake_stream_adapter_write(
	void *backend,
	const void *buf,
	size_t n,
	stream_status_t *st
) {
	fake_stream_adapter_t *fake_stream_adapter = (fake_stream_adapter_t *)backend;
	assert_non_null(fake_stream_adapter);

	size_t ret =
		fake_stream_adapter->write_all
		? n
		: fake_stream_adapter->next_write_ret;

	assert_true(
		   fake_stream_adapter->buffered_len <= FAKE_STREAM_ADAPTER_BUF_SIZE
		&& ret <= FAKE_STREAM_ADAPTER_BUF_SIZE - fake_stream_adapter->buffered_len
		&& ret <= n
	);

	fake_stream_adapter->write_call_count++;
	fake_stream_adapter->last_write_buf = buf;
	fake_stream_adapter->last_write_n = n;
	fake_stream_adapter->last_write_st = st;

	if (ret != 0) {
		assert_non_null(buf);
		osal_memcpy(fake_stream_adapter->buffered_backing + fake_stream_adapter->buffered_len, buf, ret);
		fake_stream_adapter->buffered_len += ret;
	}

	if (fake_stream_adapter->last_write_st) {
		*fake_stream_adapter->last_write_st = fake_stream_adapter->next_write_st_value;
	}
	return ret;
}

static stream_status_t fake_stream_adapter_flush(void *backend)
{
	fake_stream_adapter_t *fake_stream_adapter = (fake_stream_adapter_t *)backend;
	assert_non_null(fake_stream_adapter);
	assert_true(
		   fake_stream_adapter->buffered_len <= FAKE_STREAM_ADAPTER_BUF_SIZE
		&& fake_stream_adapter->sink_len <= FAKE_STREAM_ADAPTER_BUF_SIZE
		&& fake_stream_adapter->buffered_len <= FAKE_STREAM_ADAPTER_BUF_SIZE - fake_stream_adapter->sink_len
	);

	fake_stream_adapter->flush_call_count++;

	if (fake_stream_adapter->next_flush_ret == STREAM_STATUS_OK) {
		osal_memcpy(
			fake_stream_adapter->sink_backing + fake_stream_adapter->sink_len,
			fake_stream_adapter->buffered_backing,
			fake_stream_adapter->buffered_len
		);
		fake_stream_adapter->sink_len += fake_stream_adapter->buffered_len;
		fake_stream_adapter->buffered_len = 0;
		fake_stream_adapter->read_pos = 0;
	}

	return fake_stream_adapter->next_flush_ret;
}

static stream_status_t fake_stream_adapter_close(void *backend)
{
	fake_stream_adapter_t *fake_stream_adapter = (fake_stream_adapter_t *)backend;
	assert_non_null(fake_stream_adapter);

	fake_stream_adapter->close_call_count++;

	fake_stream_adapter_flush(backend);

	return 	fake_stream_adapter->next_close_ret;
}

static const stream_vtbl_t g_fake_stream_adapter_vtbl_impl = {
	.read = fake_stream_adapter_read,
	.write = fake_stream_adapter_write,
	.flush = fake_stream_adapter_flush,
	.close = fake_stream_adapter_close
};

const stream_vtbl_t *g_fake_stream_adapter_vtbl = &g_fake_stream_adapter_vtbl_impl;

fake_stream_adapter_ctrl_t g_fake_stream_adapter_ctrl_seq[FAKE_STREAM_ADAPTER_MAX_SEQ_LEN] = {0};
static size_t g_fake_stream_adapter_ctrl_off = 0;

static fake_stream_adapter_args_kind_t g_fake_stream_adapter_args_kind_dynamic_buffer = FAKE_STREAM_ADAPTER_DYNAMIC_BUFFER;
static fake_stream_adapter_args_kind_t g_fake_stream_adapter_args_kind_regular_file = FAKE_STREAM_ADAPTER_REGULAR_FILE;
static fake_stream_adapter_args_kind_t g_fake_stream_adapter_args_kind_standard_stream = FAKE_STREAM_ADAPTER_STANDARD_STREAM;

static stream_status_t fake_stream_adapter_backend_ctor(
	const void *ud,
	const void *args,
	void **out_backend
) {
	assert_non_null(ud);
	assert_non_null(out_backend);
	assert_true(g_fake_stream_adapter_ctrl_off < FAKE_STREAM_ADAPTER_MAX_SEQ_LEN);
	fake_stream_adapter_ctrl_t *ctrl = &g_fake_stream_adapter_ctrl_seq[g_fake_stream_adapter_ctrl_off++];
	ctrl->last_args_kind = *(const fake_stream_adapter_args_kind_t *)ud;
	switch (ctrl->last_args_kind) {
		case FAKE_STREAM_ADAPTER_DYNAMIC_BUFFER: {
			break;
		}
		case FAKE_STREAM_ADAPTER_REGULAR_FILE: {
			assert_non_null(args);
			const stream_regular_file_creator_args_t *casted_args = (const stream_regular_file_creator_args_t *)args;
			ctrl->last_path = casted_args->path;
			ctrl->last_mode = casted_args->mode;
			break;
		}
		case FAKE_STREAM_ADAPTER_STANDARD_STREAM: {
			assert_non_null(args);
			const stream_standard_stream_creator_args_t *casted_args = (const stream_standard_stream_creator_args_t *)args;
			ctrl->last_kind = casted_args->kind;
			break;
		}
		default: {
			fail();
		}
	}
	*out_backend = &ctrl->next_fake_stream_adapter;
	return ctrl->next_stream_adapter_ctor_ret;
}

static void fake_stream_adapter_dummy_free(void *ptr) { (void)ptr; }
static const osal_mem_ops_t g_fake_stream_adapter_mem_ops = {
	.malloc = NULL,
	.free = fake_stream_adapter_dummy_free,
	.calloc = NULL,
	.realloc = NULL
};

stream_adapter_provider_t g_fake_stream_adapter_provider_dynamic_buffer = {
	.backend_ctor = fake_stream_adapter_backend_ctor,
	.vtbl = &g_fake_stream_adapter_vtbl_impl,
	.ud = &g_fake_stream_adapter_args_kind_dynamic_buffer,
	.ud_dtor = NULL,
	.mem = &g_fake_stream_adapter_mem_ops
};

stream_adapter_provider_t g_fake_stream_adapter_provider_regular_file = {
	.backend_ctor = fake_stream_adapter_backend_ctor,
	.vtbl = &g_fake_stream_adapter_vtbl_impl,
	.ud = &g_fake_stream_adapter_args_kind_regular_file,
	.ud_dtor = NULL,
	.mem = &g_fake_stream_adapter_mem_ops
};

stream_adapter_provider_t g_fake_stream_adapter_provider_standard_stream = {
	.backend_ctor = fake_stream_adapter_backend_ctor,
	.vtbl = &g_fake_stream_adapter_vtbl_impl,
	.ud = &g_fake_stream_adapter_args_kind_standard_stream,
	.ud_dtor = NULL,
	.mem = &g_fake_stream_adapter_mem_ops
};

void fake_stream_adapter_reset_instance(fake_stream_adapter_t *fake_stream_adapter)
{
	assert_non_null(fake_stream_adapter);
	*fake_stream_adapter = (fake_stream_adapter_t){0};
}

void fake_stream_adapter_reset(void)
{
	for (size_t i = 0; i < FAKE_STREAM_ADAPTER_MAX_SEQ_LEN; ++i) {
		g_fake_stream_adapter_ctrl_seq[i] = (fake_stream_adapter_ctrl_t){0};
	}
	g_fake_stream_adapter_ctrl_off = 0;
}

void fake_stream_adapter_set_buffered_backing(
	fake_stream_adapter_t *fake_stream_adapter,
	const uint8_t *backing,
	size_t len
) {
	assert_non_null(fake_stream_adapter);
	assert_non_null(backing);
	assert_true(len <= FAKE_STREAM_ADAPTER_BUF_SIZE);

	osal_memcpy(fake_stream_adapter->buffered_backing, backing, len);
	fake_stream_adapter->buffered_len = len;
}

void fake_stream_adapter_set_sink_backing(
	fake_stream_adapter_t *fake_stream_adapter,
	const uint8_t *backing,
	size_t len
) {
	assert_non_null(fake_stream_adapter);
	assert_non_null(backing);
	assert_true(len <= FAKE_STREAM_ADAPTER_BUF_SIZE);

	osal_memcpy(fake_stream_adapter->sink_backing, backing, len);
	fake_stream_adapter->sink_len = len;
}

void fake_stream_adapter_set_read_pos(
	fake_stream_adapter_t *fake_stream_adapter,
	size_t n
) {
	assert_non_null(fake_stream_adapter);
	assert_true(
		   fake_stream_adapter->buffered_len <= FAKE_STREAM_ADAPTER_BUF_SIZE
		&& n <= fake_stream_adapter->buffered_len
	);
	fake_stream_adapter->read_pos = n;
}
