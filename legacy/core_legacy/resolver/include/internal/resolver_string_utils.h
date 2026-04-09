// src/core/resolver/include/internal/resolver_string_utils.h

#ifndef LEXLEO_RESOLVER_STRING_UTILS_H
#define LEXLEO_RESOLVER_STRING_UTILS_H

#ifdef USE_STRING_UTILS
    #include "string_utils.h"
    #define RESOLVER_STRING_DUPLICATE(s) get_current_string_duplicate()(s)
#else
    #include <string.h>
    #define RESOLVER_STRING_DUPLICATE(s) strdup(s)
#endif

#endif //LEXLEO_RESOLVER_STRING_UTILS_H
