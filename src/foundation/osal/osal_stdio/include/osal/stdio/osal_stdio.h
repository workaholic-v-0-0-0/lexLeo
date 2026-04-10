

#ifndef LEXLEO_OSAL_STDIO_H
#define LEXLEO_OSAL_STDIO_H

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int osal_snprintf(
	char *str,
	size_t size,
	const char *fmt,
	...);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_STDIO_H
