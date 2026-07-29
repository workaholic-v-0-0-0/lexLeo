/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_cr_types.h
 * @ingroup logger_cr_api
 * @brief Composition Root types for the `logger` port.
 *
 * @details
 * This header exposes the types used by the Composition Root to provide
 * external borrowed dependencies to the `logger` port.
 */

#ifndef LEXLEO_LOGGER_CR_TYPES_H
#define LEXLEO_LOGGER_CR_TYPES_H

#include "logger/common/logger_vtbl_type.h"

#include "osal/mem/osal_mem_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct logger_env_t
 *
 * @brief External borrowed dependencies required by the `logger` port.
 *
 * @details
 * The Composition Root provides these dependencies when creating a `logger`
 * handle. Pointer fields remain borrowed and must outlive the logger handles
 * using this environment.
 */
typedef struct logger_env_t {

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem_ops;

	/** Borrowed backend virtual table. */
	const logger_vtbl_t *vtbl;

} logger_env_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_CR_TYPES_H */
