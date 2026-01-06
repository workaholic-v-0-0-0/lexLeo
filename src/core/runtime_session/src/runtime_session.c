// src/core/runtime_session/src/runtime_session.c

#include "internal/runtime_session_internal.h"

#include "lexer_legacy.h"
#include "runtime_env.h"
#include "symtab.h"
#include "ast.h"
#include "input_provider_legacy.h"

typedef void* yyscan_t;
int yylex_init (yyscan_t* scanner);
int yylex_destroy ( yyscan_t yyscanner );

static const lexer_legacy_ops_t RUNTIME_SESSION_LEXER_OPS_DEFAULT = {
	.lexer_init_fn = yylex_init,
	.lexer_destroy_fn = yylex_destroy
};

static runtime_session_ctx g_runtime_session_ctx = {
	.lexer_ops = &RUNTIME_SESSION_LEXER_OPS_DEFAULT
};

struct runtime_session *runtime_session_create(void) {
	runtime_session *ret = RUNTIME_SESSION_MALLOC(sizeof(struct runtime_session));
	if (!ret) return NULL;
	ret->in = NULL;
	if (g_runtime_session_ctx.lexer_ops->lexer_init_fn(&ret->scanner) != 0) {
		RUNTIME_SESSION_FREE(ret);
		return NULL;
	}
	ret->st = symtab_wind_scope(NULL);
	if (!ret->st) {
		g_runtime_session_ctx.lexer_ops->lexer_destroy_fn(ret->scanner);
		RUNTIME_SESSION_FREE(ret);
		return NULL;
	}
	ret->env = runtime_env_wind(NULL);
	if (!ret->env) {
		g_runtime_session_ctx.lexer_ops->lexer_destroy_fn(ret->scanner);
		while ((ret->st = symtab_unwind_scope(ret->st)) != NULL) ;
		RUNTIME_SESSION_FREE(ret);
		return NULL;
	}
	ret->ast_pool = NULL;
	ret->symbol_pool = NULL;
	return ret;
}

static void runtime_session_destroy_symbol_adapter(void *item, void *user_data) {
	if (item) {
		RUNTIME_SESSION_FREE(((symbol *) item)->name);
		RUNTIME_SESSION_FREE(item);
	}
}

static void runtime_session_cleanup_symbol_pool(runtime_session *session) {
	list_free_list(
		session->symbol_pool,
		runtime_session_destroy_symbol_adapter,
		NULL );
	session->symbol_pool = NULL;
}

bool runtime_session_store_symbol(struct symbol *sym, runtime_session *session) {
	if (!session || !sym) return false;
	if (list_contains(session->symbol_pool, sym)) return true;
	list l = list_push(session->symbol_pool, sym);
	if (!l) return false;
	session->symbol_pool = l;
	return true;
}

// it will be only called by cli!
bool runtime_session_store_ast(struct ast *root, runtime_session *session) {
	if (!session || !root) return false;
	if (list_contains(session->ast_pool, root)) return true;
	list l = list_push(session->ast_pool, root);
	if (!l) return false;
	session->ast_pool = l;
	return true;
}

static void runtime_session_destroy_ast_adapter(void *item, void *user_data) {
	if (item) ast_destroy((ast *) item);
}

static void runtime_session_cleanup_ast_pool(runtime_session *session) {
	list_free_list(
		session->ast_pool,
		runtime_session_destroy_ast_adapter,
		NULL );
	session->ast_pool = NULL;
}

void runtime_session_destroy(struct runtime_session *session) {
	if (!session) return;
	session->in = NULL; // exlicit ; is borrow from client code
	if (session->scanner) g_runtime_session_ctx.lexer_ops->lexer_destroy_fn(session->scanner);
	while ((session->st = symtab_unwind_scope(session->st)) != NULL) ;
	runtime_session_cleanup_symbol_pool(session);
	while ((session->env = runtime_env_unwind(session->env)) != NULL) ;
	runtime_session_cleanup_ast_pool(session);
	RUNTIME_SESSION_FREE(session);
}

