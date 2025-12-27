// src/resolver/src/resolver.c

#include "resolver.h"

#include "internal/resolver_memory_allocator.h"
#include "list.h"
#include "ast.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

const resolver_ops RESOLVER_OPS_DEFAULT = {
    .push = list_push,
    .pop  = list_pop,
    .intern_symbol = symtab_intern_symbol,
    .get  = symtab_get,
    .wind_scope = symtab_wind_scope,
    .unwind_scope = symtab_unwind_scope,
};

typedef struct {
    ast *node;
    ast *parent;
    size_t parent_idx;
} frame;

static const frame FRAME_SENTINEL_WIND = {0};
static const frame FRAME_SENTINEL_UNWIND = {0};

static void destroy_frame_adapter(void *item, void *user_data) {
    (void)user_data;
    if (
            item != NULL &&
            item != &FRAME_SENTINEL_WIND &&
            item != &FRAME_SENTINEL_UNWIND ) {
        RESOLVER_FREE(item);
    }
}

static void resolver_fatal_oom(ast **a, list *stack) {
    if (stack && *stack) {
        list_free_list(*stack, destroy_frame_adapter, NULL);
        *stack = NULL;
    }
    if (a && *a) {
        ast_destroy(*a);
        *a = NULL;
    }
}

static bool push_child_frame(
        ast **a,
        resolver_ctx *ctx,
        list *stack_p,
        ast *parent,
        size_t child_idx) {

    frame *fr = RESOLVER_MALLOC(sizeof *fr);
    if (!fr) {
        resolver_fatal_oom(a, stack_p);
        return false;
    }

    fr->node = parent->children->children[child_idx];
    fr->parent = parent;
    fr->parent_idx = child_idx;

    list stack_tmp = ctx->ops.push(*stack_p, fr);
    if (!stack_tmp) {
        RESOLVER_FREE(fr);
        resolver_fatal_oom(a, stack_p);
        return false;
    }

    *stack_p = stack_tmp;
    return true;
}

static bool push_sentinel(
        ast **a,
        resolver_ctx *ctx,
        list *stack_p,
        const frame *sentinel) {
    list stack_tmp = ctx->ops.push(*stack_p, (void*)sentinel);
    if (!stack_tmp) {
        resolver_fatal_oom(a, stack_p);
        return false;
    }

    *stack_p = stack_tmp;
    return true;
}

bool resolver_resolve_ast(ast **a, resolver_ctx *ctx) {

    bool ret = true;
    if (!a || !ctx)
        return false;

    if (!*a) {
        *a = ast_create_error_node_or_sentinel(
            RESOLVER_ERROR_CODE_NULL_ROOT,
            "resolver: null root AST pointer (nothing to resolve)" );
        return false;
    }

    frame *root_frame = RESOLVER_MALLOC(sizeof(frame));
    if (!root_frame) {
        resolver_fatal_oom(a, NULL);
        return false;
    } else {
        root_frame->node = *a;
        root_frame->parent = NULL;
        root_frame->parent_idx = SIZE_MAX;
    }

    list stack = ctx->ops.push(NULL, root_frame);
    if (!stack) {
        RESOLVER_FREE(root_frame);
        resolver_fatal_oom(a, NULL);
        return false;
    }


    while (stack) {

        frame *current_frame = (frame *) ctx->ops.pop(&stack);

        if (current_frame == &FRAME_SENTINEL_WIND) {
            ctx->st = ctx->ops.wind_scope(ctx->st);
            if (!ctx->st) {
                resolver_fatal_oom(a, &stack);
                return false;
            }
            continue;
        }

        if (current_frame == &FRAME_SENTINEL_UNWIND) {
            ctx->st = ctx->ops.unwind_scope(ctx->st);
            continue;
        }
        ast *current_ast = current_frame->node;

        if (
                   current_ast
                && current_ast->type == AST_TYPE_DATA_WRAPPER
                && current_ast->data
                && current_ast->data->type == TYPE_SYMBOL_NAME ) {
            const char *name = current_ast->data->data.string_value;

            // intern
            if (ctx->ops.intern_symbol(ctx->st, name) == 1) {
                ast *error_node = ast_create_error_node_or_sentinel(
                    RESOLVER_ERROR_CODE_SYMBOL_INTERN_FAILED,
                    "resolver: failed to intern symbol during promotion from SYMBOL_NAME"
                );
                ast_destroy(current_ast);

                if (current_frame->parent) {
                    current_frame->parent->children->children[current_frame->parent_idx] = error_node;
                } else {
                    *a = error_node;
                }

                ret = false;
                RESOLVER_FREE(current_frame);
                continue;
            }

            // get
            symbol *s = ctx->ops.get(ctx->st, name);
            if (!s) {
                ast *error_node = ast_create_error_node_or_sentinel(
                    RESOLVER_ERROR_CODE_SYMBOL_LOOKUP_FAILED,
                    "resolver: unretrievable interned symbol (inconsistent symtab state)"
                );
                ast_destroy(current_ast);

                if (current_frame->parent) {
                    current_frame->parent->children->children[current_frame->parent_idx] = error_node;
                } else {
                    *a = error_node;
                }

                ret = false;
                RESOLVER_FREE(current_frame);
                continue;
            }

            // store
            if (ctx->ops.store_symbol) {
                if (!ctx->ops.store_symbol(s, ctx->user_data)) {
                    ast *error_node = ast_create_error_node_or_sentinel(
                        RESOLVER_ERROR_CODE_SYMBOL_STORE_FAILED,
                        "resolver: failed to store symbol in symbol pool"
                    );
                    ast_destroy(current_ast);

                    if (current_frame->parent) {
                        current_frame->parent->children->children[current_frame->parent_idx] = error_node;
                    } else {
                        *a = error_node;
                    }

                    ret = false;
                    RESOLVER_FREE(current_frame);
                    continue;
                }
            }

            RESOLVER_FREE(current_ast->data->data.string_value);
            current_ast->data->data.symbol_value = s;
            current_ast->data->type = TYPE_SYMBOL;
        }

        if (ast_can_have_children(current_ast)) {

            if (current_ast->type == AST_TYPE_FUNCTION) {

                if (!push_sentinel(a, ctx, &stack, &FRAME_SENTINEL_UNWIND)) {
                    RESOLVER_FREE(current_frame);
                    return false;
                }

                // push body and parameters frames between wind and unwind
                // sentinel frames for symbol resolution into a new scope
                for (size_t i = 3 ; i-- > 1 ; )
                    if (!push_child_frame(a, ctx, &stack, current_ast, i)) {
                        RESOLVER_FREE(current_frame);
                        return false;
                }

                // push wind sentinel frame
                if (!push_sentinel(a, ctx, &stack, &FRAME_SENTINEL_WIND)) {
                    RESOLVER_FREE(current_frame);
                    return false;
                }

                // push symbol name frame NOT between wind and unwind
                // sentinel frames
                if (!push_child_frame(a, ctx, &stack, current_ast, 0)) {
                    RESOLVER_FREE(current_frame);
                    return false;
                }


            } else { // can have children but not a function
                for (size_t i = current_ast->children->children_nb ; i-- > 0 ; )
                    if (!push_child_frame(a, ctx, &stack, current_ast, i)) {
                        RESOLVER_FREE(current_frame);
                        return false;
                    }
            }

        }

        RESOLVER_FREE(current_frame);
    }
    return ret;
}
