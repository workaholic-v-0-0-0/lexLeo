// src/ports/lexer/src/internal/lexer_internal.h

#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// HANDLE
// ----------------------------------------------------------------------------

struct lexer_t {
	lexer_vtbl_t vtbl;
	void *backend;
	const osal_mem_ops_t *mem;
};

#ifdef __cplusplus
}
#endif

#endif //_LEXER_INTERNAL_H