// src/data_structures/include/internal/data_structure_string_utils.h

#ifndef DATA_STRUCTURE_STRING_UTILS_H
#define DATA_STRUCTURE_STRING_UTILS_H

#ifdef USE_STRING_UTILS
    #include "string_utils.h"
    #define DATA_STRUCTURE_STRING_COMPARE(s1,s2) get_current_string_compare()(s1,s2)
    #define DATA_STRUCTURE_STRING_DUPLICATE(s) get_current_string_duplicate()(s)
#else
    #include <string.h>
    #define DATA_STRUCTURE_STRING_COMPARE(s1,s2) strcmp(s1,s2)
    #define DATA_STRUCTURE_STRING_DUPLICATE(s) strdup(s)
#endif

#endif //DATA_STRUCTURE_STRING_UTILS_H
