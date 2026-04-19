#ifndef LEXLEO_OSAL_STR_H
#define LEXLEO_OSAL_STR_H

#ifdef __cplusplus
extern "C" {
#endif

char *osal_strchr(const char *s, int c);
char *osal_strrchr(const char *s, int c);
int osal_isspace(int c);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_STR_H
