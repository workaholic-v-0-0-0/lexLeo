/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_cr_types.h
 * @ingroup stream_cr_api
 * @brief Composition Root types for the `stream` port.
 *
 * @details
 * Declares the types used by the Composition Root API to configure and
 * construct `stream` port objects.
 */

#ifndef LEXLEO_STREAM_CR_TYPES_H
#define LEXLEO_STREAM_CR_TYPES_H

#include "stream/common/stream_status_type.h"
#include "stream/common/stream_vtbl_type.h"
#include "stream/common/stream_factory_opaque_type.h"
#include "stream/common/stream_adapter_id_type.h"
#include "stream/common/stream_factory_status_type.h"

#include "osal/mem/osal_mem_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct stream_env_t
 *
 * @brief Borrowed dependencies required by the `stream` port.
 *
 * @details
 * The Composition Root provides these dependencies when creating a `stream`
 * handle. Pointer fields remain borrowed and must outlive the stream handles
 * using this environment.
 */
typedef struct stream_env_t {

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem;

	/** Borrowed stream adapter dispatch table. */
	const stream_vtbl_t *vtbl;

} stream_env_t;

/**
 * @brief Configuration for a stream factory.
 */
typedef struct stream_factory_cfg_t {

	/** Maximum number of adapter providers that can be registered. */
	size_t fact_cap;

} stream_factory_cfg_t;

/**
 * @brief Opaque stream adapter provider type.
 *
 * @details
 * Represents the adapter-specific information required to register an
 * adapter with a `stream_factory_t`.
 */
typedef struct stream_adapter_provider_t stream_adapter_provider_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_CR_TYPES_H */
