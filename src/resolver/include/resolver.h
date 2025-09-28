// src/resolver/include/resolver.h

#ifndef LEXLEO_RESOLVER_H
#define LEXLEO_RESOLVER_H

#include "ast.h"
#include "symtab.h"
#include "resolver_ctx.h"

#include <stdbool.h>

typedef enum {
    RESOLVER_ERROR_CODE_NULL_ROOT,
    RESOLVER_ERROR_CODE_SYMBOL_INTERN_FAILED,
    RESOLVER_ERROR_CODE_SYMBOL_LOOKUP_FAILED,
    RESOLVER_ERROR_CODE_OOM,
    // ...,
    RESOLVER_UNRETRIEVABLE_ERROR_CODE,
} resolver_error_type;

// true on success
// false on error
// promote ast data wrapper for symbol name into ast data wrapper for symbol
// makes error nodes when non infracture error
// cleanup and write NULL at *a if infracture error
// classical iterative implementation
bool resolver_resolve_ast(ast **a, resolver_ctx ctx);

#endif //LEXLEO_RESOLVER_H

