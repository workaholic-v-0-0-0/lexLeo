/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_fake_provider.h
 * @ingroup stream_tests_group
 * @brief Fake stream provider used by `stream` tests.
 *
 * @details
 * This header exposes a lightweight fake backend used to exercise the public
 * `stream` port in tests.
 *
 * It provides:
 * - creation and destruction helpers for a fake-backed `stream_t`,
 * - runtime control over fake write/flush outcomes,
 * - call counters for behavioral assertions,
 * - access to bytes written through the fake stream.
 */

#ifndef LEXLEO_STREAM_FAKE_PROVIDER_H
#define LEXLEO_STREAM_FAKE_PROVIDER_H

#include "stream/borrowers/stream_types.h"

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque fake stream state.
 */
typedef struct stream_fake_t stream_fake_t;

/**
 * @brief Call counters recorded by the fake stream.
 */
typedef struct stream_fake_counters_t {
	size_t read_calls;
	size_t write_calls;
	size_t flush_calls;
	size_t close_calls;
} stream_fake_counters_t;

/**
 * @brief Create a fake stream backend and its associated public `stream_t`.
 *
 * @param[out] out_fake
 * Receives the created fake backend.
 *
 * @param[out] out_stream
 * Receives the created public stream handle backed by the fake.
 *
 * @param[in] mem
 * Borrowed memory operations used for allocation.
 *
 * @retval STREAM_STATUS_OK
 * Fake backend and public stream successfully created.
 *
 * @retval STREAM_STATUS_INVALID
 * One or more arguments are invalid.
 *
 * @retval STREAM_STATUS_OOM
 * Allocation failed.
 */
stream_status_t stream_fake_create(
	stream_fake_t **out_fake,
	stream_t **out_stream,
	const osal_mem_ops_t *mem);

/**
 * @brief Destroy a fake stream backend and its associated public stream.
 *
 * @param[in,out] fake
 * Fake backend handle to destroy.
 *
 * @param[in,out] stream
 * Public stream handle to destroy.
 */
void stream_fake_destroy(
	stream_fake_t **fake,
	stream_t **stream);

/**
 * @brief Reset the fake stream runtime state and counters.
 *
 * @param[in,out] fake
 * Fake stream to reset.
 */
void stream_fake_reset(stream_fake_t *fake);

/**
 * @brief Configure the result returned by the next write operations.
 *
 * @param[in,out] fake
 * Fake stream to configure.
 *
 * @param[in] n
 * Number of bytes the fake write should report as written.
 *
 * @param[in] status
 * Stream status reported by the fake write.
 */
void stream_fake_set_write_result(
	stream_fake_t *fake,
	size_t n,
	stream_status_t status);

/**
 * @brief Configure the result returned by flush operations.
 *
 * @param[in,out] fake
 * Fake stream to configure.
 *
 * @param[in] status
 * Stream status reported by the fake flush.
 */
void stream_fake_set_flush_result(
	stream_fake_t *fake,
	stream_status_t status);

/**
 * @brief Make fake writes fail starting from a given call index.
 *
 * @param[in,out] fake
 * Fake stream to configure.
 *
 * @param[in] call_idx
 * First write-call index that must fail.
 *
 * @param[in] status
 * Stream status reported for failing writes.
 */
void stream_fake_fail_write_since(
	stream_fake_t *fake,
	size_t call_idx,
	stream_status_t status);

/**
 * @brief Return the current call counters recorded by the fake stream.
 *
 * @param[in] fake
 * Fake stream to inspect.
 *
 * @return
 * Pointer to the fake counters.
 */
const stream_fake_counters_t *stream_fake_counters(const stream_fake_t *fake);

/**
 * @brief Return the number of bytes captured by the fake stream.
 *
 * @param[in] fake
 * Fake stream to inspect.
 *
 * @return
 * Number of bytes written into the fake capture buffer.
 */
size_t stream_fake_written_len(const stream_fake_t *fake);

/**
 * @brief Return the captured bytes written through the fake stream.
 *
 * @param[in] fake
 * Fake stream to inspect.
 *
 * @return
 * Pointer to the fake captured byte buffer.
 */
const uint8_t *stream_fake_written_data(const stream_fake_t *fake);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_STREAM_FAKE_PROVIDER_H
