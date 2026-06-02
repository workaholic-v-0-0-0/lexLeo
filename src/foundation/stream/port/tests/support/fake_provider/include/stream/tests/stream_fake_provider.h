/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_fake_provider.h
 * @ingroup stream_tests_group
 * @brief Fake stream dependency provider for tests requiring `stream`
 * dependencies.
 *
 * @details
 * This header exposes fake stream creators ready to be injected in tests.
 * It also includes the underlying `fake_stream` API so tests can configure
 * and inspect prepared fake stream backends.
 */

#ifndef LEXLEO_STREAM_FAKE_PROVIDER_H
#define LEXLEO_STREAM_FAKE_PROVIDER_H

#include "stream/owners/stream_creators_api.h"
#include "stream/adapters/stream_adapters_types.h"

#include "lexleo/test/fake_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

stream_status_t stream_test_fake_provider_setup(
	const osal_mem_ops_t *mem,
	stream_key_t buffer_key,
	stream_key_t file_key,
	stream_key_t io_key);
void stream_test_fake_provider_teardown(void);

const stream_factory_t *stream_test_fake_stream_factory(void);
const stream_buffer_creator_t *stream_test_fake_buffer_creator(void);
const stream_file_creator_t *stream_test_fake_file_creator(void);
const stream_io_creator_t *stream_test_fake_io_creator(void);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_FAKE_PROVIDER_H */
