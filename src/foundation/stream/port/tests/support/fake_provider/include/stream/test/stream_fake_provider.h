/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_fake_provider.h
 * @ingroup stream_tests_group
 * @brief Fake stream dependency provider for `stream` tests.
 *
 * @details
 * This header exposes fake stream creators ready to be injected in tests.
 * It also includes the underlying `fake_stream` API so tests can configure
 * and inspect prepared fake stream backends.
 */

#ifndef LEXLEO_STREAM_FAKE_PROVIDER_H
#define LEXLEO_STREAM_FAKE_PROVIDER_H

#include "stream/owners/stream_buffer_creator.h"
#include "stream/owners/stream_file_creator.h"
#include "stream/owners/stream_io_creator.h"

#include "lexleo/test/fake_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

const stream_buffer_creator_t *stream_test_fake_buffer_creator(void);
const stream_file_creator_t *stream_test_fake_file_creator(void);
const stream_io_creator_t *stream_test_fake_io_creator(void);


#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_FAKE_PROVIDER_H */
