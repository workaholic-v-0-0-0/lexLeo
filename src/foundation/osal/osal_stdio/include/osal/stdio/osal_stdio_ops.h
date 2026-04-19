#ifndef LEXLEO_OSAL_STDIO_OPS_H
#define LEXLEO_OSAL_STDIO_OPS_H

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "osal/stdio/osal_stdio_types.h"

typedef struct osal_stdio_ops_t {

	OSAL_STDIO *(*get_stdin)(void);

	OSAL_STDIO *(*get_stdout)(void);

	OSAL_STDIO *(*get_stderr)(void);

	size_t (*read)(
		void *ptr,
		size_t size,
		size_t nmemb,
		OSAL_STDIO *stdio);

	size_t (*write)(
		const void *ptr,
		size_t size,
		size_t nmemb,
		OSAL_STDIO *stdio);

	int (*flush)(OSAL_STDIO *stdio);

	int (*error)(OSAL_STDIO *stdio);

	int (*eof)(OSAL_STDIO *stdio);

	void (*clear_error)(OSAL_STDIO *stdio);

} osal_stdio_ops_t;

const osal_stdio_ops_t *osal_stdio_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_OSAL_STDIO_OPS_H
