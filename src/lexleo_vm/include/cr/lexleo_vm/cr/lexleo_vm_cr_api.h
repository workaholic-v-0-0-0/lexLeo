/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_vm_cr_api.h
 * @ingroup lexleo_vm_cr_api
 * @brief Composition Root API for constructing LexLeo VM instances.
 *
 * @details
 * This header exposes the Composition Root API used to build the
 * configuration and environment values required by the LexLeo VM, create VM
 * handles, and complete their default owned-resource initialization.
 */

#ifndef LEXLEO_VM_CR_API_H
#define LEXLEO_VM_CR_API_H

#include "lexleo_vm/borrowers/lexleo_vm_types.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#include "stream/owners/stream_creators_api.h"

#include "logger/borrowers/logger_borrowers_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LexLeo VM configuration.
 *
 * @details
 * This structure stores configuration values used when creating a LexLeo VM
 * handle.
 *
 * It is currently reserved for future options.
 */
typedef struct lexleo_vm_cfg_t {
	/** Reserved field kept to make the configuration structure non-empty. */
	int reserved;
} lexleo_vm_cfg_t;

/**
 * @brief LexLeo VM environment.
 *
 * @details
 * This structure stores the borrowed runtime dependencies used when creating
 * a LexLeo VM handle.
 *
 * The structure is public so that Composition Root code can start from
 * `lexleo_vm_default_env()` and adjust selected fields before calling
 * `lexleo_vm_create()`.
 */
typedef struct lexleo_vm_env_t {
	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem_ops;

	/** Borrowed standard I/O operations. */
	const osal_stdio_ops_t *stdio_ops;

	/** Borrowed file operations. */
	const osal_file_ops_t *file_ops;

	/** Borrowed string operations. */
	const osal_str_ops_t *str_ops;

	/** Borrowed time operations. */
	const osal_time_ops_t *time_ops;

	/** Borrowed input stream. */
	stream_t *in;

	/** Borrowed output stream. */
	stream_t *out;

	/** Borrowed error stream. */
	stream_t *err;

	/** Borrowed logger. */
	logger_t *logger;
} lexleo_vm_env_t;

/**
 * @brief Returns the default LexLeo VM configuration.
 *
 * @return Default LexLeo VM configuration value.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_default_cfg
 */
lexleo_vm_cfg_t lexleo_vm_default_cfg(void);

/**
 * @brief Returns a LexLeo VM environment from borrowed dependencies.
 *
 * @details
 * Builds an explicit environment value used when creating a LexLeo VM handle.
 *
 * @param[in] mem_ops Borrowed memory operations.
 * @param[in] stdio_ops Borrowed standard I/O operations.
 * @param[in] file_ops Borrowed file operations.
 * @param[in] str_ops Borrowed string operations.
 * @param[in] time_ops Borrowed time operations.
 * @param[in] in Borrowed input stream.
 * @param[in] out Borrowed output stream.
 * @param[in] err Borrowed error stream.
 * @param[in] logger Borrowed logger.
 *
 * @return Environment value initialized from the provided dependencies.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_default_env
 */
lexleo_vm_env_t lexleo_vm_default_env(
	const osal_mem_ops_t *mem_ops,
	const osal_stdio_ops_t *stdio_ops,
	const osal_file_ops_t *file_ops,
	const osal_str_ops_t *str_ops,
	const osal_time_ops_t *time_ops,
	stream_t *in,
	stream_t *out,
	stream_t *err,
	logger_t *logger
);

/**
 * @brief Creates a LexLeo VM handle.
 *
 * @details
 * Allocates and initializes the public LexLeo VM handle from the provided
 * configuration and borrowed environment.
 *
 * Owned runtime resources are not initialized by this function. They are
 * initialized later by `lexleo_vm_complete_default_init()`.
 *
 * @param[out] out Receives the created VM handle.
 * @param[in] cfg VM configuration.
 * @param[in] env VM environment.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_create
 */
lexleo_vm_status_t lexleo_vm_create(
	lexleo_vm_t **out,
	const lexleo_vm_cfg_t *cfg,
	const lexleo_vm_env_t *env
);

/**
 * @brief Destroys a LexLeo VM handle.
 *
 * @details
 * Releases the VM handle and every owned runtime resource attached to it,
 * then resets `*vm` to `NULL`.
 *
 * @param[in,out] vm Address of the VM handle to destroy.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_destroy
 */
void lexleo_vm_destroy(lexleo_vm_t **vm);

/**
 * @brief Completes the default initialization of a LexLeo VM handle.
 *
 * @details
 * Initializes the VM owned runtime resources that are not created by
 * `lexleo_vm_create()`.
 *
 * @param[in,out] vm VM handle to complete.
 *
 * @return Initialization status.
 *
 * See contract:
 * - @ref specifications_lexleo_vm_complete_default_init
 */
lexleo_vm_status_t lexleo_vm_complete_default_init(lexleo_vm_t *vm);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_VM_CR_API_H */
