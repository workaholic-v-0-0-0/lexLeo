// src/cli/src/cli.c

#include <stdlib.h>
#include <stdbool.h>

#include "cli.h"
#include "stream.h"
#include "stdio_stream.h"
#include "internal/cli_internal.h"
#include "input_provider.h"
#include "runtime_env.h"
#include "runtime_session.h"
#include "lexer.h"
#include "parser_types.h"
#include "parser_api.h"
#include "resolver.h"
#include "interpreter.h"

#ifdef DEBUG
	#include "internal/input_provider_internal.h"
	#include "internal/runtime_session_internal.h"
	#include "internal/interpreter_ctx.h"
#endif

#define CLI_SIZE_OF_BUFFER 1024

struct ast *cli_read_ast(const struct interpreter_ctx *ctx) {
	return NULL; // placeholder
/* will do
init parser ctx
init resolver ctx
init interpreter ctx:
	runtime_session *rs = runtime_session_create();
	//...
	static const interpreter_ops CLI_INTERPRETER_OPS = {
		.read_ast_fn = cli_read_ast,
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
}


bool runtime_env_value_to_string(
    const runtime_env_value *v,
    char *buf,
    size_t cap,
    size_t *out_len)
{
    if (!v || !buf || cap == 0) return false;

    switch (v->type) {
    case RUNTIME_VALUE_NUMBER:
        *out_len = snprintf(buf, cap, "%d", v->as.i);
        return true;

    case RUNTIME_VALUE_STRING:
        *out_len = snprintf(buf, cap, "%s", v->as.s);
        return true;

    case RUNTIME_VALUE_SYMBOL:
        *out_len = snprintf(buf, cap, "symbol %s at %p", v->as.sym->name, v);
        return true;

	case RUNTIME_VALUE_QUOTED: {
    	const ast *q = v->as.quoted;
    	*out_len = snprintf(buf, cap, "<quoted ast=%p type=%d>", (void*)q, q ? (int)q->type : -1);
    	return true;
	}

    default:
        *out_len = snprintf(buf, cap, "<value>");
        return true;
    }
}


bool cli_print(
    const struct interpreter_ctx *ctx,
    const struct runtime_env_value *value) {
    if (!ctx || !value)
        return false;

    runtime_session *rs = (runtime_session *) interpreter_ctx_get_host_ctx(ctx);

    if (!rs || !rs->out)
        return false;

    char buf[256];
    size_t len = 0;

    if (!runtime_env_value_to_string(value, buf, sizeof(buf), &len))
        return false;

    if (len > 0) {
        stream_write(rs->out, buf, len);
    }

    stream_write(rs->out, "\n", 1);
    stream_flush(rs->out);

    return true;
}

static const interpreter_ops_t CLI_INTERPRETER_OPS = {
	.read_ast_fn = cli_read_ast,
	.write_runtime_value_fn = cli_print
};

bool cli_store_symbol(struct symbol *sym, void *session) {
	return runtime_session_store_symbol(sym, session);
}

static bool readline(
	    stream *in,
    	char *buf,
    	size_t cap,
    	size_t *out_len ) {
    if (!in || !buf || cap < 2) return false;

    size_t len = 0;

    while (len + 1 < cap) {
        char c;
        size_t r = stream_read(in, &c, 1);

        if (r == 0) {
            // EOF
            if (len == 0)
                return false; // nothing has been read
            break;
        }

        buf[len++] = c;

        if (c == '\n')
            break;
    }

    buf[len] = '\0';
    if (out_len) *out_len = len;
    return true;
}

//<here> make a new unary operator "symbol" to make a symbol runtime value
int cli_run() { // in a very dirty draft state!
	struct stream *in = stdio_stream_from_stdin();
	if (!in) return EXIT_FAILURE;

	struct runtime_session *rs = runtime_session_create();
	if (!rs) {
		stream_close(in); //sure?
		return EXIT_FAILURE;
	}

	struct input_provider *ip = input_provider_create();
	if (!ip) {
		stream_close(in); //sure?
	    runtime_session_destroy(rs);
    	return EXIT_FAILURE;
	}
	//printf("ip: %p\n", ip);

	struct stream *out = stdio_stream_to_stdout();
	if (!out) {
		//printf("out: %p\n", out);
		stream_close(in); //sure?
		input_provider_destroy(ip);
	    runtime_session_destroy(rs);
    	return EXIT_FAILURE;
	}
	//printf("out: %p\n", out);

	if (!runtime_session_bind_output_stream(rs, out)) {
		//printf("rs: %p\n", rs);
		stream_close(in); //sure?
		stream_close(out); //sure?
		input_provider_destroy(ip);
		runtime_session_destroy(rs);
    	return EXIT_FAILURE;
	}
	//printf("rs: %p\n", rs);

	if (!input_provider_set_mode_chunks(ip)) {
		stream_close(in); //sure?
		stream_close(out); //sure?
		input_provider_destroy(ip);
		runtime_session_destroy(rs);
    	return EXIT_FAILURE;
	}
	//printf("input_provider_set_mode_chunks\n");

	if (!runtime_session_bind_input_provider(rs, ip)) {
		//printf("ip: %p\n", ip);
		//printf("rs->scanner: %p\n", rs->scanner);
		//printf("ip->lexer_scanner: %p\n", ip->lexer_scanner);
		stream_close(in); //sure?
		stream_close(out); //sure?
		input_provider_destroy(ip);
		runtime_session_destroy(rs);
    	return EXIT_FAILURE;
	}
	//printf("runtime_session_bind_input_provider\n");

	resolver_ctx rctx = {
    	.ops = RESOLVER_OPS_DEFAULT,
    	.st = runtime_session_get_symtab(rs),
    	.user_data = rs,
	};
	rctx.ops.store_symbol = cli_store_symbol;

	struct interpreter_ctx *ic =
	interpreter_ctx_create(&CLI_INTERPRETER_OPS, rs);
	if (!ic) {
		stream_close(out);//sure?
		stream_close(in); //sure?
		input_provider_destroy(ip);
	    runtime_session_destroy(rs);
    	return EXIT_FAILURE;
	}
	//printf("ic: %p\n", ic);

	parser_cfg pc = get_parser_cfg_one_statement();

	//loop
	parse_status st = PARSE_STATUS_OK;
	interpreter_status ist;
	runtime_env_value *value;
	ast *parsed_ast = NULL;
	ast *resolved_ast = NULL;
	char buf[CLI_SIZE_OF_BUFFER] = {0};
	size_t n;
	bool need_prompt = true;
	while (true) {
		printf("new loop!\n");
		if (need_prompt) {
			stream_write(out, "> ", 2);
        	stream_flush(out);
			if (!readline(in, buf, sizeof(buf), &n)) break;
			if (!input_provider_append(ip, buf, n)) break;
	        need_prompt = false;
		}

		st = parse_one_statement(runtime_session_get_scanner(rs), &parsed_ast, &pc, ip);
		if (st == PARSE_STATUS_INCOMPLETE) {
        	stream_write(out, "... ", 4);
        	stream_flush(out);
			if (!readline(in, buf, CLI_SIZE_OF_BUFFER, &n)) break;
			if (!input_provider_append(ip, buf, n)) break;
			continue;
		}
		if (st == PARSE_STATUS_ERROR) {
	        stream_write(out, "parse error\n", 12);
    	    stream_flush(out);
	        input_provider_reset_chunks(ip); //close+recreate chunks_stream
	        need_prompt = true;
    	    continue;
		}
		if (st == PARSE_STATUS_EOF) {
			need_prompt = true;
        	continue;
		}
		if (st == PARSE_STATUS_OK) {//<here> pb with the symbols
			if (!resolver_resolve_ast(&parsed_ast, &rctx)) break;
			//printf("parsed_ast->type: %i\n", parsed_ast->type);
			//if (parsed_ast->type == AST_TYPE_DATA_WRAPPER) printf("parsed_ast->data->type: %i\n", parsed_ast->data->type);
			if (!runtime_session_store_ast(parsed_ast, rs)) break;
			ist = interpreter_eval(
				ic,
				runtime_session_get_env(rs),
			    parsed_ast,
    			&value );
			//printf("ist: %i\n", ist);
			// stream_write(out, "This is ok\n", 11);
			//printf("value->type: %i\n", value->type);
			if (!cli_print(ic, value)) break;
			continue;
		}


		//
		// stream_flush(rs->ip->...???)

		// break; // when?how?
	}

	stream_close(in); //sure?
	stream_close(out); //sure?
	input_provider_destroy(ip);
	runtime_session_destroy(rs);
	interpreter_ctx_destroy(ic);

	return EXIT_SUCCESS;
}



/* draft of next main
parser_ops pops = {
    .create_int_node = ast_create_int_node,
    .create_string_node = ast_create_string_node,
    .create_symbol_name_node = ast_create_symbol_name_node,
    .create_error_node_or_sentinel = ast_create_error_node_or_sentinel,
    .create_children_node_var = ast_create_children_node_var,
    .destroy = ast_destroy,
    .children_append_take = ast_children_append_take,
};
parser_cfg pctx = {
    .ops = pops,
};
symtab *st = symtab_wind_scope(NULL);
resolver_ops rops {
    .push = list_push,
    .pop = list_pop,
    .intern_symbol = symtab_intern_symbol,
    .get = symtab_get,
};
resolver_ctx rctx = {
    .ops = rops,
    .st = *st,
};
// interpreter MUST NOT KNOW SYMBOL TABLE!
interpreter_ctx ictx = {
    .symbol_table = *symbol_table,
};
value *out = malloc(sizeof(value));
while true {
    printf("> "); fflush(stdout);

    char *line = osal_readline();
    if (!line) break;

    if (strcmp(line, ":q\n") == 0 || strcmp(line, ":quit\n") == 0) {
        free(line);
        break;
    }
    if (line[0] == '\n') {
        free(line);
        continue;
    }

    ast *root = NULL;
    if (parse_string(line, &pctx, &root) != 0 || !root) {
        fprintf(stderr, "parse error\n");
        free(line);
        continue;
    }
    free(line);

    if (root->type == AST_TYPE_ERROR) {
        printf("ast error (construction)\n");
        ast_destroy(root);
        continue;
    }

    if (resolver_resolve_ast(root, rctx) != 0) {
        printf("resolve error\n");
        ast_destroy(root);
        continue;
    }

    value out = value_nil();
    int rc = interpreter_eval(ictx, root, &out);

    print_value(out);
    interpreter_destroy_value(&out);
    ast_destroy(root);
}
symtab_destroy(st);
return 0;

int parse_string(const char *src, parser_cfg *pctx, ast **out_ast) {
    yyscan_t scanner;
    if (yylex_init(&scanner)) return 1;

    YY_BUFFER_STATE buf = yy_scan_string(src, scanner);
    if (!buf) { yylex_destroy(scanner); return 1; }

    int status = yyparse(scanner, out_ast, pctx);
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
    return status;
}
*/
