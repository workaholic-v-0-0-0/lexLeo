// src/runtime_session/include/internal/runtime_session_internal.h

#ifndef LEXLEO_RUNTIME_SESSION_INTERNAL_H
#define LEXLEO_RUNTIME_SESSION_INTERNAL_H

#include "runtime_session.h"

#include "list.h"
#include "runtime_session_memory_allocator.h"
#include "runtime_session_ctx.h"

#include <stdbool.h>

struct runtime_session {
	struct input_provider *in;
	yyscan_t scanner;
	struct symtab *st;
	struct runtime_env *env; // maybe another for root
	list ast_pool;
	list symbol_pool;
};





// DRAFT COMMENT
// dontforget!
// callback for interpreter for reading
// in this callback, call runtime_session_store_ast
// callback for resolver for intern_symbol
// in this callback, call runtime_session_store_symbol
// still think about how to handle pools
// cases of ast creation:
// 1) exec filename -> cli can manage
//      parse_translation_unit->ast
//      resolver->ast but create symbol...
//      runtime_session_store_ast
// 2) an instruction in cli
//      parse_one_statement
//      resolver -> and symbols ??
//      runtime_session_store_ast
// 3) an AST of type reading evaluation
//      interpreter call callback eval_read
//      parse_readable
//      resolver -> and symbols ??
//      runtime_session_store_ast
//
//
// cases of symbol creation:
// 1) during internment,
//
// put eval_read in cli
// cli is the big orchestrer
// cli has two wrappers:
// - cli_eval_read_ast (wich call runtime_session_store_ast)
// - cli_store_symbol (wich call runtime_session_store_symbol)

#endif //LEXLEO_RUNTIME_SESSION_INTERNAL_H