// src/runtime_env/include/internal/runtime_env_string_utils.h

#ifndef LEXLEO_RUNTIME_ENV_STRING_UTILS_H
#define LEXLEO_RUNTIME_ENV_STRING_UTILS_H

#ifdef USE_STRING_UTILS
    #include "string_utils.h"
    #define RUNTIME_ENV_STRING_DUPLICATE(s) get_current_string_duplicate()(s)
#else
    #include <string.h>
    #define RUNTIME_ENV_STRING_DUPLICATE(s) strdup(s)
#endif

#endif //LEXLEO_RUNTIME_ENV_STRING_UTILS_H