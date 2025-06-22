// src/ast/include/internal/ast_string_utils.h

#ifndef AST_STRING_UTILS_H
#define AST_STRING_UTILS_H

#ifdef USE_STRING_UTILS
    #include "string_utils.h"
    #define AST_STRING_DUPLICATE(s) get_current_string_duplicate()(s)
#else
    #include <string.h>
    #define AST_STRING_DUPLICATE(s) strdup(s)
#endif

#endif //AST_STRING_UTILS_H