bool runtime_session_bind_input_provider(
		runtime_session *session,
		struct input_provider *provider ) {
	if (!session) return false;
	if (!provider) {
		session->in = NULL;
		return true;
	}
	if (!input_provider_legacy_bind_to_scanner(provider, session->scanner)) return false;
	session->in = provider;
	return true;
}

bool runtime_session_bind_output_stream(
		runtime_session *session,
		struct stream_legacy *out) {
	if (!session) return false;
	session->out = out;
	return true;
}


// getters

yyscan_t runtime_session_get_scanner(runtime_session *session) {
	return session ? session->scanner : NULL;
}

struct runtime_env *runtime_session_get_env(runtime_session *session) {
	return session ? session->env : NULL;
}

struct symtab *runtime_session_get_symtab( runtime_session *session) {
	return session ? session->st : NULL;
}



// setters and getters for dependency injection

void runtime_session_set_lexer_ops(const lexer_legacy_ops_t *overrides) {
	static lexer_legacy_ops_t applied;
	applied = *g_runtime_session_ctx.lexer_ops;

	if (overrides) {
		if (overrides->lexer_init_fn)
			applied.lexer_init_fn = overrides->lexer_init_fn;
		if (overrides->lexer_destroy_fn)
			applied.lexer_destroy_fn = overrides->lexer_destroy_fn;
	}
	g_runtime_session_ctx.lexer_ops = &applied;
}

void runtime_session_reset_lexer_ops(void) {
	g_runtime_session_ctx.lexer_ops = &RUNTIME_SESSION_LEXER_OPS_DEFAULT;
}

void runtime_session_set_lexer_init_fn(lexer_init_fn_t fn) {
	lexer_legacy_ops_t overrides = {0};
	overrides.lexer_init_fn = fn;
	runtime_session_set_lexer_ops(&overrides);
}

void runtime_session_set_lexer_destroy_fn(lexer_destroy_fn_t fn) {
	lexer_legacy_ops_t overrides = {0};
	overrides.lexer_destroy_fn = fn;
	runtime_session_set_lexer_ops(&overrides);
}

lexer_init_fn_t runtime_session_get_lexer_init_fn(void) {
	return g_runtime_session_ctx.lexer_ops->lexer_init_fn;
}

lexer_destroy_fn_t runtime_session_get_lexer_destroy_fn(void) {
	return g_runtime_session_ctx.lexer_ops->lexer_destroy_fn;
}





// DEBUG TOOLS

#ifdef UNIT_TEST

#include <stdio.h>

char *runtime_session_symbol_pool_to_string(struct runtime_session *session) {
	size_t capacity = 128;
	size_t length = 0;
	char *buf = RUNTIME_SESSION_MALLOC(capacity);
	if (!buf) return NULL;
	int written = snprintf(buf, capacity, "Symbol pool [length=%zu]:", list_length(session->symbol_pool));
	if (written < 0) { RUNTIME_SESSION_FREE(buf); return NULL; }
	length = (size_t)written;
	size_t idx = 0;
	for (list it = session->symbol_pool; it != NULL; it = it->cdr, idx++) {
		symbol *s = (symbol *)it->car;
		const char *name = (s && s->name) ? s->name : "(null symbol)";
		written = snprintf(NULL, 0, "\n  [%zu] %s", idx, name);
		if (length + (size_t)written + 1 > capacity) {
			while (length + (size_t)written + 1 > capacity) {
				capacity *= 2;
			}
			char *tmp = RUNTIME_SESSION_REALLOC(buf, capacity);
			if (!tmp) { RUNTIME_SESSION_FREE(buf); return NULL; }
			buf = tmp;
		}
		snprintf(buf + length, capacity - length, "\n  [%zu] %s", idx, name);
		length += (size_t)written;
	}
	return buf;
}

#endif
