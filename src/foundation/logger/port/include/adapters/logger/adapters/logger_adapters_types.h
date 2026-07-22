/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_adapters_types.h
 * @ingroup logger_adapters_api
 * @brief Adapter-facing types for the `logger` port.
 *
 * @details
 * This header defines the types used by `logger` adapter implementations.
 *
 * It provides the adapter environment @ref logger_env_t.
 *
 * Backend operation types and the dispatch table are declared in
 * @ref logger_adapters_vtbl.h.
 */

#ifndef LEXLEO_LOGGER_ADAPTERS_TYPES_H
#define LEXLEO_LOGGER_ADAPTERS_TYPES_H

#include "logger/borrowers/logger_borrowers_types.h"
#include "logger/adapters/logger_adapters_vtbl.h"

#include "osal/mem/osal_mem_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct logger_env_t
 *
 * @brief Borrowed dependencies required by the `logger` port.
 *
 * @details
 * The Composition Root provides these dependencies when creating a `logger`
 * handle. Pointer fields remain borrowed and must outlive the logger handles
 * using this environment.
 */
typedef struct logger_env_t {

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem;

	/** Borrowed backend virtual table. */
	const logger_vtbl_t *vtbl;

} logger_env_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_ADAPTERS_TYPES_H */
