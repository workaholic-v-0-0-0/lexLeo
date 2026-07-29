

#ifndef LEXLEO_LEXLEO_VM_CR_TYPES_H
#define LEXLEO_LEXLEO_VM_CR_TYPES_H

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#include "stream/common/stream_opaque_type.h"

#include "logger/common/logger_opaque_type.h"

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
 * This structure stores the external borrowed runtime dependencies used when
 * creating a LexLeo VM handle.
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

#endif /* LEXLEO_LEXLEO_VM_CR_TYPES_H */
