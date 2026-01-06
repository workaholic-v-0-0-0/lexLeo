// src/core/interpreter/include/internal/interpreter_ctx.h

#ifndef LEXLEO_INTERPRETER_CTX_H
#define LEXLEO_INTERPRETER_CTX_H

struct interpreter_ops_t;

typedef struct interpreter_ctx {
	const interpreter_ops_t *ops;
	void *host_ctx;  // opaque ; hook for runtime_session instance ;
} interpreter_ctx;



#endif //LEXLEO_INTERPRETER_CTX_H