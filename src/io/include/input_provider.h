// src/io/include/input_provider.h

#ifndef LEXLEO_INPUT_PROVIDER_H
#define LEXLEO_INPUT_PROVIDER_H

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "lexer.yy.h"

/* The input_provider is an abstraction layer between the reading_engine (I/O)
 * and the Flex lexer. It manages what is given to the lexer as input.
 *
 * Two operating modes exist:
 *
 *   1. INPUT_PROVIDER_MODE_CHUNKS
 *      - Input data is stored in an internal dynamic memory buffer.
 *      - The provider owns and manages the YY_BUFFER_STATE created via
 *        yy_scan_bytes().
 *      - It creates a new buffer when publishing data, and destroys the old one
 *        with yy_delete_buffer().
 *      - Used mainly for REPL / incremental input (interactive mode).
 *
 *   2. INPUT_PROVIDER_MODE_FILE
 *      - Input data comes from a FILE* stream.
 *      - The provider does NOT own the YY_BUFFER_STATE; it relies on
 *        yyset_in() or yyrestart() from Flex to attach the stream to the
 *        scanner.
 *      - The provider does NOT own the FILE*; it is opened and closed by the
 *        reading_engine.
 *      - Used for “exec <file>” type inputs.
 *
 * The input_provider does not perform any I/O itself.
 * The reading_engine is responsible for feeding data (read lines, open files)
 * and instructing the provider when to publish the new content to the scanner.
 */

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
bool input_provider_bind_to_scanner(input_provider *p, yyscan_t scanner); // one bind per session ; scanner is borrowd



//-----------------------------------------------------------------------------
// mode switching (driven by reading_engine)
//-----------------------------------------------------------------------------


bool input_provider_set_mode_chunks(input_provider *p);
bool input_provider_set_mode_file(input_provider *p);



//-----------------------------------------------------------------------------
// CHUNKS mode buffer management (called by reading_engine)
//-----------------------------------------------------------------------------


// Clears the internal dynamic buffer.
bool input_provider_buffer_reset(input_provider *p);

// Appends raw bytes to the internal dynamic buffer.
// `bytes` is borrowed from reading_engine (not owned by input_provider).
bool input_provider_buffer_append(input_provider *p, const char *bytes, size_t len); // bytes is borrowed from reading_engine



//-----------------------------------------------------------------------------
// FILE mode management (called by reading_engine)
//-----------------------------------------------------------------------------


// Sets the FILE* to be used as input (borrowed from reading_engine).
bool input_provider_set_file(input_provider *p, FILE *f); // f is borrowed from reading_engine



//-----------------------------------------------------------------------------
// Publishing to the Flex scanner
//-----------------------------------------------------------------------------



// Updates the scanner input depending on the current mode.
//   - In CHUNKS mode: creates a new YY_BUFFER_STATE with yy_scan_bytes().
//   - In FILE mode: attaches the FILE* with yyrestart() or yyset_in().
bool input_provider_publish(input_provider *p);

#endif //LEXLEO_INPUT_PROVIDER_H



/* Notes:
 * The reading_engine orchestrates the process:
 *   - Reads lines or opens files.
 *   - Calls buffer_append(), buffer_reset(), set_file().
 *   - Calls publish() before invoking parse_*() functions.
 */
