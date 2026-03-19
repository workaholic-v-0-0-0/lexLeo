#include "lexleo/test/fake_stream.h"

#include "stream/lifecycle/stream_lifecycle.h"
#include "stream/borrowers/stream.h"
#include "policy/lexleo_assert.h"

#include <string.h>

#ifndef FAKE_STREAM_BUFFER_CAP
#define FAKE_STREAM_BUFFER_CAP 4096u
#endif

typedef struct fake_stream_backend_t {
	uint8_t buf[FAKE_STREAM_BUFFER_CAP];
	size_t len;
	bool is_open;

	size_t write_result_n;
	stream_status_t write_result_status;

	bool fail_write_enabled;
	size_t fail_write_since_call;
	stream_status_t fail_write_status;

	stream_status_t flush_result_status;

	stream_env_t env;
} fake_stream_backend_t;

static fake_stream_counters_t g_counters;

static struct {
	size_t len;
	uint8_t buf[FAKE_STREAM_BUFFER_CAP];
} g_written;

static struct {
	size_t write_result_n;
	stream_status_t write_result_status;

	bool fail_write_enabled;
	size_t fail_write_since_call;
	stream_status_t fail_write_status;

	stream_status_t flush_result_status;
} g_cfg;

static size_t fake_stream_read(
	void *backend,
	void *buf,
	size_t n,
	stream_status_t *status)
{
	(void)backend;
	(void)buf;
	(void)n;

	g_counters.read_calls++;

	if (status) {
		*status = STREAM_STATUS_OK;
	}

	return 0;
}

static size_t fake_stream_write(
	void *backend,
	const void *buf,
	size_t n,
	stream_status_t *status)
{
	fake_stream_backend_t *b = (fake_stream_backend_t *)backend;

	g_counters.write_calls++;

	if (!b || !b->is_open || (!buf && n != 0u)) {
		if (status) {
			*status = STREAM_STATUS_INVALID;
		}
		return 0u;
	}

	if (b->fail_write_enabled &&
		g_counters.write_calls >= b->fail_write_since_call) {
		if (status) {
			*status = b->fail_write_status;
		}
		return 0u;
	}

	size_t wanted = b->write_result_n;
	if (wanted > n) {
		wanted = n;
	}

	size_t space = FAKE_STREAM_BUFFER_CAP - b->len;
	size_t written = wanted;
	if (written > space) {
		written = space;
	}

	if (written > 0u) {
		memcpy(b->buf + b->len, buf, written);
		memcpy(g_written.buf + g_written.len, buf, written);
		b->len += written;
		g_written.len += written;
	}

	if (status) {
		*status = b->write_result_status;
	}

	return written;
}

static stream_status_t fake_stream_flush(void *backend)
{
	fake_stream_backend_t *b = (fake_stream_backend_t *)backend;

	g_counters.flush_calls++;

	if (!b || !b->is_open) {
		return STREAM_STATUS_INVALID;
	}

	return b->flush_result_status;
}

static stream_status_t fake_stream_close(void *backend)
{
	fake_stream_backend_t *b = (fake_stream_backend_t *)backend;

	g_counters.close_calls++;

	if (!b) {
		return STREAM_STATUS_INVALID;
	}

	b->is_open = false;

	LEXLEO_ASSERT(b->env.mem != NULL);
	LEXLEO_ASSERT(b->env.mem->free != NULL);
	b->env.mem->free(b);

	return STREAM_STATUS_OK;
}

static const stream_vtbl_t g_fake_stream_vtbl = {
	.read = fake_stream_read,
	.write = fake_stream_write,
	.flush = fake_stream_flush,
	.close = fake_stream_close,
};

void fake_stream_reset(void)
{
	memset(&g_counters, 0, sizeof(g_counters));
	memset(&g_written, 0, sizeof(g_written));
	memset(&g_cfg, 0, sizeof(g_cfg));

	g_cfg.write_result_status = STREAM_STATUS_OK;
	g_cfg.flush_result_status = STREAM_STATUS_OK;
}

void fake_stream_set_write_result(size_t n, stream_status_t status)
{
	g_cfg.write_result_n = n;
	g_cfg.write_result_status = status;
}

void fake_stream_set_flush_result(stream_status_t status)
{
	g_cfg.flush_result_status = status;
}

void fake_stream_fail_write_since(size_t call_idx, stream_status_t status)
{
	g_cfg.fail_write_enabled = true;
	g_cfg.fail_write_since_call = (call_idx == 0u) ? 1u : call_idx;
	g_cfg.fail_write_status = status;
}

const fake_stream_counters_t *fake_stream_counters(void)
{
	return &g_counters;
}

size_t fake_stream_written_len(void)
{
	return g_written.len;
}

const uint8_t *fake_stream_written_data(void)
{
	return g_written.buf;
}

stream_status_t fake_stream_create(
	stream_t **out,
	const stream_env_t *env)
{
	if (!out || !env || !env->mem || !env->mem->calloc || !env->mem->free) {
		return STREAM_STATUS_INVALID;
	}

	fake_stream_backend_t *backend =
		(fake_stream_backend_t *)env->mem->calloc(1u, sizeof(*backend));
	if (!backend) {
		return STREAM_STATUS_OOM;
	}

	backend->is_open = true;
	backend->write_result_n = g_cfg.write_result_n;
	backend->write_result_status = g_cfg.write_result_status;
	backend->fail_write_enabled = g_cfg.fail_write_enabled;
	backend->fail_write_since_call = g_cfg.fail_write_since_call;
	backend->fail_write_status = g_cfg.fail_write_status;
	backend->flush_result_status = g_cfg.flush_result_status;
	backend->env = *env;

	return stream_create(out, &g_fake_stream_vtbl, backend, env);
}
