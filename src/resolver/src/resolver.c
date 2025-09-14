// src/resolver/src/resolver.c

#include <stdint.h>

#include "resolver.h"

#include "internal/resolver_memory_allocator.h"
#include "list.h"
#include "ast.h"

const resolver_ops RESOLVER_OPS_DEFAULT = {
    .push = list_push,
    .pop  = list_pop,
    .intern_symbol = symtab_intern_symbol,
    .get  = symtab_get,
};

typedef struct {
    ast *node;
    ast *parent;
    size_t parent_idx;
} frame;

int resolver_resolve_ast(ast **a, resolver_ctx ctx) {
    int ret = 0;
    if (!a)
        return 1;

    if (!*a) {
        *a = ast_create_error_node_or_sentinel(
            RESOLVER_ERROR_CODE_NULL_ROOT,
            "resolver: null root AST pointer (nothing to resolve)" );
        return 1;
    }

    frame *root_frame = RESOLVER_MALLOC(sizeof(frame));
    if (!root_frame) {
        ast_destroy(*a);
        *a = ast_create_error_node_or_sentinel(
            RESOLVER_ERROR_CODE_ALLOC_FRAME_FAILED,
            "resolver: out of memory while allocating root traversal frame" );
        return 1;
    } else {
        root_frame->node = *a;
        root_frame->parent = NULL;
        root_frame->parent_idx = SIZE_MAX;
    }

    list stack = ctx.ops.push(NULL, root_frame);
    if (!stack) {
        ast_destroy(*a);
        RESOLVER_FREE(root_frame);
        *a = ast_create_error_node_or_sentinel(
            RESOLVER_ERROR_CODE_TRAVERSAL_STACK_INIT_FAILED,
            "resolver: failed to initialize traversal stack"
        );
        return 1;
    }

    while (stack) {
        frame *current_frame = (frame *) ctx.ops.pop(&stack);
        ast *current_ast = current_frame->node;
        if (ast_can_have_children(current_ast)) {
            ast **children = current_ast->children->children;
            for (size_t i = current_ast->children->children_nb ; i-- > 0 ; ) {
                frame *f = RESOLVER_MALLOC(sizeof(frame));
                if (!f) {
                    ast_destroy(children[i]);
                    children[i] = ast_create_error_node_or_sentinel(
                        RESOLVER_ERROR_CODE_TRAVERSAL_PUSH_FAILED,
                        "\
resolver: failed to push child node onto traversal stack" );
                    ret = 1;
                } else {
                    f->node = children[i];
                    f->parent = current_ast;
                    f->parent_idx = i;

                    list stack_tmp = ctx.ops.push(stack, f);
                    if (!stack_tmp) {
                        ast_destroy(children[i]);
                        RESOLVER_FREE(f);
                        children[i] = ast_create_error_node_or_sentinel(
                            RESOLVER_ERROR_CODE_TRAVERSAL_PUSH_FAILED,
                            "\
resolver: failed to push child node onto traversal stack"
                        );
                        ret = 1;
                    } else {
                        stack = stack_tmp;
                    }
                }
            }
        } else if (ast_is_data_of(current_ast, TYPE_SYMBOL_NAME)) {
            // make internment symbol relative to symbol name leaf
            if (ctx.ops.intern_symbol(
                    ctx.st,
                    current_ast->data->data.string_value )
                    == 1 ) {
                ast *error_node = ast_create_error_node_or_sentinel(
                    RESOLVER_ERROR_CODE_SYMBOL_INTERN_FAILED,
                    "\
resolver: failed to intern symbol during promotion from SYMBOL_NAME"
                );
                ast_destroy(current_ast);
                if (current_frame->parent) {
                    current_frame->parent->children->children
                            [current_frame->parent_idx]
                        = error_node;
                } else {
                    *a = error_node;
                }
                ret = 1;
            } else { // promote into symbol data wrapper ast
                symbol *s =
                    ctx.ops.get(
                        ctx.st,
                        current_ast->data->data.string_value );
                if (!s) {
                    ast *error_node = ast_create_error_node_or_sentinel(
                        RESOLVER_ERROR_CODE_SYMBOL_LOOKUP_FAILED,
                        "\
resolver: unretrievable interned symbol (inconsistent symtab state)" );
                    ast_destroy(current_ast);
                    if (current_frame->parent) {
                        current_frame->parent->children->children
                                [current_frame->parent_idx] =
                            error_node;
                    } else {
                        *a = error_node;
                    }
                    ret = 1;
                } else {
                    RESOLVER_FREE(current_ast->data->data.string_value);
                    current_ast->data->data.symbol_value = s;
                    current_ast->data->type = TYPE_SYMBOL;
                }
            }
        }
        RESOLVER_FREE(current_frame);
    }
    return ret;
}
