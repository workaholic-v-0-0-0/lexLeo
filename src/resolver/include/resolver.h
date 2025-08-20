// src/resolver/include/resolver.h

#ifndef LEXLEO_RESOLVER_H
#define LEXLEO_RESOLVER_H

#include "symtab.h"
#include "resolver_ctx.h"

typedef enum {
    RESOLVER_ERROR_CODE_NULL_ROOT,
    RESOLVER_ERROR_CODE_ALLOC_FRAME_FAILED,
    RESOLVER_ERROR_CODE_TRAVERSAL_STACK_INIT_FAILED,
    RESOLVER_ERROR_CODE_TRAVERSAL_PUSH_FAILED,
    RESOLVER_ERROR_CODE_SYMBOL_INTERN_FAILED,
    RESOLVER_ERROR_CODE_SYMBOL_LOOKUP_FAILED,
    // ...,
    RESOLVER_UNRETRIEVABLE_ERROR_CODE,
} resolver_error_type;

// 0 on success
// -1 on error
// promote ast data wrapper for symbol name into ast data wrapper for symbol
// make error nodes when ... is detected
// classical iterative implementation
int resolver_resolve_ast(ast **a, resolver_ctx ctx);

#endif //LEXLEO_RESOLVER_H

