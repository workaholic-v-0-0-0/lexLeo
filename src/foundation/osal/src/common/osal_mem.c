// src/foundation/osal/src/osal_mem.c

#include "mem/osal_mem.h"
#include "mem/internal/osal_mem_ctx.h"

#include <stdlib.h>
#include <string.h>

#include "mem/osal_mem_ops.h"

void *osal_malloc(size_t size) { return malloc(size); }
void *osal_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }
void *osal_realloc(void *ptr, size_t size) { return realloc(ptr, size); }
void  osal_free(void *ptr) { free(ptr); }

char *osal_strdup(const char *s)
{
	if (!s) return NULL;

	size_t n = strlen(s) + 1;
	char *p = osal_malloc(n);
	if (!p) return NULL;

	memcpy(p, s, n);
	return p;
}

const osal_mem_ops_t *osal_mem_default_ops(void)
{
	static const osal_mem_ops_t OPS = {
		.malloc  = osal_malloc,
		.free    = osal_free,
		.calloc  = osal_calloc,
		.realloc = osal_realloc,
		.strdup  = osal_strdup,
	};
	return &OPS;
}
