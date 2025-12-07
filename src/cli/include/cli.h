// src/cli/include/cli.h

// DRAFT SQUELETON

#ifndef LEXLEO_CLI_H
#define LEXLEO_CLI_H

#include "interpreter.h"
#include "runtime_env.h"
// maybe others...

struct ast;
struct symbol;

interpreter_status cli_read_eval (
	struct interpreter_ctx *ctx,
	struct runtime_env *env,
	const struct runtime_env_value **out );
/* will do
init parser ctx
init resolver ctx
init interpreter ctx:
	runtime_session *rs = runtime_session_create();
	//...
	static const interpreter_ops CLI_INTERPRETER_OPS = {
		.read_ast_fn = cli_read_eval,
	};
	interpreter_ctx_init(&rs->ictx, &CLI_INTERPRETER_OPS, rs);
a loop to fill input_provider and try parse while it returns incomplete status (return if it returns error)
a non resolved ast* root is produced and returned by parser
resolver_resolve_ast(&root, rctx); // rctx is initialized by cli so that it stores in symbol_pool of the current session
now root is resolved
runtime_session_store_ast(root, (runtime_session *) ctx->host_ctx);
return interpreter_eval(
	ctx,
	env,
	root,
	out );
*/

bool cli_store_symbol(struct symbol *sym, void *session); // wrapper






// DRAFT COMMENT

// cli_store_symbol will call runtime_session_store_symbol(sym, (runtime_session *) session)
/* This is useless:
   bool cli_store_ast(runtime_session *session, void *root); // wrapper
   // will call runtime_session_store_ast(rs, (struct ast *) root)
because cli will call runtime_session_store_ast directly
*/

/*
cli will create an input_provider, bind it to scanner via API of runtime_session
then manipulate it
runtime_session read from it automatically via YY_INPUT
one input_provider per runtime_session
*/


/* a global rs for the runtime_session* instance
*/
/* a global rctx for the resolver_ctx* instance
*/
/* a global ictx for the interpreter_ctx* instance
*/

/* later remove globals and make a cli_ctx */

/* will initialize rctx via:
   hang cli_store_symbol at rctx.ops.store_symbol
   hang rs.symbol_pool at rctx.user_data
*/

/* a fct to initialize ictx via:
   hang rs at ictx.host_ctx
   hang cli_eval_read_ast at ictx.ops.eval_read_ast
*/

#endif //LEXLEO_CLI_H
