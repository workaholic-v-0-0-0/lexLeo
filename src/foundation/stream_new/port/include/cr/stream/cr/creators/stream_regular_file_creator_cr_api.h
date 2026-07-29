/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_regular_file_creator_cr_api.h
 * @ingroup stream_cr_api
 * @brief Composition Root API for regular-file stream creators.
 *
 * @details
 * This header exposes CR-facing services used to create and destroy
 * `stream_regular_file_creator_t` objects.
 */

#ifndef LEXLEO_STREAM_REGULAR_FILE_CREATOR_CR_API_H
#define LEXLEO_STREAM_REGULAR_FILE_CREATOR_CR_API_H

#include "stream/cr/stream_cr_types.h"
#include "stream/owners/creators/stream_regular_file_creator.h"

#include "osal/mem/osal_mem_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a regular-file stream creator.
 *
 * @param[out] out Receives the created creator handle.
 * @param[in] factory Stream factory used for stream creation.
 * @param[in] adapter_id Adapter provider identifier used by the factory.
 * @param[in] mem Memory operations used to allocate and destroy the creator.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_create_regular_file_creator
 */
stream_factory_status_t stream_create_regular_file_creator(
	stream_regular_file_creator_t **out,
	stream_factory_t *factory,
	stream_adapter_id_t adapter_id,
	const osal_mem_ops_t *mem
);

/**
 * @brief Destroy a regular-file stream creator.
 *
 * @param[in,out] creator Address of the creator handle to destroy.
 *
 * See contract:
 * - @ref specifications_stream_destroy_regular_file_creator
 */
void stream_destroy_regular_file_creator(
	stream_regular_file_creator_t **creator
);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_REGULAR_FILE_CREATOR_CR_API_H */
