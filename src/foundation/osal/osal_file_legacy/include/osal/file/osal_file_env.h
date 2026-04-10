// src/foundation/osal/osal_file/include/osal/file/osal_file_env.h

#ifndef LEXLEO_OSAL_FILE_ENV_H
#define LEXLEO_OSAL_FILE_ENV_H

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_file_env_t {
	const osal_mem_ops_t *mem;
} osal_file_env_t;

osal_file_env_t osal_file_default_env(const osal_mem_ops_t *mem_ops);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_FILE_ENV_H
