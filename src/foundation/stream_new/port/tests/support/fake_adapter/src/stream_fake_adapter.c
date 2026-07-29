/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_fake_adapter.c
 * @ingroup stream_tests_group
 * @brief Programmable stream adapter test double implementation.
 *
 * @details
 * Implements the fake stream adapter backend operations, backend constructor,
 * spies, and adapter provider helpers used by stream unit tests.
 */

#include "stream/tests/stream_fake_adapter.h"

#include "stream/adapters/stream_adapters_types.h"

#include "osal/mem/osal_mem_ops.h"

#include "lexleo_cmocka.h"

void stream_fake_adapter_init_backend(stream_fake_adapter_backend_t *b)
{
	assert_non_null(b);

	b->next_read_st = STREAM_STATUS_INVALID;
	b->next_write_st = STREAM_STATUS_INVALID;
	b->next_read_ret = (size_t)-1;
	b->next_write_ret = (size_t)-1;
	b->next_flush_ret = STREAM_STATUS_INVALID;
	b->next_close_ret = STREAM_STATUS_OK;

	b->read_call_count = 0;
    b->write_call_count = 0;
    b->flush_call_count = 0;
    b->close_call_count = 0;
    b->last_read_buf = NULL;
    b->last_write_buf = NULL;
    b->last_read_n = (size_t)-1;
    b->last_write_n = (size_t)-1;
	b->last_read_backend = NULL;
	b->last_write_backend = NULL;
	b->last_flush_backend = NULL;
	b->last_close_backend = NULL;
    b->last_read_st = NULL;
    b->last_write_st = NULL;
}

static size_t stream_fake_adapter_read(
    void *backend,
    void *buf,
    size_t n,
    stream_status_t *st
) {
    stream_fake_adapter_backend_t *b = (stream_fake_adapter_backend_t *)backend;
    assert_non_null(b);

    b->read_call_count++;

	b->last_read_backend = backend;
    b->last_read_buf = buf;
    b->last_read_n = n;
	b->last_read_st = st;

    if (st) *st = b->next_read_st;
    return b->next_read_ret;
}

static size_t stream_fake_adapter_write(
    void *backend,
    const void *buf,
    size_t n,
    stream_status_t *st
) {
    stream_fake_adapter_backend_t *b = (stream_fake_adapter_backend_t *)backend;
    assert_non_null(b);

    b->write_call_count++;

	b->last_write_backend = backend;
    b->last_write_buf = buf;
    b->last_write_n = n;
	b->last_write_st = st;

    if (st) *st = b->next_write_st;
    return b->next_write_ret;
}

static stream_status_t stream_fake_adapter_flush(void *backend)
{
    stream_fake_adapter_backend_t *b = (stream_fake_adapter_backend_t *)backend;
    assert_non_null(b);

    b->flush_call_count++;

	b->last_flush_backend = backend;

    return b->next_flush_ret;
}

static stream_status_t stream_fake_adapter_close(void *backend)
{
	if (!backend) return STREAM_STATUS_NO_BACKEND;
	stream_fake_adapter_backend_t *b = (stream_fake_adapter_backend_t *)backend;

    b->close_call_count++;

	b->last_close_backend = backend;

    return b->next_close_ret;
}

const stream_vtbl_t stream_fake_adapter_vtbl = {
    .read = stream_fake_adapter_read,
    .write = stream_fake_adapter_write,
    .flush = stream_fake_adapter_flush,
    .close = stream_fake_adapter_close
};

static stream_status_t stream_fake_adapter_backend_ctor(
	const void *ud,
	const void *args,
	void **out_backend
) {
	assert_non_null(ud);
	stream_fake_adapter_backend_ctor_mock_spy_t *mock_spy =
		(stream_fake_adapter_backend_ctor_mock_spy_t *)ud;
	++mock_spy->call_count;
	mock_spy->last_args_arg = args;
	mock_spy->last_out_backend_arg = out_backend;
	assert_non_null(out_backend);
	if (mock_spy->next_ret == STREAM_STATUS_OK) {
		*out_backend = mock_spy->next_out_backend_value;
	}
	return mock_spy->next_ret;
}

stream_status_t stream_fake_adapter_create_provider(
	stream_adapter_provider_t **out,
	stream_fake_adapter_backend_t *backend,
	const osal_mem_ops_t *mem,
	stream_fake_adapter_backend_ctor_mock_spy_t *ctor_mock_spy
) {
	assert_non_null(out);
	assert_non_null(backend);
	assert_non_null(mem);
	assert_non_null(mem->calloc);
	assert_non_null(mem->free);
	assert_non_null(ctor_mock_spy);
	stream_adapter_provider_t *tmp = mem->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return STREAM_STATUS_OOM;
	}
	ctor_mock_spy->next_out_backend_value = backend;
	ctor_mock_spy->next_ret = STREAM_STATUS_OK;
	ctor_mock_spy->call_count = 0;
	tmp->ud = ctor_mock_spy;
	tmp->ud_dtor = NULL;
	tmp->backend_ctor = stream_fake_adapter_backend_ctor;
	tmp->vtbl = &stream_fake_adapter_vtbl;
	tmp->mem = mem;
	*out = tmp;
	return STREAM_STATUS_OK;
}

void stream_fake_adapter_destroy_provider(
	stream_adapter_provider_t **fake_provider
) {
	if (!fake_provider || !*fake_provider) {
		return;
	}
	assert_non_null((*fake_provider)->mem);
	assert_non_null((*fake_provider)->mem->free);
	(*fake_provider)->mem->free(*fake_provider);
	*fake_provider = NULL;
}
