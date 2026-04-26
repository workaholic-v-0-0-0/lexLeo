#ifndef LEXLEO_OSAL_STR_H
#define LEXLEO_OSAL_STR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "policy/lexleo_cstd_types.h"

size_t osal_strlen(const char *s);
int osal_strcmp(const char *s1, const char *s2);
char *osal_strchr(const char *s, int c);
char *osal_strrchr(const char *s, int c);
int osal_isspace(int c);
int osal_snprintf(char *str, size_t size, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_OSAL_STR_H
