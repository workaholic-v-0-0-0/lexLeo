/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_buffer_creator_cr_api.h
 * @ingroup stream_cr_api
 * @brief Composition Root API for buffer stream creators.
 *
 * @details
 * This header exposes CR-facing services used to create and destroy
 * `stream_buffer_creator_t` objects.
 */

#ifndef LEXLEO_STREAM_BUFFER_CREATOR_CR_API_H
#define LEXLEO_STREAM_BUFFER_CREATOR_CR_API_H

#include "stream/owners/creators/stream_buffer_creator.h"
#include "stream/cr/stream_cr_api.h"

#include "osal/mem/osal_mem_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a buffer stream creator.
 *
 * @param[out] out Receives the created creator handle.
 * @param[in] factory Stream factory used for creation.
 * @param[in] key Adapter key used by the factory.
 * @param[in] mem Memory operations used by the creator.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_create_buffer_creator
 */
stream_status_t stream_create_buffer_creator(
	stream_buffer_creator_t **out,
	stream_factory_t *factory,
	stream_key_t key,
	const osal_mem_ops_t *mem);

/**
 * @brief Destroy a buffer stream creator.
 *
 * @param[in,out] creator
 * Address of the creator handle to destroy.
 *
 * @details
 * If `creator == NULL` or `*creator == NULL`, this function does nothing.
 * Otherwise, it releases the creator object and sets `*creator` to `NULL`.
 *
 * See contract:
 * - @ref specifications_stream_destroy_buffer_creator
 */
void stream_destroy_buffer_creator(stream_buffer_creator_t **creator);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_BUFFER_CREATOR_CR_API_H */
