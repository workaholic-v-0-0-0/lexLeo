/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_stream.c
 * @ingroup test_support_fake_stream
 * @brief Fake stream backend implementation for unit tests.
 *
 * @details
 * This file todo
 */

#include "lexleo/test/fake_stream.h"

#include "stream/adapters/stream_adapters_api.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_assert.h"

/* FAKE API */

typedef struct fake_stream_backend_t
{
	/* state */

	bool is_open;
	const osal_mem_ops_t *mem_ops;
	uint8_t buffered_backing[FAKE_STREAM_BUF_SIZE];
	uint8_t sink_backing[FAKE_STREAM_BUF_SIZE];
	size_t buffered_len;
	size_t sink_len;
	size_t pos;

	/* cfg */

	bool noop_read;
	stream_status_t read_status;

	bool noop_write;
	stream_status_t write_status;

	bool noop_flush;
	stream_status_t flush_status;

	stream_status_t close_status;

	/* spy */

	size_t read_call_count;
	void *last_read_backend;
	void *last_read_buf;
	size_t last_read_n;
	stream_status_t *last_read_st;

	size_t write_call_count;
	void *last_write_backend;
	const void *last_write_buf;
	size_t last_write_n;
	stream_status_t *last_write_st;

	size_t flush_call_count;
	void *last_flush_backend;

	size_t close_call_count;
	void *last_close_backend;

} fake_stream_backend_t;

typedef struct fake_stream_ctrl_t
{
	/* cfg */

	stream_env_t env;

	void *next_backend;

	stream_status_t buffer_create_status;
	stream_status_t file_create_status;
	stream_status_t io_create_status;

	/* spy */

	size_t buffer_create_call_count;
	stream_t **last_buffer_create_out;

	size_t file_create_call_count;
	const char *last_file_create_path;
	const char *last_file_create_mode;
	stream_t **last_file_create_out;

	size_t io_create_call_count;
	stream_io_kind_t last_io_create_kind;
	stream_t **last_io_create_out;
} fake_stream_ctrl_t;

static fake_stream_ctrl_t g_fake_stream_ctrl = {0};

static fake_stream_backend_t *fake_stream_backend_real_to_fake(
	void *backend
) {
	return (fake_stream_backend_t *)backend;
}

static void *fake_stream_backend_fake_to_real(
	fake_stream_backend_t *fake
) {
	return (void *)fake;
}

static size_t fake_stream_read(
	void *backend,
	void *buf,
	size_t n,
	stream_status_t *st
) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);

	fake_backend->read_call_count++;
	fake_backend->last_read_backend = backend;
	fake_backend->last_read_buf = buf;
	fake_backend->last_read_n = n;
	fake_backend->last_read_st = st;

	if (fake_backend->noop_read) {
		if (st) *st = STREAM_STATUS_OK;
		return 0;
	}

	if (st) *st = fake_backend->read_status;

	if (fake_backend->read_status != STREAM_STATUS_OK) {
		return 0;
	}

	if (n == 0) {
		return 0;
	}

	LEXLEO_ASSERT(
		   buf
		&& fake_backend->is_open
		&& fake_backend->buffered_len <= FAKE_STREAM_BUF_SIZE
		&& fake_backend->pos <= fake_backend->buffered_len
	);

	size_t available_len = fake_backend->buffered_len - fake_backend->pos;
	size_t read_len = (available_len >= n) ? n : available_len;
	if (read_len > 0) {
		osal_memcpy(
			buf,
			fake_backend->buffered_backing + fake_backend->pos,
			read_len
		);
		fake_backend->pos += read_len;
	}

	return read_len;
}

static size_t fake_stream_write(
	void *backend,
	const void *buf,
	size_t n,
	stream_status_t *st
) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);

	fake_backend->write_call_count++;
	fake_backend->last_write_backend = backend;
	fake_backend->last_write_buf = buf;
	fake_backend->last_write_n = n;
	fake_backend->last_write_st = st;

	if (fake_backend->noop_write) {
		if (st) *st = STREAM_STATUS_OK;
		return 0;
	}

	if (st) *st = fake_backend->write_status;

	if (fake_backend->write_status != STREAM_STATUS_OK) {
		return 0;
	}

	if (n == 0) {
		return 0;
	}

	LEXLEO_ASSERT(
		   buf
		&& fake_backend->is_open
		&& fake_backend->buffered_len <= FAKE_STREAM_BUF_SIZE
		&& fake_backend->pos <= fake_backend->buffered_len
		&& n <= FAKE_STREAM_BUF_SIZE - fake_backend->pos
	);

	osal_memcpy(
		fake_backend->buffered_backing + fake_backend->pos,
		buf,
		n
	);
	fake_backend->pos += n;
	if (fake_backend->pos > fake_backend->buffered_len) {
		fake_backend->buffered_len = fake_backend->pos;
	}

	return n;
}

