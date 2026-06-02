/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_stream.h
 * @ingroup test_support_fake_stream
 * @brief Fake stream backend for unit tests.
 *
 * @details
 * This header declares fake stream operations and helper functions used by
 * unit tests.
 */

#ifndef LEXLEO_FAKE_STREAM_H
#define LEXLEO_FAKE_STREAM_H

#include "stream/borrowers/stream_borrowers_types.h"
#include "stream/adapters/stream_adapters_api.h"
#include "stream/owners/stream_creators_api.h"

#include "policy/lexleo_cstd_types.h"

#define FAKE_STREAM_BUF_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

/* FAKE API */

/*
 * No injectable stream service.
 *
 * Stream behavior is injected through the backend-specific vtable
 * bound to each stream instance.
 */

/* CFG */

/**
 * @brief Build fake stream adapter descriptors for tests.
 *
 * @details
 * The produced descriptors are intended to be registered into a real
 * `stream_factory_t` with `stream_factory_add_adapter()`.
 *
 * Once registered, real stream creators can be bound to the same factory and
 * key, so tests exercise the normal creator/factory path while receiving
 * streams backed by the fake backend and fake vtable.
 */
void fake_stream_create_buffer_desc(
	stream_adapter_desc_t *out,
	stream_key_t key);

void fake_stream_create_file_desc(
	stream_adapter_desc_t *out,
	stream_key_t key);

void fake_stream_create_io_desc(
	stream_adapter_desc_t *out,
	stream_key_t key);

void fake_stream_reset(const stream_env_t *env);
void fake_stream_prepare_next_backend(void *backend);

/**
 * @brief Create a fake stream backend instance.
 *
 * @return
 * Newly allocated fake backend object.
 *
 * @details
 * The returned backend can be bound to a real `stream_t` through
 * `stream_create()`.
 *
 * This allows tests to exercise the public `stream` API while all backend
 * operations are handled by the fake implementation.
 */
void *fake_stream_create_fake_backend(void);

void fake_stream_destroy_fake_backend(void *fake);

void fake_stream_backend_reset(void *fake);
void fake_stream_set_buffered_backing(
	void *backend,
	const uint8_t *data,
	size_t len);
void fake_stream_set_sink_backing(
	void *backend,
	const uint8_t *data,
	size_t len);
void fake_stream_set_pos(void *backend, size_t pos);

void fake_stream_set_noop_read(void *backend, bool noop_read);
void fake_stream_set_read_status(void *backend, stream_status_t st);

void fake_stream_set_noop_write(void *backend, bool noop_write);
void fake_stream_set_write_status(void *backend, stream_status_t st);

void fake_stream_set_noop_flush(void *backend, bool noop_flush);
void fake_stream_set_flush_status(void *backend, stream_status_t st);

void fake_stream_set_close_status(void *backend, stream_status_t st);

/* SPY */

size_t fake_stream_buffer_create_call_count(void);
const void *fake_stream_last_buffer_create_ud(void);
stream_t **fake_stream_last_buffer_create_out(void);

size_t fake_stream_file_create_call_count(void);
const char *fake_stream_last_file_create_path(void);
const char *fake_stream_last_file_create_mode(void);
stream_t **fake_stream_last_file_create_out(void);

size_t fake_stream_io_create_call_count(void);
const void *fake_stream_last_io_create_ud(void);
stream_io_kind_t fake_stream_last_io_create_kind(void);
stream_t **fake_stream_last_io_create_out(void);

bool fake_stream_is_open(void *fake);

size_t fake_stream_read_call_count(void *fake);
void *fake_stream_last_read_backend(void *fake);
void *fake_stream_last_read_buf(void *fake);
size_t fake_stream_last_read_n(void *fake);
stream_status_t *fake_stream_last_read_st(void *fake);

size_t fake_stream_write_call_count(void *fake);
void *fake_stream_last_write_backend(void *fake);
const void *fake_stream_last_write_buf(void *fake);
size_t fake_stream_last_write_n(void *fake);
stream_status_t *fake_stream_last_write_st(void *fake);

size_t fake_stream_flush_call_count(void *fake);
void *fake_stream_last_flush_backend(void *fake);

size_t fake_stream_close_call_count(void *fake);
void *fake_stream_last_close_backend(void *fake);

const uint8_t *fake_stream_buffered_backing(void *fake);
const uint8_t *fake_stream_sink_backing(void *fake);
size_t fake_stream_buffered_len(void *fake);
size_t fake_stream_sink_len(void *fake);
size_t fake_stream_pos(void *fake);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FAKE_STREAM_H */
