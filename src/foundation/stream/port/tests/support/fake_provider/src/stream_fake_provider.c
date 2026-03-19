/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "internal/stream_fake_provider_internal.h"

#include "stream/test/stream_fake_provider.h"

#include "stream/adapters/stream_env.h"
#include "policy/lexleo_assert.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

struct stream_fake_t {
	fake_stream_t *impl;
	const osal_mem_ops_t *mem;
};

stream_status_t stream_fake_create(
	stream_fake_t **out_fake,
	stream_t **out_stream,
	const osal_mem_ops_t *mem)
{
	if (out_fake) {
		*out_fake = NULL;
	}
	if (out_stream) {
		*out_stream = NULL;
	}

	if (!out_fake || !out_stream || !mem) {
		return STREAM_STATUS_INVALID;
	}

	const stream_env_t env = {
		.mem = mem
	};

	fake_stream_t *impl = NULL;
	stream_t *stream = NULL;

	stream_status_t st = fake_stream_create(&impl, &stream, &env);
	if (st != STREAM_STATUS_OK) {
		return st;
	}

	stream_fake_t *fake =
		(stream_fake_t *)env.mem->calloc(1u, sizeof(*fake));
	if (!fake) {
		fake_stream_destroy(&impl, &stream);
		return STREAM_STATUS_OOM;
	}

	fake->impl = impl;
	fake->mem = mem;

	*out_fake = fake;
	*out_stream = stream;
	return STREAM_STATUS_OK;
}

void stream_fake_destroy(
	stream_fake_t **fake,
	stream_t **stream)
{
	fake_stream_t *impl = NULL;

	if (fake && *fake) {
		impl = (*fake)->impl;

		LEXLEO_ASSERT((*fake)->mem && (*fake)->mem->free);

		(*fake)->mem->free(*fake);
		*fake = NULL;
	}

	fake_stream_destroy(&impl, stream);
}

void stream_fake_reset(stream_fake_t *fake)
{
	if (!fake || !fake->impl) {
		return;
	}

	fake_stream_reset(fake->impl);
}

void stream_fake_set_write_result(
	stream_fake_t *fake,
	size_t n,
	stream_status_t status)
{
	if (!fake || !fake->impl) {
		return;
	}

	fake_stream_set_write_result(fake->impl, n, status);
}

void stream_fake_set_flush_result(
	stream_fake_t *fake,
	stream_status_t status)
{
	if (!fake || !fake->impl) {
		return;
	}

	fake_stream_set_flush_result(fake->impl, status);
}

void stream_fake_fail_write_since(
	stream_fake_t *fake,
	size_t call_idx,
	stream_status_t status)
{
	if (!fake || !fake->impl) {
		return;
	}

	fake_stream_fail_write_since(fake->impl, call_idx, status);
}

const stream_fake_counters_t *stream_fake_counters(const stream_fake_t *fake)
{
	if (!fake || !fake->impl) {
		return NULL;
	}

	return (const stream_fake_counters_t *)fake_stream_counters(fake->impl);
}

size_t stream_fake_written_len(const stream_fake_t *fake)
{
	if (!fake || !fake->impl) {
		return 0u;
	}

	return fake_stream_written_len(fake->impl);
}

const uint8_t *stream_fake_written_data(const stream_fake_t *fake)
{
	if (!fake || !fake->impl) {
		return NULL;
	}

	return fake_stream_written_data(fake->impl);
}