static stream_status_t fake_stream_flush(void *backend)
{
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);

	fake_backend->flush_call_count++;
	fake_backend->last_flush_backend = backend;

	if (fake_backend->noop_flush) {
		return STREAM_STATUS_OK;
	}

	if (fake_backend->flush_status != STREAM_STATUS_OK) {
		return fake_backend->flush_status;
	}

	LEXLEO_ASSERT(
		   fake_backend->is_open
		&& fake_backend->buffered_len <= FAKE_STREAM_BUF_SIZE
		&& fake_backend->sink_len <= FAKE_STREAM_BUF_SIZE
	);

	osal_memset(fake_backend->sink_backing, 0, FAKE_STREAM_BUF_SIZE);
	osal_memcpy(
		fake_backend->sink_backing,
		fake_backend->buffered_backing,
		fake_backend->buffered_len
	);
	fake_backend->sink_len = fake_backend->buffered_len;

	return STREAM_STATUS_OK;
}

static stream_status_t fake_stream_close(void *backend
) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);

	fake_backend->close_call_count++;
	fake_backend->last_close_backend = backend;

	if (fake_backend->close_status != STREAM_STATUS_OK) {
		return fake_backend->close_status;
	}

	if (!fake_backend->noop_flush) {
		stream_status_t flush_st = fake_stream_flush(backend);
		if (flush_st != STREAM_STATUS_OK) {
			return flush_st;
		}
	}

	osal_memset(fake_backend->buffered_backing, 0, FAKE_STREAM_BUF_SIZE);
	fake_backend->buffered_len = 0;
	fake_backend->pos = 0;
	fake_backend->is_open = false;

	fake_backend->mem_ops->free(fake_backend);

	return STREAM_STATUS_OK;
}

static const stream_vtbl_t g_fake_stream_vtbl = {
	.read = fake_stream_read,
	.write = fake_stream_write,
	.flush = fake_stream_flush,
	.close = fake_stream_close
};

stream_status_t fake_stream_create_stream(stream_t **out)
{
	if (!out) {
		return STREAM_STATUS_INVALID;
	}

	void *fake_backend = g_fake_stream_ctrl.next_backend;
	bool backend_was_prepared = fake_backend != NULL;

	if (!fake_backend) {
		fake_backend = fake_stream_create_fake_backend();
		if (!fake_backend) {
			return STREAM_STATUS_OOM;
		}
	}

	stream_t *tmp = NULL;

	stream_status_t st =
		stream_create(&tmp, &g_fake_stream_ctrl.env);

	if (st != STREAM_STATUS_OK) {
		/*
		 * An automatically created backend is owned locally and must
		 * therefore be released on failure.
		 *
		 * A prepared backend remains owned by the test and remains prepared
		 * so that creation may be retried.
		 */
		if (!backend_was_prepared) {
			fake_stream_destroy_fake_backend(fake_backend);
		}

		return st;
	}

	st = stream_complete_default_init(tmp, fake_backend);
	if (st != STREAM_STATUS_OK) {
		stream_status_t destroy_st = stream_destroy(&tmp);
		LEXLEO_ASSERT(destroy_st == STREAM_STATUS_OK);

		if (!backend_was_prepared) {
			fake_stream_destroy_fake_backend(fake_backend);
		}

		return st;
	}

	fake_stream_backend_real_to_fake(fake_backend)->is_open = true;

	g_fake_stream_ctrl.next_backend = NULL;
	*out = tmp;

	return STREAM_STATUS_OK;
}

static stream_status_t fake_stream_dbs_ctor(
	const void *ud,
	const void *args,
	stream_t **out
) {
	(void)ud;
	LEXLEO_ASSERT(out);

	if (args) {
		return STREAM_STATUS_INVALID;
	}

	g_fake_stream_ctrl.buffer_create_call_count++;
	g_fake_stream_ctrl.last_buffer_create_out = out;

	if (g_fake_stream_ctrl.buffer_create_status != STREAM_STATUS_OK) {
		return g_fake_stream_ctrl.buffer_create_status;
	}

	return fake_stream_create_stream(out);
}

static stream_status_t fake_stream_file_ctor(
	const void *ud,
	const void *args,
	stream_t **out
) {
	(void)ud;
	LEXLEO_ASSERT(out);

	const stream_file_creator_args_t *file_args =
		(const stream_file_creator_args_t *)args;

	if (!file_args) {
		return STREAM_STATUS_INVALID;
	}

	g_fake_stream_ctrl.file_create_call_count++;
	g_fake_stream_ctrl.last_file_create_path = file_args->path;
	g_fake_stream_ctrl.last_file_create_mode = file_args->mode;
	g_fake_stream_ctrl.last_file_create_out = out;

	if (g_fake_stream_ctrl.file_create_status != STREAM_STATUS_OK) {
		return g_fake_stream_ctrl.file_create_status;
	}

	return fake_stream_create_stream(out);
}

