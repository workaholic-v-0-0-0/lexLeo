/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_factory_create_stream.h
 * @ingroup stream_internal_group
 * @brief Owner-facing factory creation operation for the `stream` port.
 */

#ifndef LEXLEO_STREAM_FACTORY_CREATE_STREAM_H
#define LEXLEO_STREAM_FACTORY_CREATE_STREAM_H

#include "stream/borrowers/stream_borrowers_types.h"
#include "stream/owners/stream_owners_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a stream through a registered stream factory.
 *
 * @param[in] f Stream factory to use.
 * @param[in] key Adapter key identifying the backend to instantiate.
 * @param[in] args Adapter-specific creation arguments.
 * @param[out] out Receives the created stream handle.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_factory_create_stream
 */
stream_status_t stream_factory_create_stream(
	const stream_factory_t *f,
	stream_key_t key,
	const void *args,
	stream_t **out);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_FACTORY_CREATE_STREAM_H */
