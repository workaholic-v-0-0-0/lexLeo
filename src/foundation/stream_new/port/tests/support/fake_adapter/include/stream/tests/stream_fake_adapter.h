/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_fake_adapter.h
 * @ingroup stream_tests_group
 * @brief Programmable stream adapter test double.
 *
 * @details
 * Declares the fake stream adapter backend, its configurable operation
 * results and spies, and helper operations used to construct adapter
 * providers for stream unit tests.
 */

#ifndef LEXLEO_STREAM_FAKE_ADAPTER_H
#define LEXLEO_STREAM_FAKE_ADAPTER_H

#include "stream/adapters/stream_adapters_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Dispatch table for the fake stream adapter.
 */
extern const stream_vtbl_t stream_fake_adapter_vtbl;

/**
 * @brief Programmable fake stream adapter backend.
 *
 * @details
 * Stores configuration controlling the values returned by fake backend
 * operations together with spies recording their invocations.
 */
typedef struct stream_fake_adapter_backend_t {

	/** Status returned by the next read operation. */
	stream_status_t next_read_st;

	/** Status returned by the next write operation. */
	stream_status_t next_write_st;

	/** Value returned by the next read operation. */
	size_t next_read_ret;

	/** Value returned by the next write operation. */
	size_t next_write_ret;

	/** Status returned by the next flush operation. */
	stream_status_t next_flush_ret;

	/** Status returned by the next close operation. */
	stream_status_t next_close_ret;

	/** Number of read operation calls. */
	size_t read_call_count;

	/** Number of write operation calls. */
	size_t write_call_count;

	/** Number of flush operation calls. */
	size_t flush_call_count;

	/** Number of close operation calls. */
	size_t close_call_count;

	/** Buffer passed to the most recent read operation. */
	void *last_read_buf;

	/** Buffer passed to the most recent write operation. */
	const void *last_write_buf;

	/** Byte count passed to the most recent read operation. */
	size_t last_read_n;

	/** Byte count passed to the most recent write operation. */
	size_t last_write_n;

	/** Backend passed to the most recent read operation. */
	void *last_read_backend;

	/** Backend passed to the most recent write operation. */
	void *last_write_backend;

	/** Backend passed to the most recent flush operation. */
	void *last_flush_backend;

	/** Backend passed to the most recent close operation. */
	void *last_close_backend;

	/** Status output passed to the most recent read operation. */
	stream_status_t *last_read_st;

	/** Status output passed to the most recent write operation. */
	stream_status_t *last_write_st;

} stream_fake_adapter_backend_t;

/**
 * @brief Reset a fake adapter backend to its default state.
 *
 * @param[in,out] b Fake adapter backend to reset.
 */
void stream_fake_adapter_init_backend(
	stream_fake_adapter_backend_t *b
);

/**
 * @brief Fake adapter backend constructor control and spy state.
 *
 * @details
 * Stores values used to configure the fake backend constructor together
 * with information recorded from its most recent invocation.
 */
typedef struct stream_fake_adapter_backend_ctor_mock_spy_t {

	/** Backend value produced when construction succeeds. */
	void *next_out_backend_value;

	/** Status returned by the next constructor call. */
	stream_status_t next_ret;

	/** Number of constructor calls. */
	size_t call_count;

	/** Arguments passed to the most recent constructor call. */
	const void *last_args_arg;

	/** Backend output address passed to the most recent constructor call. */
	void **last_out_backend_arg;

} stream_fake_adapter_backend_ctor_mock_spy_t;

/**
 * @brief Create an adapter provider for the fake stream adapter.
 *
 * @param[out] out Receives the created adapter provider.
 * @param[in,out] backend Backend instance returned by the adapter provider
 * constructor when the configured constructor status is `STREAM_STATUS_OK`.
 * @param[in] mem Memory operations used to allocate and destroy the adapter
 * provider.
 * @param[in,out] ctor_mock_spy Constructor control and spy object used to
 * configure and observe the fake adapter constructor. It is borrowed for the
 * lifetime of the created adapter provider.
 *
 * @return Creation status.
 */
stream_status_t stream_fake_adapter_create_provider(
	stream_adapter_provider_t **out,
	stream_fake_adapter_backend_t *backend,
	const osal_mem_ops_t *mem,
	stream_fake_adapter_backend_ctor_mock_spy_t *ctor_mock_spy
);

/**
 * @brief Destroy a fake stream adapter provider.
 *
 * @param[in,out] provider Address of the provider handle to destroy.
 */
void stream_fake_adapter_destroy_provider(
	stream_adapter_provider_t **provider
);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_FAKE_ADAPTER_H */
