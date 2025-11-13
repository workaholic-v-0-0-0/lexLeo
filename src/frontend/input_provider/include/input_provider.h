// src/io/input_provider/include/input_provider.h

/* The input_provider is an abstraction layer between the parse_engine
 * and the Flex lexer. It manages what is given to the lexer as input.
 *
 * Two operating modes exist:
 *
 *   1. INPUT_PROVIDER_MODE_CHUNKS
 *      - Input data is accumulated in an internal dynamic memory buffer.
 *      - The provider owns and manages the YY_BUFFER_STATE created via
 *        yy_scan_bytes().
 *      - On publish(), it creates a fresh buffer (yy_scan_bytes) and
 *        deletes the previous one (yy_delete_buffer) if any.
 *      - Typical uses:
 *          * REPL / incremental input (line-by-line).
 *          * Interpreter-driven interactive reads that may require
 *            multiple user chunks until a full construct can be parsed
 *            (e.g., when evaluating an AST of type AST_TYPE_READING).
 *
 *   2. INPUT_PROVIDER_MODE_FILE
 *      - Input data comes from a FILE* stream.
 *      - The provider does NOT own the YY_BUFFER_STATE; it relies on
 *        yyset_in() or yyrestart() from Flex to attach the stream to the
 *        scanner.
 *      - The provider does NOT own the FILE*; it is opened and closed by the
 *        parse_engine.
 *      - Used for “exec <file>” type inputs.
 *
 * Responsibilities & ownership:
 *   - input_provider never performs I/O: the frontend provides bytes/streams
 *     using adapters (see src/io/adapters/*) behind frontend/ports/*.
 *   - In CHUNKS mode, input_provider owns the Flex buffer it creates and must
 *     delete it on republish() and destroy().
 *   - In FILE mode, the FILE* lifetime is managed by the caller;
 *     input_provider only (re)attaches it to the scanner.
 *   - The scanner (yyscan_t) is borrowed: created/destroyed by the caller.
 */

#ifndef LEXLEO_INPUT_PROVIDER_H
#define LEXLEO_INPUT_PROVIDER_H

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

typedef enum {
    INPUT_PROVIDER_MODE_CHUNKS,
    INPUT_PROVIDER_MODE_FILE
} input_provider_mode;

typedef struct input_provider input_provider;



//-----------------------------------------------------------------------------
// Creation, destruction and scanner binding
//-----------------------------------------------------------------------------


input_provider *input_provider_create(void);

void input_provider_destroy(input_provider *p);

// one bind per session ; scanner is borrowed
bool input_provider_bind_to_scanner(input_provider *p, yyscan_t scanner);



//-----------------------------------------------------------------------------
// mode switching (driven by parse_engine)
//-----------------------------------------------------------------------------


bool input_provider_set_mode_chunks(input_provider *p);
bool input_provider_set_mode_file(input_provider *p);



//-----------------------------------------------------------------------------
// CHUNKS mode buffer management (called by parse_engine)
//-----------------------------------------------------------------------------


// Clears the internal dynamic buffer.
bool input_provider_buffer_reset(input_provider *p);

// Appends raw bytes to the internal dynamic buffer.
// `bytes` is borrowed from parse_engine (not owned by input_provider).
bool input_provider_buffer_append(
		input_provider *p,
		const char *bytes, // bytes is borrowed from parse_engine
		size_t len );

// Same input_provider_buffer_append but add '\n' at the end
bool input_provider_buffer_append_line(
		input_provider *p,
		const char *bytes,
		size_t len );



//-----------------------------------------------------------------------------
// FILE mode management (called by parse_engine)
//-----------------------------------------------------------------------------


// Sets the FILE* to be used as input
bool input_provider_set_file(input_provider *p, FILE *f); // f is borrowed



//-----------------------------------------------------------------------------
// Publishing to the Flex scanner
//-----------------------------------------------------------------------------


// Updates the scanner input depending on the current mode.
//   - In CHUNKS mode: creates a new YY_BUFFER_STATE with yy_scan_bytes().
//   - In FILE mode: attaches the FILE* with yyrestart() or yyset_in().
bool input_provider_publish(input_provider *p);

#endif //LEXLEO_INPUT_PROVIDER_H
