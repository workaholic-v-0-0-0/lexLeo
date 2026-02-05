// src/foundation/osal/include/file/internal/osal_file_ctx.h

#ifndef LEXLEO_OSAL_FILE_CTX_H
#define LEXLEO_OSAL_FILE_CTX_H

#include "mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_file_deps_t {
	const osal_mem_ops_t *mem;
} osal_file_deps_t;

typedef struct osal_file_ctx_t {
	osal_file_deps_t deps;
} osal_file_ctx_t;

osal_file_ctx_t osal_file_default_ctx(const osal_mem_ops_t *mem_ops);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_FILE_CTX_H
