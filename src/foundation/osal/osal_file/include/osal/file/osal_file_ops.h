// src/foundation/osal/osal_file/include/osal/file/osal_file_ops.h

#ifndef LEXLEO_OSAL_FILE_OPS_H
#define LEXLEO_OSAL_FILE_OPS_H

#include <stddef.h>
#include <stdint.h>

#include "osal_file_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_file_env_t osal_file_env_t;

typedef struct osal_file_ops_t {
	osal_file_t *(*open)(
		const char *path_utf8,
		uint32_t flags,
		osal_file_status_t *status,
		const osal_file_env_t *env );
	size_t (*read)(
		osal_file_t *f,
		void *buf,
		size_t n,
		osal_file_status_t *status );
	size_t (*write)(
		osal_file_t *f,
		const void *buf,
		size_t n,
		osal_file_status_t *status );
	osal_file_status_t (*flush)(osal_file_t *f);
	osal_file_status_t (*close)(osal_file_t *f);
} osal_file_ops_t;

const osal_file_ops_t *osal_file_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_FILE_OPS_H
