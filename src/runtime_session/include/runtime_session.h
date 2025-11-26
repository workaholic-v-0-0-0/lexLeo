// src/runtime_session/include/runtime_session.h

#ifndef LEXLEO_RUNTIME_SESSION_H
#define LEXLEO_RUNTIME_SESSION_H

#include <stddef.h>
#include <stdbool.h>

struct input_provider;
struct runtime_env;
struct symtab;
struct ast;
struct symbol;

struct runtime_session; // opaque
typedef struct runtime_session runtime_session;

struct runtime_session *runtime_session_create(void);
void runtime_session_destroy(runtime_session *session);

struct runtime_env *runtime_session_get_env(runtime_session *session);
struct symtab *runtime_session_get_symtab( runtime_session *session);

// Bind the given input provider to the session's scanner.
//
// Purpose:
//   Makes the session read all future lexer input from `provider`. After a
//   successful bind, any change made to `provider` (mode switches, chunk
//   appends, stream replacement, etc.) is automatically reflected in the
//   scanner via the YY_INPUT hook set by input_provider_bind_to_scanner().
//
// Ownership:
//   - `provider` is *borrowed*: runtime_session does not take ownership and
//     will not destroy it. The caller remains responsible for the lifetime of
//     the input_provider.
//   - Passing NULL detaches the current provider (session will no longer feed
//     the scanner from an input_provider).
//
// Returns:
//   true  on success,
//   false on error (e.g. NULL session, provider binding failure).
//
// Notes:
//   - Only one provider can be bound at a time; a new bind overrides the
//     previous one.
//   - This function does *not* reset or recreate the session's scanner.
bool runtime_session_bind_input_provider(
	runtime_session *session,
	struct input_provider *provider );

// Try to register `root` into the session's ast_pool.
// Ownership:
//   - On success: runtime_session takes ownership of `root` and will destroy it
//     during runtime_session_destroy().
//   - On failure: ownership of `root` is unchanged; caller remains responsible
//     for destroying it (if needed).
bool runtime_session_store_ast(struct ast *root, runtime_session *session);

// Try to register `sym` into the session's symbol_pool.
// PRE: `sym` is already a valid, interned symbol referenced by the symtab.
// Ownership:
//   - On success: runtime_session keeps a *reference* to `sym` in its pool.
//   - On failure: ownership of `sym` is unchanged (caller MUST NOT free it
//     just because this function returned false).
// Returns:
//   true  on success (stored or already present in pool),
//   false on infrastructure error (e.g., OOM when pushing to the list).
bool runtime_session_store_symbol(struct symbol *sym, runtime_session *session);

#endif //LEXLEO_RUNTIME_SESSION_H
