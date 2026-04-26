

#ifndef LEXLEO_OSAL_STDIO_H
#define LEXLEO_OSAL_STDIO_H

#include "osal/stdio/osal_stdio_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

OSAL_STDIO *osal_stdio_stdin(void);
OSAL_STDIO *osal_stdio_stdout(void);
OSAL_STDIO *osal_stdio_stderr(void);

size_t osal_stdio_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio);

size_t osal_stdio_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio);

int osal_stdio_flush(OSAL_STDIO *stdio);

int osal_stdio_error(OSAL_STDIO *stdio);

int osal_stdio_eof(OSAL_STDIO *stdio);

void osal_stdio_clearerr(OSAL_STDIO *stdio);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_STDIO_H
