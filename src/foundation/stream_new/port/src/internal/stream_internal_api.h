/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_internal_api.h
 * @ingroup stream_internal_group
 * @brief Internal operations for the `stream` port implementation.
 *
 * @details
 * This header declares implementation-level operations shared within the
 * `stream` module and optionally exposed to white-box tests.
 *
 * These declarations are not part of the public `stream` API.
 */

#ifndef LEXLEO_STREAM_INTERNAL_API_H
#define LEXLEO_STREAM_INTERNAL_API_H

#include "stream/common/stream_status_type.h"
#include "stream/common/stream_opaque_type.h"

#include "internal/stream_handle.h"
#include "internal/stream_factory_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Construct a stream using a registered adapter provider.
 *
 * @param[in] factory Factory containing the registered adapter providers.
 * @param[in] adapter_id Adapter identifier used to select the provider.
 * @param[in] args Adapter-specific construction arguments.
 * @param[out] out Receives the newly constructed stream handle.
 *
 * @return Stream construction status.
 *
 * See contract:
 * - @ref specifications_stream_factory_create_stream
 */
stream_factory_status_t stream_factory_create_stream(
	const stream_factory_t *factory,
	stream_adapter_id_t adapter_id,
	const void *args,
	stream_t **out);

/**
 * @brief Convert a stream factory status to a stream status.
 *
 * @param[in] factory_status Factory status to convert.
 *
 * @return Corresponding stream status.
 */
stream_status_t stream_factory_status_to_stream_status(
	stream_factory_status_t factory_status
);

/**
 * @brief Convert a stream status to a stream factory status.
 *
 * @param[in] status Stream status to convert.
 *
 * @return Corresponding stream factory status.
 */
stream_factory_status_t stream_status_to_stream_factory_status(
	stream_status_t status
);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_INTERNAL_API_H */
