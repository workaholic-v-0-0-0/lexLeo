// src/cli/src/main.c

#include "osal.h"
#include <stdio.h>

int main() {
#ifdef DEBUG
    osal_sleep(1000);
    printf("Hello World from debug build!\n");
#elif RELEASE
    printf("Hello World from release build!\n");
#endif
    return 0;
}

// near the end:
// symtab_cleanup_pool();

// BE CAREFULL!!! MAYBE IN RESOLVER WE HAVE TO CHECK IF INTERN
// DO NOT MAKE TROUBLE WHEN THE SYMBOL IS ALREADY INTERNED

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
parser_ctx pctx = {
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

int parse_string(const char *src, parser_ctx *pctx, ast **out_ast) {
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
