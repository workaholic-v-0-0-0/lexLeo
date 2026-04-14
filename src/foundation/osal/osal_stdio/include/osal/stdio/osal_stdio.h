

#ifndef LEXLEO_OSAL_STDIO_H
#define LEXLEO_OSAL_STDIO_H

#include "osal/stdio/osal_stdio_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

OSAL_STDIO *osal_stdio_stdout(void);
OSAL_STDIO *osal_stdio_stderr(void);

size_t osal_stdio_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio);

int osal_stdio_flush(OSAL_STDIO *stdio);

int osal_snprintf(
	char *str,
	size_t size,
	const char *fmt,
	...);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_STDIO_H
