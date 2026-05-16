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

#include "stream/adapters/stream_env.h"
#include "stream/borrowers/stream_types.h"
#include "stream/owners/stream_buffer_creator.h"
#include "stream/owners/stream_file_creator.h"
#include "stream/owners/stream_io_creator.h"

#include "policy/lexleo_cstd_types.h"

#define FAKE_STREAM_BUF_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

/* FAKE API */

const stream_buffer_creator_t *fake_stream_buffer_creator(void);
const stream_file_creator_t *fake_stream_file_creator(void);
const stream_io_creator_t *fake_stream_io_creator(void);

/* CFG */

void fake_stream_reset(const stream_env_t *env);
void fake_stream_prepare_next_backend(void *backend);

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
const void *fake_stream_last_file_create_ud(void);
const char *fake_stream_last_file_create_path(void);
uint32_t fake_stream_last_file_create_flags(void);
bool fake_stream_last_file_create_autoclose(void);
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
