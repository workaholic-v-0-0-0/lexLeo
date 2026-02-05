// src/foundation/osal/src/internal/osal_mem_ops.c

#include "internal/osal_mem_ops.h"

#include <stdlib.h>
#include <string.h>

static void *libc_malloc(size_t s) { return malloc(s); }
static void  libc_free(void *p)    { free(p); }
static void *libc_calloc(size_t n, size_t s) { return calloc(n, s); }
static void *libc_realloc(void *p, size_t s) { return realloc(p, s); }

static char *libc_strdup(const char *s)
{
	if (!s) return NULL;
	size_t len = strlen(s) + 1;
	char *p = (char *)malloc(len);
	if (!p) return NULL;
	memcpy(p, s, len);
	return p;
}

const osal_mem_ops_t *osal_mem_default_ops(void)
{
	static const osal_mem_ops_t OPS = {
		.malloc  = libc_malloc,
		.free    = libc_free,
		.calloc  = libc_calloc,
		.realloc = libc_realloc,
		.strdup  = libc_strdup,
	};
	return &OPS;
}
