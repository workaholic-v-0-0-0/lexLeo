#include "osal/str/osal_str.h"

#include "policy/lexleo_cstring.h"
#include "policy/lexleo_cstd_ctype.h"

char* osal_strchr(const char* s, int c)
{
	return strchr(s, c);
}

char* osal_strrchr(const char* s, int c)
{
	return strrchr(s, c);
}

int osal_isspace(int c)
{
	return isspace(c);
}
