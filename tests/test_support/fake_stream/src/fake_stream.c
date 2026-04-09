/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "lexleo/test/fake_stream.h"

#include "stream/borrowers/stream.h"
#include "stream/lifecycle/stream_lifecycle.h"

#include "osal/mem/osal_mem.h"

#include "policy/lexleo_assert.h"

struct fake_stream_t {
	uint8_t written_buf[FAKE_STREAM_BUFFER_CAP];
	size_t written_len;
	bool is_open;

	size_t write_result_n;
	stream_status_t write_result_status;

	bool fail_write_enabled;
	size_t fail_write_since_call;
	stream_status_t fail_write_status;

	stream_status_t flush_result_status;

	fake_stream_counters_t counters;
	stream_env_t env;

	bool write_result_configured;
};

void fake_stream_reset(fake_stream_t *fs)
{
	if (!fs) {
		return;
	}

	osal_memset(&fs->counters, 0, sizeof(fs->counters));
	osal_memset(fs->written_buf, 0, sizeof(fs->written_buf));
	fs->written_len = 0u;

	fs->write_result_n = 0u;
	fs->write_result_status = STREAM_STATUS_OK;

	fs->fail_write_enabled = false;
	fs->fail_write_since_call = 0u;
	fs->fail_write_status = STREAM_STATUS_OK;

	fs->flush_result_status = STREAM_STATUS_OK;

	fs->write_result_configured = false;
}

static size_t fake_stream_read(
	void *backend,
	void *buf,
	size_t n,
	stream_status_t *status)
{
	(void)buf;
	(void)n;

	fake_stream_t *fs = (fake_stream_t *)backend;
	if (!fs || !fs->is_open) {
		if (status) {
			*status = STREAM_STATUS_INVALID;
		}
		return 0u;
	}

	fs->counters.read_calls++;

	if (status) {
		*status = STREAM_STATUS_EOF;
	}

	return 0u;
}

static size_t fake_stream_write(
	void *backend,
	const void *buf,
	size_t n,
	stream_status_t *status)
{
	fake_stream_t *fs = (fake_stream_t *)backend;

	if (!fs || !fs->is_open || (!buf && n != 0u)) {
		if (status) {
			*status = STREAM_STATUS_INVALID;
		}
		return 0u;
	}

	fs->counters.write_calls++;

	if (fs->fail_write_enabled &&
		fs->counters.write_calls >= fs->fail_write_since_call) {
		if (status) {
			*status = fs->fail_write_status;
		}
		return 0u;
	}

	size_t wanted = 0u;
	stream_status_t ret_status = STREAM_STATUS_OK;

	if (fs->write_result_configured) {
		wanted = fs->write_result_n;
		ret_status = fs->write_result_status;
	} else {
		wanted = n;
		ret_status = STREAM_STATUS_OK;
	}

	if (wanted > n) {
		wanted = n;
	}

	size_t space = FAKE_STREAM_BUFFER_CAP - fs->written_len;
	size_t written = wanted;
	if (written > space) {
		written = space;
	}

	if (written > 0u) {
		osal_memcpy(fs->written_buf + fs->written_len, buf, written);
		fs->written_len += written;
	}

	if (status) {
		*status = ret_status;
	}

	return written;
}

static stream_status_t fake_stream_flush(void *backend)
{
	fake_stream_t *fs = (fake_stream_t *)backend;

	if (!fs || !fs->is_open) {
		return STREAM_STATUS_INVALID;
	}

	fs->counters.flush_calls++;
	return fs->flush_result_status;
}

static stream_status_t fake_stream_close(void *backend)
{
	fake_stream_t *fs = (fake_stream_t *)backend;

	if (!fs) {
		return STREAM_STATUS_INVALID;
	}

	fs->counters.close_calls++;
	fs->is_open = false;

	LEXLEO_ASSERT(fs->env.mem != NULL);
	LEXLEO_ASSERT(fs->env.mem->free != NULL);
	fs->env.mem->free(fs);

	return STREAM_STATUS_OK;
}

static const stream_vtbl_t g_fake_stream_vtbl = {
	.read = fake_stream_read,
	.write = fake_stream_write,
	.flush = fake_stream_flush,
	.close = fake_stream_close,
};

void fake_stream_set_write_result(
	fake_stream_t *fs,
	size_t n,
	stream_status_t status)
{
	if (!fs) {
		return;
	}

	fs->write_result_n = n;
	fs->write_result_status = status;
	fs->write_result_configured = true;
}

void fake_stream_set_flush_result(
	fake_stream_t *fs,
	stream_status_t status)
{
	if (!fs) {
		return;
	}

	fs->flush_result_status = status;
}

void fake_stream_fail_write_since(
	fake_stream_t *fs,
	size_t call_idx,
	stream_status_t status)
{
	if (!fs) {
		return;
	}

	fs->fail_write_enabled = true;
	fs->fail_write_since_call = (call_idx == 0u) ? 1u : call_idx;
	fs->fail_write_status = status;
}

const fake_stream_counters_t *fake_stream_counters(const fake_stream_t *fs)
{
	return fs ? &fs->counters : NULL;
}

size_t fake_stream_written_len(const fake_stream_t *fs)
{
	return fs ? fs->written_len : 0u;
}

const uint8_t *fake_stream_written_data(const fake_stream_t *fs)
{
	return fs ? fs->written_buf : NULL;
}

stream_status_t fake_stream_create(
	fake_stream_t **out_fake,
	stream_t **out_stream,
	const stream_env_t *env)
{
	if (out_fake) {
		*out_fake = NULL;
	}

	if (out_stream) {
		*out_stream = NULL;
	}

	if (!out_fake || !out_stream ||
		!env || !env->mem || !env->mem->calloc || !env->mem->free) {
		return STREAM_STATUS_INVALID;
	}

	fake_stream_t *fs =
		(fake_stream_t *)env->mem->calloc(1u, sizeof(*fs));
	if (!fs) {
		return STREAM_STATUS_OOM;
	}

	fs->is_open = true;
	fs->env = *env;
	fake_stream_reset(fs);

	stream_t *stream = NULL;
	stream_status_t st = stream_create(&stream, &g_fake_stream_vtbl, fs, env);
	if (st != STREAM_STATUS_OK) {
		env->mem->free(fs);
		return st;
	}

	*out_fake = fs;
	*out_stream = stream;
	return STREAM_STATUS_OK;
}

void fake_stream_destroy(
	fake_stream_t **fake,
	stream_t **stream)
{
	if (stream && *stream) {
		stream_destroy(stream);
	}

	if (fake) {
		*fake = NULL;
	}
}
