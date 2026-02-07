// src/core/lexer/port/src/internal/lexer_internal.h

#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

#include "internal/lexer_ctx.h"

struct lexer_t {
	lexer_vtbl_t vtbl;
	void *backend;
	const osal_mem_ops_t *mem;
};

#endif //_LEXER_INTERNAL_H
