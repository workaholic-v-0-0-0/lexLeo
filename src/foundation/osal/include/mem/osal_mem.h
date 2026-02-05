// src/foundation/osal/include/osal_mem.h

#ifndef LEXLEO_OSAL_MEM_H
#define LEXLEO_OSAL_MEM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------

void *osal_malloc(size_t size);
void osal_free(void *ptr);
void *osal_calloc(size_t nmemb, size_t size);
void *osal_realloc(void *ptr, size_t size);

char *osal_strdup(const char *s);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_MEM_H
