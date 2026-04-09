// src/foundation/osal/osal_mem/include/osal/mem/osal_mem.h

#ifndef LEXLEO_OSAL_MEM_H
#define LEXLEO_OSAL_MEM_H

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void *osal_malloc(size_t size);
void osal_free(void *ptr);
void *osal_calloc(size_t nmemb, size_t size);
void *osal_realloc(void *ptr, size_t size);
char *osal_strdup(const char *s);

void *osal_memcpy(void *dest, const void *src, size_t n);
void *osal_memset(void *s, int c, size_t n);
size_t osal_strlen(const char *s);
int osal_strcmp(const char *s1, const char *s2);
void *osal_memmove(void *dest, const void *src, size_t n);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_MEM_H
