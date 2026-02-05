// src/foundation/osal/osal_file/include/osal/file/osal_file.h

#ifndef LEXLEO_OSAL_FILE_H
#define LEXLEO_OSAL_FILE_H

#include <stddef.h>
#include <stdint.h>

#include "osal_file_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_file_env_t osal_file_env_t;

osal_file_t *osal_file_open(
	const char *path_utf8,
	uint32_t flags,
	osal_file_status_t *status,
	const osal_file_env_t *env );

size_t osal_file_read(
	osal_file_t *f,
	void *buf,
	size_t n,
	osal_file_status_t *status );

size_t osal_file_write(
	osal_file_t *f,
	const void *buf,
	size_t n,
	osal_file_status_t *status );

osal_file_status_t osal_file_flush(osal_file_t *f);

osal_file_status_t osal_file_close(osal_file_t *f);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_OSAL_FILE_H