static stream_status_t fake_stream_io_ctor(
	const void *ud,
	const void *args,
	stream_t **out
) {
	(void)ud;
	LEXLEO_ASSERT(out);

	const stream_io_kind_t *kind =
		(const stream_io_kind_t *)args;

	if (!kind) {
		return STREAM_STATUS_INVALID;
	}

	g_fake_stream_ctrl.io_create_call_count++;
	g_fake_stream_ctrl.last_io_create_kind = *kind;
	g_fake_stream_ctrl.last_io_create_out = out;

	if (g_fake_stream_ctrl.io_create_status != STREAM_STATUS_OK) {
		return g_fake_stream_ctrl.io_create_status;
	}

	return fake_stream_create_stream(out);
}

void fake_stream_create_buffer_desc(
	stream_adapter_desc_t *out,
	stream_key_t key
) {
	LEXLEO_ASSERT(
		   out
		&& key
		&& *key != '\0'
	);

	*out = (stream_adapter_desc_t){
		.key = key,
		.ctor = fake_stream_dbs_ctor,
		.ud = NULL,
		.ud_dtor = NULL
	};
}

void fake_stream_create_file_desc(
	stream_adapter_desc_t *out,
	stream_key_t key
) {
	LEXLEO_ASSERT(
		   out
		&& key
		&& *key != '\0'
	);

	*out = (stream_adapter_desc_t){
		.key = key,
		.ctor = fake_stream_file_ctor,
		.ud = NULL,
		.ud_dtor = NULL
	};
}

void fake_stream_create_io_desc(
	stream_adapter_desc_t *out,
	stream_key_t key
) {
	LEXLEO_ASSERT(
		   out
		&& key
		&& *key != '\0'
	);

	*out = (stream_adapter_desc_t){
		.key = key,
		.ctor = fake_stream_io_ctor,
		.ud = NULL,
		.ud_dtor = NULL
	};
}

/* CFG */

void fake_stream_reset(const osal_mem_ops_t *mem_ops)
{
	LEXLEO_ASSERT(mem_ops);
	osal_memset(&g_fake_stream_ctrl, 0, sizeof(g_fake_stream_ctrl));
	g_fake_stream_ctrl.env = stream_default_env(&g_fake_stream_vtbl, mem_ops);
	g_fake_stream_ctrl.buffer_create_status = STREAM_STATUS_OK;
	g_fake_stream_ctrl.file_create_status = STREAM_STATUS_OK;
	g_fake_stream_ctrl.io_create_status = STREAM_STATUS_OK;
}

void fake_stream_prepare_next_backend(void *backend)
{
	g_fake_stream_ctrl.next_backend = backend;
}

static void fake_stream_backend_init(fake_stream_backend_t *fake)
{
	LEXLEO_ASSERT(fake);
	fake->mem_ops = g_fake_stream_ctrl.env.mem;
	fake->read_status = STREAM_STATUS_OK;
	fake->write_status = STREAM_STATUS_OK;
	fake->flush_status = STREAM_STATUS_OK;
	fake->close_status = STREAM_STATUS_OK;
}

void *fake_stream_create_fake_backend(void)
{
	LEXLEO_ASSERT(
		   g_fake_stream_ctrl.env.mem
		&& g_fake_stream_ctrl.env.mem->calloc);

	fake_stream_backend_t *ret =
		g_fake_stream_ctrl.env.mem->calloc(1, sizeof(*ret));
	if (!ret) {
		return NULL;
	}

	fake_stream_backend_init(ret);
	return fake_stream_backend_fake_to_real(ret);
}

void fake_stream_destroy_fake_backend(void *fake_backend)
{
	LEXLEO_ASSERT(fake_backend);
	fake_stream_backend_t *fake_backend_casted =
		fake_stream_backend_real_to_fake(fake_backend);
	LEXLEO_ASSERT(
		   fake_backend_casted->mem_ops
		&& fake_backend_casted->mem_ops->free);
	fake_backend_casted->mem_ops->free(fake_backend);
}

void fake_stream_backend_reset(void *fake) {
	LEXLEO_ASSERT(fake && g_fake_stream_ctrl.env.mem);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	osal_memset(fake_backend, 0, sizeof(*fake_backend));
	fake_backend->is_open = false;
	fake_backend->mem_ops = g_fake_stream_ctrl.env.mem;
	fake_backend->read_status = STREAM_STATUS_OK;
	fake_backend->write_status = STREAM_STATUS_OK;
	fake_backend->flush_status = STREAM_STATUS_OK;
	fake_backend->close_status = STREAM_STATUS_OK;
}

