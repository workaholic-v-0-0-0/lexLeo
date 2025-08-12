// src/symtab/include/internal/symtab_string_utils.h

#ifndef SYMTAB_STRING_UTILS_H
#define SYMTAB_STRING_UTILS_H

#ifdef USE_STRING_UTILS
    #include "string_utils.h"
    #define SYMTAB_STRING_DUPLICATE(s) get_current_string_duplicate()(s)
#else
    #include <string.h>
    #define SYMTAB_STRING_DUPLICATE(s) strdup(s)
#endif

#endif // SYMTAB_STRING_UTILS_H
