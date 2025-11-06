// src/io/input_provider/include/internal/input_provider_ctx.h

#ifndef LEXLEO_INPUT_PROVIDER_CTX_H
#define LEXLEO_INPUT_PROVIDER_CTX_H

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;

// write here forward declaration of proto used by implementation

typedef YY_BUFFER_STATE (*yy_scan_bytes_fn_t)(const char *bytes, int len, yyscan_t scanner);
typedef void (*yy_delete_buffer_fn_t)(YY_BUFFER_STATE b, yyscan_t scanner);
typedef void (*yyrestart_fn_t)(FILE *input_file, yyscan_t scanner);
typedef void (*yy_switch_to_buffer_fn_t)(YY_BUFFER_STATE b, yyscan_t scanner);

typedef struct lexer_ops_t {
	yy_scan_bytes_fn_t yy_scan_bytes_fn;
	yy_delete_buffer_fn_t yy_delete_buffer_fn;
	yyrestart_fn_t yyrestart_fn;
	yy_switch_to_buffer_fn_t yy_switch_to_buffer_fn;
} lexer_ops_t;

typedef struct input_provider_ctx {
	const lexer_ops_t *lexer_ops;
} input_provider_ctx;




// setters and getters

void input_provider_set_lexer_ops(const lexer_ops_t *overrides);
void input_provider_reset_lexer_ops(void);
void input_provider_set_yy_scan_bytes_fn(yy_scan_bytes_fn_t yy_scan_bytes_fn);
void input_provider_set_yy_delete_buffer_fn(yy_delete_buffer_fn_t yy_delete_buffer_fn);
void input_provider_set_yyrestart_fn(yyrestart_fn_t yyrestart_fn);
void input_provider_set_yy_switch_to_buffer_fn(yy_switch_to_buffer_fn_t yy_switch_to_buffer_fn);

yy_scan_bytes_fn_t input_provider_get_yy_scan_bytes_fn(void);
yy_delete_buffer_fn_t input_provider_get_yy_delete_buffer_fn(void);
yyrestart_fn_t input_provider_get_yyrestart_fn(void);
yy_switch_to_buffer_fn_t input_provider_get_yy_switch_to_buffer_fn(void);

#endif //LEXLEO_INPUT_PROVIDER_CTX_H