void fake_stream_set_buffered_backing(
	void *backend,
	const uint8_t *data,
	size_t len
) {
	LEXLEO_ASSERT(
		   backend
		&& (data || len == 0)
		&& len <= FAKE_STREAM_BUF_SIZE
	);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);

	osal_memset(fake_backend->buffered_backing, 0, FAKE_STREAM_BUF_SIZE);
	fake_backend->buffered_len = len;
	fake_backend->pos = 0;
	if (len > 0) {
		osal_memcpy(fake_backend->buffered_backing, data, len);
	}
}

void fake_stream_set_sink_backing(
	void *backend,
	const uint8_t *data,
	size_t len
) {
	LEXLEO_ASSERT(
		   backend
		&& (data || len == 0)
		&& len <= FAKE_STREAM_BUF_SIZE
	);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);

	osal_memset(fake_backend->sink_backing, 0, FAKE_STREAM_BUF_SIZE);
	fake_backend->sink_len = len;
	if (len > 0) {
		osal_memcpy(fake_backend->sink_backing, data, len);
	}
}

void fake_stream_set_pos(void *backend, size_t pos)
{
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);
	LEXLEO_ASSERT(pos <= fake_backend->buffered_len);
	fake_backend->pos = pos;
}

void fake_stream_set_noop_read(void *backend, bool noop_read) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);
	fake_backend->noop_read = noop_read;
}

void fake_stream_set_read_status(void *backend, stream_status_t st) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);
	fake_backend->read_status = st;
}

void fake_stream_set_noop_write(void *backend, bool noop_write) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);
	fake_backend->noop_write = noop_write;
}

void fake_stream_set_write_status(void *backend, stream_status_t st) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);
	fake_backend->write_status = st;
}

void fake_stream_set_noop_flush(void *backend, bool noop_flush) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);
	fake_backend->noop_flush = noop_flush;
}

void fake_stream_set_flush_status(void *backend, stream_status_t st) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);
	fake_backend->flush_status = st;
}

void fake_stream_set_close_status(void *backend, stream_status_t st) {
	LEXLEO_ASSERT(backend);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(backend);
	fake_backend->close_status = st;
}

/* SPY */

size_t fake_stream_buffer_create_call_count(void) {
	return g_fake_stream_ctrl.buffer_create_call_count;
}

stream_t **fake_stream_last_buffer_create_out(void) {
	return g_fake_stream_ctrl.last_buffer_create_out;
}

size_t fake_stream_file_create_call_count(void) {
	return g_fake_stream_ctrl.file_create_call_count;
}

const char *fake_stream_last_file_create_path(void) {
	return g_fake_stream_ctrl.last_file_create_path;
}

const char *fake_stream_last_file_create_mode(void) {
	return g_fake_stream_ctrl.last_file_create_mode;
}

stream_t **fake_stream_last_file_create_out(void) {
	return g_fake_stream_ctrl.last_file_create_out;
}

size_t fake_stream_io_create_call_count(void) {
	return g_fake_stream_ctrl.io_create_call_count;
}

stream_io_kind_t fake_stream_last_io_create_kind(void) {
	return g_fake_stream_ctrl.last_io_create_kind;
}

stream_t **fake_stream_last_io_create_out(void) {
	return g_fake_stream_ctrl.last_io_create_out;
}

bool fake_stream_is_open(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->is_open;
}

size_t fake_stream_read_call_count(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->read_call_count;
}

void *fake_stream_last_read_backend(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_read_backend;
}

void *fake_stream_last_read_buf(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_read_buf;
}

size_t fake_stream_last_read_n(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_read_n;
}

stream_status_t *fake_stream_last_read_st(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_read_st;
}

size_t fake_stream_write_call_count(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->write_call_count;
}

void *fake_stream_last_write_backend(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_write_backend;
}

const void *fake_stream_last_write_buf(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_write_buf;
}

size_t fake_stream_last_write_n(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_write_n;
}

stream_status_t *fake_stream_last_write_st(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_write_st;
}

size_t fake_stream_flush_call_count(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->flush_call_count;
}

void *fake_stream_last_flush_backend(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_flush_backend;
}

size_t fake_stream_close_call_count(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->close_call_count;
}

void *fake_stream_last_close_backend(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->last_close_backend;
}

const uint8_t *fake_stream_buffered_backing(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->buffered_backing;
}

const uint8_t *fake_stream_sink_backing(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->sink_backing;
}

size_t fake_stream_buffered_len(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->buffered_len;
}

size_t fake_stream_sink_len(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->sink_len;
}

size_t fake_stream_pos(void *fake) {
	LEXLEO_ASSERT(fake);
	fake_stream_backend_t *fake_backend = fake_stream_backend_real_to_fake(fake);
	return fake_backend->pos;
}
