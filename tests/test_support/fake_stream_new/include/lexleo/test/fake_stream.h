/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fake_stream.h
 * @ingroup test_support_fake_stream
 * @brief Fake stream support for unit tests.
 *
 * @details
 * This header declares reusable test support for constructing and resetting
 * `stream` instances backed by fake stream adapters.
 *
 * The module exposes dynamic-buffer, regular-file, and standard-stream
 * creators configured with fake adapter providers, allowing tests to exercise
 * the production stream factory and creator paths without relying on concrete
 * adapter implementations.
 *
 * It also provides direct construction of `stream` instances backed by
 * caller-supplied `fake_stream_adapter_t` objects for tests that only require
 * a stream dependency.
 */

#ifndef LEXLEO_FAKE_STREAM_H
#define LEXLEO_FAKE_STREAM_H

#include "stream/owners/stream_owners_api.h"

#include "lexleo/test/fake_stream_adapter.h"

extern stream_dynamic_buffer_creator_t *g_fake_stream_dynamic_buffer_creator;
extern stream_regular_file_creator_t *g_fake_stream_regular_file_creator;
extern stream_standard_stream_creator_t *g_fake_stream_standard_stream_creator;

typedef stream_t fake_stream_t;

fake_stream_t *fake_stream_create_instance(fake_stream_adapter_t *fake_stream_adapter);

void fake_stream_reset_instance(fake_stream_t *fake_stream);
void fake_stream_reset(void);

#endif /* LEXLEO_FAKE_STREAM_H */
