/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_stream_adapter.h
 * @ingroup test_support_fake_stream_adapter
 * @brief Fake stream adapter support for unit tests.
 *
 * @details
 * This header declares fake `stream` adapter providers, configurable fake
 * backends, constructor controls, and helper functions for use in tests.
 *
 * Tests can configure read, write, flush, close, and backend-construction
 * behavior, prepare buffered and sink data, control the read position, inspect
 * operation call counts and arguments, and observe creator arguments passed to
 * dynamic-buffer, regular-file, and standard-stream adapter providers.
 */

#ifndef LEXLEO_FAKE_STREAM_ADAPTER_H
#define LEXLEO_FAKE_STREAM_ADAPTER_H

#include "stream/adapters/stream_adapters_types.h"
#include "stream/adapters/stream_adapters_regular_file_contract.h"
#include "stream/adapters/stream_adapters_standard_stream_contract.h"

#define FAKE_STREAM_ADAPTER_BUF_SIZE 1024
#define FAKE_STREAM_ADAPTER_MAX_SEQ_LEN 256

#ifdef __cplusplus
extern "C" {
#endif

extern const stream_vtbl_t *g_fake_stream_adapter_vtbl;

typedef struct fake_stream_adapter_t {

	/* cfg */
	size_t next_read_ret;
	stream_status_t next_read_st_value;
	bool write_all;
	size_t next_write_ret;
	stream_status_t next_write_st_value;
	stream_status_t next_flush_ret;
	stream_status_t next_close_ret;
	bool end_of_input_has_been_signaled;

	/* spy */
	size_t read_call_count;
	void *last_read_buf;
	size_t last_read_n;
	stream_status_t *last_read_st;
	size_t write_call_count;
	const void *last_write_buf;
	size_t last_write_n;
	stream_status_t *last_write_st;
	size_t flush_call_count;
	size_t close_call_count;

	/* state */
	uint8_t buffered_backing[FAKE_STREAM_ADAPTER_BUF_SIZE];
	uint8_t sink_backing[FAKE_STREAM_ADAPTER_BUF_SIZE];
	size_t buffered_len;
	size_t sink_len;
	size_t read_pos;
	bool end_of_input_has_been_encountered;

} fake_stream_adapter_t;

typedef enum fake_stream_adapter_args_kind_t {
	FAKE_STREAM_ADAPTER_ARGS_NONE = 0,
	FAKE_STREAM_ADAPTER_DYNAMIC_BUFFER,
	FAKE_STREAM_ADAPTER_REGULAR_FILE,
	FAKE_STREAM_ADAPTER_STANDARD_STREAM
} fake_stream_adapter_args_kind_t;

typedef struct fake_stream_adapter_ctrl_t {

	/* cfg */
	stream_status_t next_stream_adapter_ctor_ret;
	fake_stream_adapter_t next_fake_stream_adapter;

	/* spy */
	fake_stream_adapter_args_kind_t last_args_kind;
	const char *last_path;
	const char *last_mode;
	stream_standard_stream_kind_t last_kind;

} fake_stream_adapter_ctrl_t;

extern fake_stream_adapter_ctrl_t g_fake_stream_adapter_ctrl_seq[FAKE_STREAM_ADAPTER_MAX_SEQ_LEN];

extern stream_adapter_provider_t g_fake_stream_adapter_provider_dynamic_buffer;
extern stream_adapter_provider_t g_fake_stream_adapter_provider_regular_file;
extern stream_adapter_provider_t g_fake_stream_adapter_provider_standard_stream;

void fake_stream_adapter_reset_instance(fake_stream_adapter_t *fake_stream_adapter);
void fake_stream_adapter_reset(void);

void fake_stream_adapter_set_buffered_backing(
	fake_stream_adapter_t *fake_stream_adapter,
	const uint8_t *backing,
	size_t len);
void fake_stream_adapter_set_sink_backing(
	fake_stream_adapter_t *fake_stream_adapter,
	const uint8_t *backing,
	size_t len);
void fake_stream_adapter_set_read_pos(
	fake_stream_adapter_t *fake_stream_adapter,
	size_t n);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FAKE_STREAM_ADAPTER_H */
