// src/ast/tests/test_ast.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>

#include "memory_allocator.h"
#include "string_utils.h"
#include "logger.h"
#include "list.h"

#include "internal/ast_test_utils.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static list collected_ptr_to_be_freed = NULL;

static char dummy[10];
static const void *DUMMY_MALLOC_RETURNED_VALUE = (void*)&dummy[0];
static const void *DUMMY_STRDUP_RETURNED_VALUE = &dummy[1];
#define MALLOC_ERROR_CODE NULL
static const int DUMMY_INT = 7;
static char *const DUMMY_STRING = "dummy string";
static void *const DUMMY_SYMBOL = (void*)&dummy[2];
#define STRDUP_ERROR_CODE NULL
static typed_data *const DUMMY_TYPED_DATA_P = (typed_data*)&dummy[3];
static char *string_value = NULL;
static char *symbol_value = NULL;
static typed_data *typed_data_int_p = NULL;
static typed_data *typed_data_string_p = NULL;
static typed_data *typed_data_symbol_p = NULL;
static ast *const TYPED_DATA_WRAPPER_DEFINED_IN_SETUP = (ast *) &dummy;
static typed_data *const TYPED_DATA_INT_DEFINED_IN_SETUP = (typed_data *) &dummy;
static typed_data *const TYPED_DATA_STRING_DEFINED_IN_SETUP = (typed_data *) &dummy;
static typed_data *const TYPED_DATA_SYMBOL_DEFINED_IN_SETUP = (typed_data *) &dummy;
static const int DUMMY_DATA_TYPE = 6;
static ast *const DUMMY_DATA_WRAPPER = (ast*)&dummy[4];
static const int DUMMY_CHILDREN_NB = 777;
static ast **const DUMMY_AST_CHILDREN = (ast**)&dummy[5];
static ast **const AST_CHILDREN_DEFINED_IN_SETUP = (ast **)&dummy[6];
static ast dummy_ast_child_one;
static ast dummy_ast_child_two;
static ast *DUMMY_CHILDREN_ARRAY_OF_SIZE_ONE[] = {&dummy_ast_child_one};
static ast *DUMMY_CHILDREN_ARRAY_OF_SIZE_TWO[] = {&dummy_ast_child_one, &dummy_ast_child_two};
static ast_children_t *const AST_CHILDREN_T_DEFINED_IN_SETUP = (ast_children_t *)&dummy[7];
static ast_children_t *const DUMMY_AST_CHILDREN_T_P = (ast_children_t *)&dummy[8];
static const int UNSUPPORTED_AST_TYPE = AST_TYPE_NB_TYPES;
static const int SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER = AST_TYPE_PROGRAM;
static ast *ast_not_data_wrapper = NULL;



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


void * mock_malloc(size_t size) {
    check_expected(size);
    return mock_type(void *);
}

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
}

char *mock_strdup(const char *s) {
    check_expected_ptr(s);
    return mock_type(char *);
}

static void *fake_malloc_returned_value_for_a_typed_data_int = NULL;
static void *fake_malloc_returned_value_for_a_typed_data_string = NULL;
static void *fake_malloc_returned_value_for_a_typed_data_symbol = NULL;
static void *fake_malloc_returned_value_for_an_ast = NULL;
static void *fake_malloc_returned_value_for_an_ast_children_t = NULL;
static void *fake_malloc_returned_value_for_a_double_ast_pointer = NULL;
static void *fake_typed_data_int_p = NULL;
static char *fake_strdup_returned_value_for_string_value = NULL;

void mock_ast_destroy_typed_data_int(typed_data *typed_data_int) {
    check_expected_ptr(typed_data_int);
}

void mock_ast_destroy_typed_data_string(typed_data *typed_data_string) {
    check_expected_ptr(typed_data_string);
}

void mock_ast_destroy_typed_data_symbol(typed_data *typed_data_symbol) {
    check_expected_ptr(typed_data_symbol);
}

void mock_ast_destroy_typed_data_wrapper(ast *ast_data_wrapper) {
    check_expected_ptr(ast_data_wrapper);
}

void mock_ast_destroy_non_typed_data_wrapper(ast *non_typed_data_wrapper) {
    check_expected_ptr(non_typed_data_wrapper);
}

ast_children_t *mock_ast_create_ast_children_arr(size_t children_nb, ast **children) {
    check_expected_ptr(children_nb);
    check_expected_ptr(children);
    return mock_type(ast_children_t *);
}

static ast_children_t *fake_ast_create_ast_children_arr_returned_value = NULL;

void mock_ast_destroy_ast_children(ast_children_t *ast_children) {
    check_expected_ptr(ast_children);
}



//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------


static void alloc_and_save_address_to_be_freed(void **ptr, size_t size) {
    *ptr = malloc(size);
    assert_non_null(*ptr);
    set_allocators(NULL, NULL);
    collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, *ptr);
    set_allocators(mock_malloc, mock_free);
}



//-----------------------------------------------------------------------------
// ast_create_typed_data_int TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_typed_data_int_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_typed_data_int_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls malloc with sizeof(typed_data)
static void create_typed_data_int_calls_malloc_for_a_typed_data(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    ast_create_typed_data_int(DUMMY_INT);
}

// Given: malloc fails
// Expected: return NULL
static void create_typed_data_int_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_typed_data_int(DUMMY_INT));
}

// Given: malloc succeeds
// Expected: the malloc'ed typed_data is initialized and returned
static void create_typed_data_int_initializes_and_returns_malloced_typed_data_when_malloc_succeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_typed_data_int, sizeof(typed_data));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_int);
    typed_data *ret = ast_create_typed_data_int(DUMMY_INT);
    assert_int_equal(ret->type, TYPE_INT);
    assert_int_equal((ret->data).int_value, DUMMY_INT);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_typed_data_int);
}



//-----------------------------------------------------------------------------
// ast_destroy_typed_data_int TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_typed_data_int_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_typed_data_int_p, sizeof(typed_data));
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int destroy_typed_data_int_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls free with typed_data_int
static void destroy_typed_data_int_calls_free(void **state) {
    expect_value(mock_free, ptr, fake_typed_data_int_p);
    ast_destroy_typed_data_int(fake_typed_data_int_p);
}



//-----------------------------------------------------------------------------
// ast_create_typed_data_string TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int ast_create_typed_data_string_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    set_string_duplicate(mock_strdup);
    return 0;
}

static int ast_create_typed_data_string_teardown(void **state) {
    set_string_duplicate(NULL);
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls malloc with sizeof(typed_data)
static void ast_create_typed_data_string_calls_malloc_for_a_typed_data(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid mock call
    ast_create_typed_data_string(DUMMY_STRING);
}

// Given: malloc fails
// Expected: returns NULL
static void ast_create_typed_data_string_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_typed_data_string(DUMMY_STRING));
}

// Given: malloc succeeds
// Expected: calls strdup with s
static void ast_create_typed_data_string_calls_strdup_when_malloc_succeds(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);
    ast_create_typed_data_string(DUMMY_STRING);
}

// Given: strdup fails
// Expected: calls free with pointer returned by malloc for a typed data and returns null
static void ast_create_typed_data_string_calls_free_and_returns_null_when_strdup_fails(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_typed_data_string, sizeof(typed_data));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_string);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, STRDUP_ERROR_CODE);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_a_typed_data_string);
    ast_create_typed_data_string(DUMMY_STRING);
}

// Given: strdup succeeds
// Expected: the malloc'ed typed_data is initialized and returned
static void ast_create_typed_data_string_initializes_and_returns_malloced_typed_data_when_strdup_succeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_typed_data_string, sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **)&fake_strdup_returned_value_for_string_value, strlen(DUMMY_STRING) + 1);
    strcpy(fake_strdup_returned_value_for_string_value, DUMMY_STRING);
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_string);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, fake_strdup_returned_value_for_string_value);
    typed_data *ret = ast_create_typed_data_string(DUMMY_STRING);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_typed_data_string);
    assert_int_equal(ret->type, TYPE_STRING);
    assert_ptr_equal(ret->data.string_value, fake_strdup_returned_value_for_string_value);
    assert_memory_equal(ret->data.string_value, DUMMY_STRING, strlen(DUMMY_STRING) + 1);
}



//-----------------------------------------------------------------------------
// ast_destroy_typed_data_string TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_typed_data_string_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **)&typed_data_string_p, sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **)&string_value, strlen(DUMMY_STRING)+1);
    typed_data_string_p->type = TYPE_STRING;
    typed_data_string_p->data.string_value = string_value;
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int destroy_typed_data_string_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls free with typed_data_string_p->data.string_value and then with typed_data_string_p
static void destroy_typed_data_string_calls_free_for_string_value_field_then_for_typed_data_string(void **state) {
    expect_value(mock_free, ptr, typed_data_string_p->data.string_value);
    expect_value(mock_free, ptr, typed_data_string_p);
    ast_destroy_typed_data_string(typed_data_string_p);
}



//-----------------------------------------------------------------------------
// ast_create_typed_data_symbol TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_typed_data_symbol_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_typed_data_symbol_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls malloc with sizeof(typed_data)
static void create_typed_data_symbol_calls_malloc_for_a_typed_data(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid mock call
    ast_create_typed_data_symbol(DUMMY_SYMBOL);
}

// Given: malloc fails
// Expected: return NULL
static void create_typed_data_symbol_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_typed_data_symbol(DUMMY_SYMBOL));
}

// Given: malloc succeeds
// Expected: the malloc'ed typed_data is initialized and returned
static void create_typed_data_symbol_initializes_and_returns_malloced_typed_data_when_malloc_succeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_typed_data_symbol, sizeof(typed_data));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_symbol);
    typed_data *ret = ast_create_typed_data_symbol(DUMMY_SYMBOL);
    assert_int_equal(ret->type, TYPE_SYMBOL);
    assert_ptr_equal((ret->data).symbol_value, DUMMY_SYMBOL);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_typed_data_symbol);
}



//-----------------------------------------------------------------------------
// ast_destroy_typed_data_symbol TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_typed_data_symbol_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **)&typed_data_symbol_p, sizeof(typed_data));
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int destroy_typed_data_symbol_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls free with typed_data_symbol
static void destroy_typed_data_symbol_calls_free(void **state) {
    expect_value(mock_free, ptr, typed_data_symbol_p);
    ast_destroy_typed_data_symbol(typed_data_symbol_p);
}



//-----------------------------------------------------------------------------
// ast_create_typed_data_wrapper TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_typed_data_wrapper_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_typed_data_wrapper_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls malloc with sizeof(typed_data)
static void create_typed_data_wrapper_calls_malloc_for_an_ast(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    ast_create_typed_data_wrapper(DUMMY_TYPED_DATA_P);
}

// Given: malloc fails
// Expected: return NULL
static void create_typed_data_wrapper_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_typed_data_wrapper(DUMMY_TYPED_DATA_P));
}

// Given: malloc succeeds
// Expected: the malloc'ed typed_data is initialized and returned
static void create_typed_data_wrapper_initializes_and_returns_malloced_ast_when_malloc_succeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    ast *ret = ast_create_typed_data_wrapper(DUMMY_TYPED_DATA_P);
    assert_int_equal(ret->type, AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->data, DUMMY_TYPED_DATA_P);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
}



//-----------------------------------------------------------------------------
// ast_destroy_typed_data_wrapper TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURES
//----------------------------------------------------------------------------

typedef struct {
    const char *label;
    data_type type;
    ast *typed_data_wrapper;
    union {
        typed_data *typed_data_int;
        typed_data *typed_data_string;
        typed_data *typed_data_symbol;
    };
} destroy_typed_data_wrapper_params_t;



//-----------------------------------------------------------------------------
// PARAM CASES
//-----------------------------------------------------------------------------


static const destroy_typed_data_wrapper_params_t destroy_typed_data_wrapper_params_template_int = {
    .label = "a typed data wrapper for an int",
    .type = TYPE_INT,
    .typed_data_wrapper = TYPED_DATA_WRAPPER_DEFINED_IN_SETUP,
    .typed_data_int = TYPED_DATA_INT_DEFINED_IN_SETUP,
};

static const destroy_typed_data_wrapper_params_t destroy_typed_data_wrapper_params_template_string = {
    .label = "a typed data wrapper for a string",
    .type = TYPE_STRING,
    .typed_data_wrapper = TYPED_DATA_WRAPPER_DEFINED_IN_SETUP,
    .typed_data_string = TYPED_DATA_STRING_DEFINED_IN_SETUP,
};

static const destroy_typed_data_wrapper_params_t destroy_typed_data_wrapper_params_template_symbol = {
    .label = "a typed data wrapper for a symbol",
    .type = TYPE_SYMBOL,
    .typed_data_wrapper = TYPED_DATA_WRAPPER_DEFINED_IN_SETUP,
    .typed_data_symbol = TYPED_DATA_SYMBOL_DEFINED_IN_SETUP,
};

static const destroy_typed_data_wrapper_params_t destroy_typed_data_wrapper_not_a_data_wrapper = {
    .label = "not a typed data wrapper",
    .type = DUMMY_DATA_TYPE,
    .typed_data_wrapper = DUMMY_DATA_WRAPPER,
};



//-----------------------------------------------------------------------------
// HELPER
//-----------------------------------------------------------------------------


static void initialize_destroy_typed_data_wrapper_params(destroy_typed_data_wrapper_params_t *params) {
    alloc_and_save_address_to_be_freed((void **)&(params->typed_data_wrapper), sizeof(ast));
    params->typed_data_wrapper->type = AST_TYPE_DATA_WRAPPER;
    switch (params->type) {
        case TYPE_INT:
            alloc_and_save_address_to_be_freed((void **)&(params->typed_data_int), sizeof(typed_data));
            params->typed_data_int->type = TYPE_INT;
            params->typed_data_int->data.int_value = DUMMY_INT;
            params->typed_data_wrapper->data = params->typed_data_int;
            break;
        case TYPE_STRING:
            alloc_and_save_address_to_be_freed((void **)&(params->typed_data_string), sizeof(typed_data));
            params->typed_data_string->type = TYPE_STRING;
            alloc_and_save_address_to_be_freed((void **)&(params->typed_data_string->data.string_value), strlen(DUMMY_STRING)+1);
            memcpy(params->typed_data_string->data.string_value, DUMMY_STRING, strlen(DUMMY_STRING)+1);
            params->typed_data_wrapper->data = params->typed_data_string;
            break;
        case TYPE_SYMBOL:
            alloc_and_save_address_to_be_freed((void **)&(params->typed_data_symbol), sizeof(typed_data));
            params->typed_data_symbol->type = TYPE_SYMBOL;
            params->typed_data_symbol->data.symbol_value = DUMMY_SYMBOL;
            params->typed_data_wrapper->data = params->typed_data_symbol;
    }
}



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_typed_data_wrapper_setup(void **state) {
    const destroy_typed_data_wrapper_params_t *model = *state;
    destroy_typed_data_wrapper_params_t *params = NULL;
    if (state == (void **)&destroy_typed_data_wrapper_not_a_data_wrapper) {
        ast *not_a_data_wrapper = NULL;
        alloc_and_save_address_to_be_freed((void*)&not_a_data_wrapper, sizeof(ast));
        not_a_data_wrapper->type = AST_TYPE_ADDITION;
        not_a_data_wrapper->children->children_nb = DUMMY_CHILDREN_NB;
        not_a_data_wrapper->children->children = DUMMY_AST_CHILDREN;
        *state = not_a_data_wrapper;
    } else {
        alloc_and_save_address_to_be_freed((void**)&params, sizeof(destroy_typed_data_wrapper_params_t));
        *params = *model;
        initialize_destroy_typed_data_wrapper_params(params);
        *state = params;
    }
    set_allocators(mock_malloc, mock_free);
    set_ast_destroy_typed_data_int(mock_ast_destroy_typed_data_int);
    set_ast_destroy_typed_data_string(mock_ast_destroy_typed_data_string);
    set_ast_destroy_typed_data_symbol(mock_ast_destroy_typed_data_symbol);
    return 0;
}

static int destroy_typed_data_wrapper_teardown(void **state) {
    set_ast_destroy_typed_data_int(NULL);
    set_ast_destroy_typed_data_string(NULL);
    set_ast_destroy_typed_data_symbol(NULL);
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: ast_data_wrapper is null
// Expected: does not any side effect
static void destroy_typed_data_wrapper_do_nothing_when_argument_null(void **state) {
    ast_destroy_typed_data_wrapper(NULL);
}

// Given: ast_data_wrapper->type is not AST_TYPE_DATA_WRAPPER
// Expected: does not any side effect
// params:
//  - destroy_typed_data_wrapper_not_a_data_wrapper
static void destroy_typed_data_wrapper_do_nothing_when_no_data_wrapper(void **state) {
    ast_destroy_typed_data_wrapper((ast*)*state);
}

// Given: ast_data_wrapper->type is AST_TYPE_DATA_WRAPPER
// Expected:
// - calls ast_destroy_typed_data_<int|string|symbol> with field data of argument of destroy_typed_data_wrapper
// - calls free with with argument of destroy_typed_data_wrapper
// params:
//  - destroy_typed_data_wrapper_params_template_int
//  - destroy_typed_data_wrapper_params_template_string
//  - destroy_typed_data_wrapper_params_template_symbol
static void destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper(void **state) {
    destroy_typed_data_wrapper_params_t *params = *state;
    ast *typed_data_wrapper = ((destroy_typed_data_wrapper_params_t*)(*state))->typed_data_wrapper;
    switch (params->type) {
        case TYPE_INT:
            expect_value(mock_ast_destroy_typed_data_int, typed_data_int, typed_data_wrapper->data);
            break;
        case TYPE_STRING:
            expect_value(mock_ast_destroy_typed_data_string, typed_data_string, typed_data_wrapper->data);
            break;
        case TYPE_SYMBOL:
            expect_value(mock_ast_destroy_typed_data_symbol, typed_data_symbol, typed_data_wrapper->data);
    }
    expect_value(mock_free, ptr, typed_data_wrapper);
    ast_destroy_typed_data_wrapper(typed_data_wrapper);
}



//-----------------------------------------------------------------------------
// ast_create_ast_children_arr TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURES
//----------------------------------------------------------------------------

typedef struct {
    const char *label;
    size_t children_nb;
    ast **ast_children;
} create_ast_children_arr_params_t;



//-----------------------------------------------------------------------------
// PARAM CASES
//-----------------------------------------------------------------------------


static const create_ast_children_arr_params_t create_ast_children_arr_params_no_child = {
    .label = "children_nb = 0",
    .children_nb = 0,
    .ast_children = DUMMY_AST_CHILDREN,
};

static const create_ast_children_arr_params_t create_ast_children_arr_params_template_one_child = {
    .label = "children_nb = 1",
    .children_nb = 1,
    .ast_children = AST_CHILDREN_DEFINED_IN_SETUP,
};

static const create_ast_children_arr_params_t create_ast_children_arr_params_template_two_children = {
    .label = "children_nb = 2",
    .children_nb = 2,
    .ast_children = AST_CHILDREN_DEFINED_IN_SETUP,
};



//-----------------------------------------------------------------------------
// HELPER
//-----------------------------------------------------------------------------


static void initialize_ast_children(create_ast_children_arr_params_t *params) {
    if (params->children_nb == 1) {
        params->ast_children = DUMMY_CHILDREN_ARRAY_OF_SIZE_ONE;
    } else if (params->children_nb == 2) {
        params->ast_children = DUMMY_CHILDREN_ARRAY_OF_SIZE_TWO;
    }
}



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_ast_children_arr_setup(void **state) {
    const create_ast_children_arr_params_t *model = *state;
    create_ast_children_arr_params_t *params = NULL;
    alloc_and_save_address_to_be_freed((void**)&params, sizeof(create_ast_children_arr_params_t));
    *params = *model;
    initialize_ast_children(params);
    *state = params;
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_ast_children_arr_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls malloc with sizeof(ast_children_t)
// params:
//  - create_ast_children_arr_params_no_child
//  - create_ast_children_arr_params_template_one_child
//  - create_ast_children_arr_params_template_two_children
static void create_ast_children_arr_calls_malloc_for_an_ast_children_t(void **state) {
    create_ast_children_arr_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    ast_create_ast_children_arr(params->children_nb, params->ast_children);
}

// Given: malloc fails
// Expected: returns null
// params:
//  - create_ast_children_arr_params_template_one_child
//  - create_ast_children_arr_params_template_two_children
static void create_ast_children_arr_return_null_when_malloc_fails(void **state) {
    create_ast_children_arr_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_ast_children_arr(params->children_nb, params->ast_children));
}

// Given: malloc succeeds
// Expected: the malloc'ed ast_children_t is initialized and returned
// params:
//  - create_ast_children_arr_params_template_one_child
//  - create_ast_children_arr_params_template_two_children
static void create_ast_children_arr_initializes_and_returns_malloced_ast_children_t_when_malloc_succeds(void **state) {
    create_ast_children_arr_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast_children_t, sizeof(ast_children_t));
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast_children_t);
    ast_children_t *ret = ast_create_ast_children_arr(params->children_nb, params->ast_children);
    assert_int_equal(ret->children_nb, params->children_nb);
    assert_ptr_equal(ret->children, params->ast_children);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast_children_t);
}



//-----------------------------------------------------------------------------
// ast_create_ast_children_var TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURES
//----------------------------------------------------------------------------

#define create_ast_children_var_params_t create_ast_children_arr_params_t



//-----------------------------------------------------------------------------
// PARAM CASES
//-----------------------------------------------------------------------------


static const create_ast_children_var_params_t create_ast_children_var_params_no_child = create_ast_children_arr_params_no_child;
static const create_ast_children_var_params_t create_ast_children_var_params_template_one_child = create_ast_children_arr_params_template_one_child;
static const create_ast_children_var_params_t create_ast_children_var_params_template_two_children = create_ast_children_arr_params_template_two_children;



//-----------------------------------------------------------------------------
// HELPER
//-----------------------------------------------------------------------------


// use initialize_ast_children define in "ast_create_ast_children_arr TESTS" section



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


#define create_ast_children_var_setup create_ast_children_arr_setup
#define create_ast_children_var_teardown create_ast_children_arr_teardown



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls malloc with sizeof(ast_children_t)
// params:
//  - create_ast_children_var_params_no_child
//  - create_ast_children_var_params_template_one_child
//  - create_ast_children_var_params_template_two_children
static void create_ast_children_var_calls_malloc_for_an_ast_children_t(void **state) {
    create_ast_children_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid mock call
    if (params->children_nb == 0)
        ast_create_ast_children_var(params->children_nb);
    if (params->children_nb == 1)
        ast_create_ast_children_var(params->children_nb, (params->ast_children)[0]);
    if (params->children_nb == 2)
        ast_create_ast_children_var(params->children_nb, (params->ast_children)[0], (params->ast_children)[1]);
}

// Given: malloc fails
// Expected: returns null
// params:
//  - create_ast_children_var_params_template_one_child
//  - create_ast_children_var_params_template_two_children
static void create_ast_children_var_return_null_when_malloc_fails(void **state) {
    create_ast_children_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    ast_children_t *ret = NULL;
    if (params->children_nb == 1)
        ret = ast_create_ast_children_var(params->children_nb, (params->ast_children)[0]);
    if (params->children_nb == 2)
        ret = ast_create_ast_children_var(params->children_nb, (params->ast_children)[0], (params->ast_children)[1]);
    assert_null(ret);
}

// Given: malloc for an ast_children_t succeeds
// Expected: calls malloc with children_nb * sizeof(ast *)
// params:
//  - create_ast_children_var_params_template_one_child
//  - create_ast_children_var_params_template_two_children
static void create_ast_children_var_calls_malloc_for_a_double_pointer_of_ast_when_malloc_for_an_ast_children_t_succeeds(void **state) {
    create_ast_children_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    if (params->children_nb == 1)
        ast_create_ast_children_var(params->children_nb, (params->ast_children)[0]);
    if (params->children_nb == 2)
        ast_create_ast_children_var(params->children_nb, (params->ast_children)[0], (params->ast_children)[1]);
}

// Given: malloc for a double pointer of ast fails
// Expected: calls free with pointer returned by malloc for an ast_children_t and returns null
// params:
//  - create_ast_children_var_params_template_one_child
//  - create_ast_children_var_params_template_two_children
static void create_ast_children_var_calls_free_with_pointer_for_ast_children_t_when_malloc_for_double_pointer_of_ast_fails(void **state) {
    create_ast_children_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast_children_t, sizeof(ast_children_t));
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast_children_t);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_an_ast_children_t);
    ast_children_t *ret = NULL;
    if (params->children_nb == 1)
        ret = ast_create_ast_children_var(params->children_nb, (params->ast_children)[0]);
    if (params->children_nb == 2)
        ret = ast_create_ast_children_var(params->children_nb, (params->ast_children)[0], (params->ast_children)[1]);
    assert_null(ret);
}

// Given: malloc for a double pointer of ast succeeds, children_nb = 1
// Expected: the malloc'ed ast_children_t is initialized and returned
// params:
//  - create_ast_children_var_params_template_one_child
static void create_ast_children_var_initializes_and_returns_malloced_ast_children_t_when_malloc_for_double_pointer_of_ast_succeeds_and_one_arg(void **state) {
    create_ast_children_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast_children_t, sizeof(ast_children_t));
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_double_ast_pointer, params->children_nb * sizeof(ast *));
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast_children_t);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_double_ast_pointer);
    ast_children_t *ret = ast_create_ast_children_var(params->children_nb, (params->ast_children)[0]);
    assert_int_equal(ret->children_nb, params->children_nb);
    assert_ptr_equal(ret->children[0], params->ast_children[0]);
    assert_ptr_not_equal(ret->children, params->ast_children); // double pointer of ast has been malloced
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast_children_t);
}

// Given: malloc for a double pointer of ast succeeds, children_nb = 2
// Expected: the malloc'ed ast_children_t is initialized and returned
// params:
//  - create_ast_children_var_params_template_two_child
static void create_ast_children_var_initializes_and_returns_malloced_ast_children_t_when_malloc_for_double_pointer_of_ast_succeeds_and_two_args(void **state) {
    create_ast_children_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast_children_t, sizeof(ast_children_t));
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_double_ast_pointer, params->children_nb * sizeof(ast *));
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast_children_t);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_double_ast_pointer);
    ast_children_t *ret = ast_create_ast_children_var(params->children_nb, (params->ast_children)[0], (params->ast_children)[1]);
    assert_int_equal(ret->children_nb, params->children_nb);
    assert_ptr_equal(ret->children[0], params->ast_children[0]);
    assert_ptr_equal(ret->children[1], params->ast_children[1]);
    assert_ptr_not_equal(ret->children, params->ast_children); // double pointer of ast has been malloced
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast_children_t);
}



//-----------------------------------------------------------------------------
// ast_destroy_ast_children TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURES
//----------------------------------------------------------------------------

typedef struct {
    const char *label;
    size_t children_nb;
    ast_children_t *ast_children;
} destroy_ast_children_params_t;



//-----------------------------------------------------------------------------
// PARAM CASES
//-----------------------------------------------------------------------------


static const destroy_ast_children_params_t destroy_ast_children_params_null = {
    .label = "ast_children is null",
    .children_nb = DUMMY_INT,
    .ast_children = NULL,
};

static const destroy_ast_children_params_t destroy_ast_children_params_template_no_child = {
    .label = "children_nb = 0",
    .children_nb = 0,
    .ast_children = DUMMY_AST_CHILDREN_T_P,
};

static const destroy_ast_children_params_t destroy_ast_children_params_template_one_child = {
    .label = "children_nb = 1, the child is a AST_TYPE_DATA_WRAPPER",
    .children_nb = 1,
    .ast_children = AST_CHILDREN_T_DEFINED_IN_SETUP,
};

static const destroy_ast_children_params_t destroy_ast_children_params_template_two_child = {
    .label = "children_nb = 2, first child is a AST_TYPE_DATA_WRAPPER but not the second",
    .children_nb = 2,
    .ast_children = AST_CHILDREN_T_DEFINED_IN_SETUP,
};



//-----------------------------------------------------------------------------
// HELPER
//-----------------------------------------------------------------------------


static void initialize_ast_children_dynamically(destroy_ast_children_params_t *params) {
    if (params->children_nb == 0) {
        alloc_and_save_address_to_be_freed((void**)&(params->ast_children), sizeof(ast_children_t));
        params->ast_children->children_nb = 0;
        params->ast_children->children = DUMMY_AST_CHILDREN;
    }
    if (params->children_nb == 1) {
        alloc_and_save_address_to_be_freed((void**)&params->ast_children, sizeof(ast_children_t));
        params->ast_children->children_nb = 1;
        alloc_and_save_address_to_be_freed((void**)&(params->ast_children->children), 1 * sizeof(ast *));
        alloc_and_save_address_to_be_freed((void**)&((params->ast_children->children)[0]), sizeof(ast));
        ((params->ast_children->children)[0])->type = AST_TYPE_DATA_WRAPPER;
        ((params->ast_children->children)[0])->data = DUMMY_TYPED_DATA_P;
    }
    if (params->children_nb == 2) {
        alloc_and_save_address_to_be_freed((void**)&params->ast_children, sizeof(ast_children_t));
        params->ast_children->children_nb = 2;
        alloc_and_save_address_to_be_freed((void**)&(params->ast_children->children), 2 * sizeof(ast *));
        alloc_and_save_address_to_be_freed((void**)&((params->ast_children->children)[0]), sizeof(ast));
        alloc_and_save_address_to_be_freed((void**)&((params->ast_children->children)[1]), sizeof(ast));
        ((params->ast_children->children)[0])->type = AST_TYPE_DATA_WRAPPER;
        ((params->ast_children->children)[1])->type = AST_TYPE_COMPUTATION;
        ((params->ast_children->children)[0])->data = DUMMY_TYPED_DATA_P;
        ((params->ast_children->children)[1])->children = DUMMY_AST_CHILDREN_T_P;
    }
    // note: do nothing when params->children_nb == DUMMY_INT because DUMMY_INT = 7
}



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_ast_children_setup(void **state) {
    const destroy_ast_children_params_t *model = *state;
    destroy_ast_children_params_t *params = NULL;
    alloc_and_save_address_to_be_freed((void**)&params, sizeof(destroy_ast_children_params_t));
    *params = *model;
    initialize_ast_children_dynamically(params);
    *state = params;
    set_allocators(mock_malloc, mock_free);
    set_ast_destroy_typed_data_wrapper(mock_ast_destroy_typed_data_wrapper);
    set_ast_destroy_non_typed_data_wrapper(mock_ast_destroy_non_typed_data_wrapper);
    return 0;
}

static int destroy_ast_children_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_ast_destroy_typed_data_wrapper(NULL);
    set_ast_destroy_non_typed_data_wrapper(NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------

// Given: ast_children is null
// Expected: does not any side effect
// params: destroy_ast_children_params_null
static void destroy_ast_children_do_nothing_when_argument_null(void **state) {
    destroy_ast_children_params_t *params = *state;
    ast_destroy_ast_children(params->ast_children);
}

// Given: children_nb = 0
// Expected: does not any side effect
// params: destroy_ast_children_params_template_no_child
static void destroy_ast_children_do_nothing_when_no_child(void **state) {
    destroy_ast_children_params_t *params = *state;
    ast_destroy_ast_children(params->ast_children);
}

// Given: children_nb = 1, the child is a typed data wrapper
// Expected:
//  - calls ast_destroy_typed_data_wrapper with ast_children->children[0]
//  - calls free with ast_children->children
//  - calls free with ast_children
// params: destroy_ast_children_params_template_one_child
static void destroy_ast_children_calls_destroy_properly_when_one_child_which_is_data_wrapper(void **state) {
    destroy_ast_children_params_t *params = *state;
    expect_value(mock_ast_destroy_typed_data_wrapper, ast_data_wrapper, params->ast_children->children[0]);
    expect_value(mock_free, ptr, params->ast_children->children);
    expect_value(mock_free, ptr, params->ast_children);
    ast_destroy_ast_children(params->ast_children);
}

// Given: children_nb = 2, first child is a typed data wrapper but not the second
// Expected:
//  - calls ast_destroy_typed_data_wrapper with ast_children->children[0]
//  - calls ast_destroy_non_typed_data_wrapper with ast_children->children[1]
//  - calls free with ast_children->children
//  - calls free with ast_children
// params: destroy_ast_children_params_template_two_child
static void destroy_ast_children_calls_destroy_properly_when_two_children_one_data_wrapper_other_not(void **state) {
    destroy_ast_children_params_t *params = *state;
    expect_value(mock_ast_destroy_typed_data_wrapper, ast_data_wrapper, params->ast_children->children[0]);
    expect_value(mock_ast_destroy_non_typed_data_wrapper, non_typed_data_wrapper, params->ast_children->children[1]);
    expect_value(mock_free, ptr, params->ast_children->children);
    expect_value(mock_free, ptr, params->ast_children);
    ast_destroy_ast_children(params->ast_children);
}



//-----------------------------------------------------------------------------
// ast_create_non_typed_data_wrapper TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_non_typed_data_wrapper_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_non_typed_data_wrapper_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: type is AST_TYPE_DATA_WRAPPER or not supported
// Expected: returns null
static void create_non_typed_data_wrapper_returns_null_when_type_data_wrapper_or_unsupported(void **state) {
    assert_null(ast_create_non_typed_data_wrapper(AST_TYPE_DATA_WRAPPER, DUMMY_AST_CHILDREN_T_P));
    assert_null(ast_create_non_typed_data_wrapper(UNSUPPORTED_AST_TYPE, DUMMY_AST_CHILDREN_T_P));
}

// Given: type is supported and not AST_TYPE_DATA_WRAPPER, ast_children is null
// Expected: returns null
static void create_non_typed_data_wrapper_returns_null_when_ast_children_null(void **state) {
    assert_null(ast_create_non_typed_data_wrapper(AST_TYPE_PROGRAM, NULL));
}

// Given: type is supported, ast_children is not null
// Expected: calls malloc with sizeof(ast)
static void create_non_typed_data_wrapper_calls_malloc_for_an_ast_when_args_valid(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    ast_create_non_typed_data_wrapper(AST_TYPE_PROGRAM, DUMMY_AST_CHILDREN_T_P);
}

// Given: malloc fails
// Expected: returns null
static void create_non_typed_data_wrapper_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_non_typed_data_wrapper(AST_TYPE_PROGRAM, DUMMY_AST_CHILDREN_T_P));
}

// Given: malloc succeeds
// Expected: the malloc'ed ast is initialized and returned
static void create_non_typed_data_wrapper_initializes_and_returns_malloced_ast_when_malloc_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    ast *ret = ast_create_non_typed_data_wrapper(AST_TYPE_PROGRAM, DUMMY_AST_CHILDREN_T_P);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, AST_TYPE_PROGRAM);
    assert_ptr_equal(ret->children, DUMMY_AST_CHILDREN_T_P);
}



//-----------------------------------------------------------------------------
// ast_create_non_typed_data_wrapper_arr TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_non_typed_data_wrapper_arr_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    set_ast_create_ast_children_arr(mock_ast_create_ast_children_arr);
    return 0;
}

static int create_non_typed_data_wrapper_arr_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_ast_create_ast_children_arr(NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: type is AST_TYPE_DATA_WRAPPER or not supported
// Expected: returns null
static void create_non_typed_data_wrapper_arr_returns_null_when_type_data_wrapper_or_unsupported(void **state) {
    assert_null(ast_create_non_typed_data_wrapper_arr(AST_TYPE_DATA_WRAPPER, DUMMY_INT, DUMMY_AST_CHILDREN));
    assert_null(ast_create_non_typed_data_wrapper_arr(UNSUPPORTED_AST_TYPE, DUMMY_INT, DUMMY_AST_CHILDREN));
}

// Given: type is supported and not AST_TYPE_DATA_WRAPPER
// Expected: calls ast_create_ast_children_arr with children_nb and children
static void create_non_typed_data_wrapper_arr_calls_ast_create_ast_children_arr_with_last_args_when_type_supported_and_not_data_wrapper(void **state) {
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, NULL); // to avoid more mock calls
    ast_create_non_typed_data_wrapper_arr(AST_TYPE_PROGRAM, DUMMY_INT, DUMMY_AST_CHILDREN);
}

// Given: ast_create_ast_children_arr fails (ie returns NULL)
// Expected: returns NULL
static void create_non_typed_data_wrapper_arr_returns_null_when_ast_create_ast_children_arr_fails(void **state) {
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, NULL);
    assert_null(ast_create_non_typed_data_wrapper_arr(AST_TYPE_PROGRAM, DUMMY_INT, DUMMY_AST_CHILDREN));
}

// Given: ast_create_ast_children_arr succeeds (ie does not return NULL)
// Expected: calls malloc with sizeof(ast)
static void create_non_typed_data_wrapper_arr_calls_malloc_for_an_ast_when_ast_create_ast_children_arr_succeeds(void **state) {
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    ast_create_non_typed_data_wrapper_arr(AST_TYPE_PROGRAM, DUMMY_INT, DUMMY_AST_CHILDREN);
}

// Given: calls malloc with sizeof(ast) fails
// Expected: calls free with pointer returned by ast_create_ast_children_arr and returns null
static void create_non_typed_data_wrapper_arr_calls_free_and_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_AST_CHILDREN_T_P);
    assert_null(ast_create_non_typed_data_wrapper_arr(AST_TYPE_PROGRAM, DUMMY_INT, DUMMY_AST_CHILDREN));
}

// Given: calls malloc with sizeof(ast) succeeds
// Expected: the malloc'ed ast is initialized and returned
static void create_non_typed_data_wrapper_arr_initializes_and_returns_malloced_ast_when_malloc_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_ast_create_ast_children_arr_returned_value, sizeof(ast_children_t));
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, fake_ast_create_ast_children_arr_returned_value);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    ast * ret = ast_create_non_typed_data_wrapper_arr(AST_TYPE_PROGRAM, DUMMY_INT, DUMMY_AST_CHILDREN);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, AST_TYPE_PROGRAM);
    assert_ptr_equal(ret->children, fake_ast_create_ast_children_arr_returned_value);
}



//-----------------------------------------------------------------------------
// ast_create_non_typed_data_wrapper_var TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURES
//----------------------------------------------------------------------------

#define create_non_typed_data_wrapper_var_params_t create_ast_children_arr_params_t



//-----------------------------------------------------------------------------
// PARAM CASES
//-----------------------------------------------------------------------------


static const create_non_typed_data_wrapper_var_params_t create_non_typed_data_wrapper_var_params_no_child = create_ast_children_arr_params_no_child;
static const create_non_typed_data_wrapper_var_params_t create_non_typed_data_wrapper_var_params_template_one_child = create_ast_children_arr_params_template_one_child;
static const create_non_typed_data_wrapper_var_params_t create_non_typed_data_wrapper_var_params_template_two_children = create_ast_children_arr_params_template_two_children;



//-----------------------------------------------------------------------------
// HELPER
//-----------------------------------------------------------------------------


// use initialize_ast_children define in "ast_create_ast_children_arr TESTS" section




//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_non_typed_data_wrapper_var_setup(void **state) {
    const create_non_typed_data_wrapper_var_params_t *model = *state;
    create_non_typed_data_wrapper_var_params_t *params = NULL;
    alloc_and_save_address_to_be_freed((void**)&params, sizeof(create_non_typed_data_wrapper_var_params_t));
    *params = *model;
    initialize_ast_children(params);
    *state = params;
    set_allocators(mock_malloc, mock_free);
    set_ast_create_ast_children_arr(mock_ast_create_ast_children_arr);
    return 0;
}

static int create_non_typed_data_wrapper_var_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_ast_create_ast_children_arr(NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: type is AST_TYPE_DATA_WRAPPER or not supported
// Expected: returns null
static void create_non_typed_data_wrapper_var_returns_null_when_type_data_wrapper_or_unsupported(void **state) {
    assert_null(ast_create_non_typed_data_wrapper_var(AST_TYPE_DATA_WRAPPER, DUMMY_INT, DUMMY_AST_CHILDREN));
    assert_null(ast_create_non_typed_data_wrapper_var(UNSUPPORTED_AST_TYPE, DUMMY_INT, DUMMY_AST_CHILDREN));
}

// Given: type is supported and not AST_TYPE_DATA_WRAPPER, children_nb = 0, 1 or 2
// Expected:
//  - calls malloc with sizeof(ast)
// params:
//  - create_non_typed_data_wrapper_var_params_no_child
//  - create_non_typed_data_wrapper_var_params_template_one_child;
//  - create_non_typed_data_wrapper_var_params_template_two_children
static void create_non_typed_data_wrapper_var_calls_malloc_for_an_ast_when_type_supported_and_not_data_wrapper(void **state) {
    create_non_typed_data_wrapper_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid more mock calls
    if (params->children_nb == 0) {
        ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            0 );
    }
    if (params->children_nb == 1) {
        ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0] );
    }
    if (params->children_nb == 2) {
        ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1] );
    }
}

// Given: malloc for an ast fails
// Expected: returns null
// params:
//  - create_non_typed_data_wrapper_var_params_no_child
//  - create_non_typed_data_wrapper_var_params_template_one_child
//  - create_non_typed_data_wrapper_var_params_template_two_children
static void create_non_typed_data_wrapper_var_returns_null_when_malloc_for_an_ast_fails(void **state) {
    create_non_typed_data_wrapper_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    ast *ret = NULL;
    if (params->children_nb == 0) {
        ret = ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            0 );
    }
    if (params->children_nb == 1) {
        ret = ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0] );
    }
    if (params->children_nb == 2) {
       ret =  ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1] );
    }
    assert_null(ret);
}

// Given: malloc for an ast succeeds, children_nb = 0
// Expected: calls ast_create_ast_children_arr with 0 and NULL (and then get an ast_children_t pointing to {0, NULL})
// params: create_non_typed_data_wrapper_var_params_no_child
static void create_non_typed_data_wrapper_var_calls_ast_create_ast_children_arr_when_malloc_for_an_ast_succeeds_and_children_nb_0(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_ast_create_ast_children_arr, children_nb, 0);
    expect_value(mock_ast_create_ast_children_arr, children, NULL);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    ast_create_non_typed_data_wrapper_var(
        SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
        0 );
}

// Given: calls ast_create_ast_children_arr with 0 and NULL fails
// Expected: calls free with pointer returned by malloc for an ast and return NULL
// params: create_non_typed_data_wrapper_var_params_no_child
static void create_non_typed_data_wrapper_var_calls_free_and_returns_null_when_ast_create_ast_children_arr_fails_and_children_nb_0(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_ast_create_ast_children_arr, children_nb, 0);
    expect_value(mock_ast_create_ast_children_arr, children, NULL);
    will_return(mock_ast_create_ast_children_arr, NULL);
    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE);
    assert_null(
        ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            0 ) );
}

// Given: calls ast_create_ast_children_arr with 0 and NULL succeeds
// Expected: the malloc'ed ast is initialized and returned
// params: create_non_typed_data_wrapper_var_params_no_child
static void create_non_typed_data_wrapper_var_initializes_and_returns_malloced_ast_when_ast_create_ast_children_arr_succeeds_and_children_nb_0(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_ast_create_ast_children_arr, children_nb, 0);
    expect_value(mock_ast_create_ast_children_arr, children, NULL);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    ast *ret =
        ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            0 );
    assert_ptr_equal(ret, DUMMY_MALLOC_RETURNED_VALUE);
    assert_int_equal(ret->type, SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->children, DUMMY_AST_CHILDREN_T_P);
}

// Given: malloc for an ast succeeds, children_nb = 1
// Expected: calls malloc with children_nb * sizeof() for an array of ast pointer
// params:
//  - create_non_typed_data_wrapper_var_params_template_one_child
//  - create_non_typed_data_wrapper_var_params_template_two_children
static void create_non_typed_data_wrapper_var_calls_malloc_for_an_array_of_pointer_of_ast_when_malloc_for_an_ast_succeeds_and_children_nb_not_0(void **state) {
    create_non_typed_data_wrapper_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid more mock calls
    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE); // to avoid more mock calls
    if (params->children_nb == 1)
        ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0]);
    if (params->children_nb == 2)
        ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1] );
}

// Given: malloc for an array of ast pointer fails
// Expected: calls free with pointer returned by malloc for an ast and return NULL
// params:
//  - create_non_typed_data_wrapper_var_params_template_one_child
//  - create_non_typed_data_wrapper_var_params_template_two_children
static void create_non_typed_data_wrapper_var_calls_free_and_returns_null_when_malloc_for_an_array_of_pointer_of_ast_fails(void **state) {
    create_non_typed_data_wrapper_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE);
    ast *ret = NULL;
    if (params->children_nb == 1)
        ret = ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0]);
    if (params->children_nb == 2)
        ret = ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1] );
    assert_null(ret);
}

// Given: malloc for an array of ast pointer succeeds
// Expected:
//  - initializes malloc'ed array of ast pointer with variadic arguments
//  - calls ast_create_ast_children_arr with children_nb and the array of ast pointer
// params:
//  - create_non_typed_data_wrapper_var_params_template_one_child
//  - create_non_typed_data_wrapper_var_params_template_two_children
static void create_non_typed_data_wrapper_var_initializes_ast_p_array_and_calls_ast_create_ast_children_arr_when_malloc_for_ast_p_array_succeeds(void **state) {
    create_non_typed_data_wrapper_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast, sizeof(ast));
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_double_ast_pointer, params->children_nb * sizeof(ast *));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_double_ast_pointer);
    expect_value(mock_ast_create_ast_children_arr, children_nb, params->children_nb);
    expect_value(mock_ast_create_ast_children_arr, children, fake_malloc_returned_value_for_a_double_ast_pointer);
    will_return(mock_ast_create_ast_children_arr, NULL); // to avoid more mock calls
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_a_double_ast_pointer); // to avoid more mock calls
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_an_ast); // to avoid more mock calls
    if (params->children_nb == 1) {
        ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0] );
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **)fake_malloc_returned_value_for_a_double_ast_pointer)[0] );
    }
    if (params->children_nb == 2) {
        ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1] );
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **)fake_malloc_returned_value_for_a_double_ast_pointer)[0] );
        assert_ptr_equal(
            (params->ast_children)[1],
            ((ast **)fake_malloc_returned_value_for_a_double_ast_pointer)[1] );
    }
}

// Given: ast_create_ast_children_arr with children_nb and the array of ast pointer fails
// Expected:
//  - calls free with the array of ast pointer
//  - calls free with the pointer for an ast
//  - returns NULL
// params:
//  - create_non_typed_data_wrapper_var_params_template_one_child
//  - create_non_typed_data_wrapper_var_params_template_two_children
static void create_non_typed_data_wrapper_var_frees_two_malloced_pointers_when_ast_create_ast_children_arr_fails(void **state) {
    create_non_typed_data_wrapper_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast, sizeof(ast));
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_double_ast_pointer, params->children_nb * sizeof(ast *));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_double_ast_pointer);
    expect_value(mock_ast_create_ast_children_arr, children_nb, params->children_nb);
    expect_value(mock_ast_create_ast_children_arr, children, fake_malloc_returned_value_for_a_double_ast_pointer);
    will_return(mock_ast_create_ast_children_arr, NULL); // the call fails
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_a_double_ast_pointer);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_an_ast);
    ast *ret = NULL;
    if (params->children_nb == 1) {
        ret = ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0] );
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **)fake_malloc_returned_value_for_a_double_ast_pointer)[0] );
    }
    if (params->children_nb == 2) {
        ret = ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1] );
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **)fake_malloc_returned_value_for_a_double_ast_pointer)[0] );
        assert_ptr_equal(
            (params->ast_children)[1],
            ((ast **)fake_malloc_returned_value_for_a_double_ast_pointer)[1] );
    }
    assert_null(ret);
}

// Given: ast_create_ast_children_arr with children_nb and the array of ast pointer succeeds
// Expected:
//  - initializes the malloc'ed ast
//  - calls free with the array of ast pointer
//  - returns the malloc'ed ast
// params:
//  - create_non_typed_data_wrapper_var_params_template_one_child
//  - create_non_typed_data_wrapper_var_params_template_two_children
static void create_non_typed_data_wrapper_var_initializes_and_returns_malloced_ast_and_frees_ast_p_arr_when_ast_create_ast_children_arr_succeeds(void **state) {
    create_non_typed_data_wrapper_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast, sizeof(ast));
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_double_ast_pointer, params->children_nb * sizeof(ast *));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_double_ast_pointer);
    expect_value(mock_ast_create_ast_children_arr, children_nb, params->children_nb);
    expect_value(mock_ast_create_ast_children_arr, children, fake_malloc_returned_value_for_a_double_ast_pointer);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_a_double_ast_pointer);
    ast *ret = NULL;
    if (params->children_nb == 1) {
        ret = ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0] );
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **)fake_malloc_returned_value_for_a_double_ast_pointer)[0] );
    }
    if (params->children_nb == 2) {
        ret = ast_create_non_typed_data_wrapper_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1] );
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **)fake_malloc_returned_value_for_a_double_ast_pointer)[0] );
        assert_ptr_equal(
            (params->ast_children)[1],
            ((ast **)fake_malloc_returned_value_for_a_double_ast_pointer)[1] );
    }
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->children, DUMMY_AST_CHILDREN_T_P);
}



//-----------------------------------------------------------------------------
// ast_destroy_non_typed_data_wrapper TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_non_typed_data_wrapper_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **)&ast_not_data_wrapper, sizeof(ast));
    set_allocators(mock_malloc, mock_free);
    set_ast_destroy_ast_children(mock_ast_destroy_ast_children);
    return 0;
}

static int destroy_non_typed_data_wrapper_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_ast_destroy_ast_children(NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: non_typed_data_wrapper = NULL
// Expected: do nothing
static void destroy_non_typed_data_wrapper_do_nothing_when_arg_null(void **state) {
    ast_destroy_non_typed_data_wrapper(NULL);
}

// Given: non_typed_data_wrapper != NULL, non_typed_data_wrapper->type == AST_TYPE_DATA_WRAPPER
// Expected: do nothing
static void destroy_non_typed_data_wrapper_do_nothing_when_arg_is_typed_data_wrapper(void **state) {
    ast_not_data_wrapper->type = AST_TYPE_DATA_WRAPPER;
    ast_not_data_wrapper->children = DUMMY_AST_CHILDREN_T_P;
    ast_destroy_non_typed_data_wrapper(ast_not_data_wrapper);
}

// Given: non_typed_data_wrapper->type is not AST_TYPE_DATA_WRAPPER (and can even be not supported)
// Expected:
//  - calls ast_destroy_ast_children with non_typed_data_wrapper->children
//  - calls free with non_typed_data_wrapper
static void destroy_non_typed_data_wrapper_calls_ast_destroy_ast_children_and_calls_free_when_arg_is_valid(void **state) {
    ast_not_data_wrapper->type = AST_TYPE_ADDITION;
    ast_not_data_wrapper->children = DUMMY_AST_CHILDREN_T_P;
    expect_value(mock_ast_destroy_ast_children, ast_children, DUMMY_AST_CHILDREN_T_P);
    expect_value(mock_free, ptr, ast_not_data_wrapper);
    ast_destroy_non_typed_data_wrapper(ast_not_data_wrapper);

    ast_not_data_wrapper->type = UNSUPPORTED_AST_TYPE;
    expect_value(mock_ast_destroy_ast_children, ast_children, DUMMY_AST_CHILDREN_T_P);
    expect_value(mock_free, ptr, ast_not_data_wrapper);
    ast_destroy_non_typed_data_wrapper(ast_not_data_wrapper);
}



//-----------------------------------------------------------------------------
// ast_create TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **)&ast_not_data_wrapper, sizeof(ast));
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
// Expected:
static void create_(void **state) {
    ast_create();
}





//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest create_typed_data_int_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_typed_data_int_calls_malloc_for_a_typed_data,
            create_typed_data_int_setup, create_typed_data_int_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_int_returns_null_when_malloc_fails,
            create_typed_data_int_setup, create_typed_data_int_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_int_initializes_and_returns_malloced_typed_data_when_malloc_succeds,
            create_typed_data_int_setup, create_typed_data_int_teardown),
    };

    const struct CMUnitTest ast_destroy_typed_data_int_tests[] = {
        cmocka_unit_test_setup_teardown(
            destroy_typed_data_int_calls_free,
            destroy_typed_data_int_setup, destroy_typed_data_int_teardown),
    };

    const struct CMUnitTest ast_create_typed_data_string_tests[] = {
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_calls_malloc_for_a_typed_data,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_returns_null_when_malloc_fails,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_calls_strdup_when_malloc_succeds,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_calls_free_and_returns_null_when_strdup_fails,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_initializes_and_returns_malloced_typed_data_when_strdup_succeds,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
    };

    const struct CMUnitTest ast_destroy_typed_data_string_tests[] = {
        cmocka_unit_test_setup_teardown(
            destroy_typed_data_string_calls_free_for_string_value_field_then_for_typed_data_string,
            destroy_typed_data_string_setup, destroy_typed_data_string_teardown),
    };

    const struct CMUnitTest ast_create_typed_data_symbol_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_typed_data_symbol_calls_malloc_for_a_typed_data,
            create_typed_data_symbol_setup, create_typed_data_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_symbol_returns_null_when_malloc_fails,
            create_typed_data_symbol_setup, create_typed_data_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_symbol_initializes_and_returns_malloced_typed_data_when_malloc_succeds,
            create_typed_data_symbol_setup, create_typed_data_symbol_teardown),
    };

    const struct CMUnitTest ast_destroy_typed_data_symbol_tests[] = {
        cmocka_unit_test_setup_teardown(
            destroy_typed_data_symbol_calls_free,
            destroy_typed_data_string_setup, destroy_typed_data_string_teardown),
    };

    const struct CMUnitTest ast_create_typed_data_wrapper_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_typed_data_wrapper_calls_malloc_for_an_ast,
            create_typed_data_wrapper_setup, create_typed_data_wrapper_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_wrapper_returns_null_when_malloc_fails,
            create_typed_data_wrapper_setup, create_typed_data_wrapper_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_wrapper_initializes_and_returns_malloced_ast_when_malloc_succeds,
            create_typed_data_wrapper_setup, create_typed_data_wrapper_teardown),
    };

    const struct CMUnitTest ast_destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper_tests[] = {
        cmocka_unit_test(destroy_typed_data_wrapper_do_nothing_when_argument_null),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_typed_data_wrapper_do_nothing_when_no_data_wrapper,
            destroy_typed_data_wrapper_setup, destroy_typed_data_wrapper_teardown, (void *)&destroy_typed_data_wrapper_not_a_data_wrapper),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper,
            destroy_typed_data_wrapper_setup, destroy_typed_data_wrapper_teardown, (void *)&destroy_typed_data_wrapper_params_template_int),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper,
            destroy_typed_data_wrapper_setup, destroy_typed_data_wrapper_teardown, (void *)&destroy_typed_data_wrapper_params_template_string),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper,
            destroy_typed_data_wrapper_setup, destroy_typed_data_wrapper_teardown, (void *)&destroy_typed_data_wrapper_params_template_symbol),
    };

    const struct CMUnitTest ast_create_ast_children_arr_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_calls_malloc_for_an_ast_children_t,
            create_ast_children_arr_setup, create_ast_children_arr_teardown, (void *)&create_ast_children_arr_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_calls_malloc_for_an_ast_children_t,
            create_ast_children_arr_setup, create_ast_children_arr_teardown, (void *)&create_ast_children_arr_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_calls_malloc_for_an_ast_children_t,
            create_ast_children_arr_setup, create_ast_children_arr_teardown, (void *)&create_ast_children_arr_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_return_null_when_malloc_fails,
            create_ast_children_arr_setup, create_ast_children_arr_teardown, (void *)&create_ast_children_arr_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_return_null_when_malloc_fails,
            create_ast_children_arr_setup, create_ast_children_arr_teardown, (void *)&create_ast_children_arr_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_initializes_and_returns_malloced_ast_children_t_when_malloc_succeds,
            create_ast_children_arr_setup, create_ast_children_arr_teardown, (void *)&create_ast_children_arr_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_initializes_and_returns_malloced_ast_children_t_when_malloc_succeds,
            create_ast_children_arr_setup, create_ast_children_arr_teardown, (void *)&create_ast_children_arr_params_template_two_children),
    };

    const struct CMUnitTest ast_create_ast_children_var_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_an_ast_children_t,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_an_ast_children_t,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_an_ast_children_t,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_return_null_when_malloc_fails,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_return_null_when_malloc_fails,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_a_double_pointer_of_ast_when_malloc_for_an_ast_children_t_succeeds,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_a_double_pointer_of_ast_when_malloc_for_an_ast_children_t_succeeds,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_free_with_pointer_for_ast_children_t_when_malloc_for_double_pointer_of_ast_fails,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_free_with_pointer_for_ast_children_t_when_malloc_for_double_pointer_of_ast_fails,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_initializes_and_returns_malloced_ast_children_t_when_malloc_for_double_pointer_of_ast_succeeds_and_one_arg,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_initializes_and_returns_malloced_ast_children_t_when_malloc_for_double_pointer_of_ast_succeeds_and_two_args,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_two_children),
    };

    const struct CMUnitTest ast_destroy_ast_children_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            destroy_ast_children_do_nothing_when_argument_null,
            destroy_ast_children_setup, destroy_ast_children_teardown, (void *)&destroy_ast_children_params_null),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_ast_children_do_nothing_when_no_child,
            destroy_ast_children_setup, destroy_ast_children_teardown, (void *)&destroy_ast_children_params_template_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_ast_children_calls_destroy_properly_when_one_child_which_is_data_wrapper,
            destroy_ast_children_setup, destroy_ast_children_teardown, (void *)&destroy_ast_children_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_ast_children_calls_destroy_properly_when_two_children_one_data_wrapper_other_not,
            destroy_ast_children_setup, destroy_ast_children_teardown, (void *)&destroy_ast_children_params_template_two_child),
    };

    const struct CMUnitTest ast_create_non_typed_data_wrapper_tests[] = {
        cmocka_unit_test(create_non_typed_data_wrapper_returns_null_when_type_data_wrapper_or_unsupported),
        cmocka_unit_test(create_non_typed_data_wrapper_returns_null_when_ast_children_null),
        cmocka_unit_test_setup_teardown(
            create_non_typed_data_wrapper_returns_null_when_malloc_fails,
            create_non_typed_data_wrapper_setup, create_non_typed_data_wrapper_teardown),
        cmocka_unit_test_setup_teardown(
            create_non_typed_data_wrapper_initializes_and_returns_malloced_ast_when_malloc_succeeds,
            create_non_typed_data_wrapper_setup, create_non_typed_data_wrapper_teardown),
    };

    const struct CMUnitTest ast_create_non_typed_data_wrapper_arr_tests[] = {
        cmocka_unit_test(create_non_typed_data_wrapper_arr_returns_null_when_type_data_wrapper_or_unsupported),
        cmocka_unit_test_setup_teardown(
            create_non_typed_data_wrapper_arr_calls_ast_create_ast_children_arr_with_last_args_when_type_supported_and_not_data_wrapper,
            create_non_typed_data_wrapper_arr_setup, create_non_typed_data_wrapper_arr_teardown),
        cmocka_unit_test_setup_teardown(
            create_non_typed_data_wrapper_arr_returns_null_when_ast_create_ast_children_arr_fails,
            create_non_typed_data_wrapper_arr_setup, create_non_typed_data_wrapper_arr_teardown),
        cmocka_unit_test_setup_teardown(
            create_non_typed_data_wrapper_arr_calls_malloc_for_an_ast_when_ast_create_ast_children_arr_succeeds,
            create_non_typed_data_wrapper_arr_setup, create_non_typed_data_wrapper_arr_teardown),
        cmocka_unit_test_setup_teardown(
            create_non_typed_data_wrapper_arr_calls_free_and_returns_null_when_malloc_fails,
            create_non_typed_data_wrapper_arr_setup, create_non_typed_data_wrapper_arr_teardown),
        cmocka_unit_test_setup_teardown(
            create_non_typed_data_wrapper_arr_initializes_and_returns_malloced_ast_when_malloc_succeeds,
            create_non_typed_data_wrapper_arr_setup, create_non_typed_data_wrapper_arr_teardown),
    };

    const struct CMUnitTest ast_create_non_typed_data_wrapper_var_tests[] = {
        cmocka_unit_test(create_non_typed_data_wrapper_var_returns_null_when_type_data_wrapper_or_unsupported),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_calls_malloc_for_an_ast_when_type_supported_and_not_data_wrapper,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_calls_malloc_for_an_ast_when_type_supported_and_not_data_wrapper,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_calls_malloc_for_an_ast_when_type_supported_and_not_data_wrapper,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_returns_null_when_malloc_for_an_ast_fails,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_returns_null_when_malloc_for_an_ast_fails,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_returns_null_when_malloc_for_an_ast_fails,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_calls_ast_create_ast_children_arr_when_malloc_for_an_ast_succeeds_and_children_nb_0,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_calls_free_and_returns_null_when_ast_create_ast_children_arr_fails_and_children_nb_0,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_initializes_and_returns_malloced_ast_when_ast_create_ast_children_arr_succeeds_and_children_nb_0,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_calls_malloc_for_an_array_of_pointer_of_ast_when_malloc_for_an_ast_succeeds_and_children_nb_not_0,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_calls_malloc_for_an_array_of_pointer_of_ast_when_malloc_for_an_ast_succeeds_and_children_nb_not_0,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_calls_free_and_returns_null_when_malloc_for_an_array_of_pointer_of_ast_fails,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_calls_free_and_returns_null_when_malloc_for_an_array_of_pointer_of_ast_fails,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_initializes_ast_p_array_and_calls_ast_create_ast_children_arr_when_malloc_for_ast_p_array_succeeds,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_initializes_ast_p_array_and_calls_ast_create_ast_children_arr_when_malloc_for_ast_p_array_succeeds,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_frees_two_malloced_pointers_when_ast_create_ast_children_arr_fails,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_frees_two_malloced_pointers_when_ast_create_ast_children_arr_fails,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_initializes_and_returns_malloced_ast_and_frees_ast_p_arr_when_ast_create_ast_children_arr_succeeds,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_non_typed_data_wrapper_var_initializes_and_returns_malloced_ast_and_frees_ast_p_arr_when_ast_create_ast_children_arr_succeeds,
            create_non_typed_data_wrapper_var_setup, create_non_typed_data_wrapper_var_teardown, (void *)&create_non_typed_data_wrapper_var_params_template_two_children),
    };

    const struct CMUnitTest ast_destroy_non_typed_data_wrapper_tests[] = {
        cmocka_unit_test(destroy_non_typed_data_wrapper_do_nothing_when_arg_null),
        cmocka_unit_test_setup_teardown(
            destroy_non_typed_data_wrapper_do_nothing_when_arg_is_typed_data_wrapper,
            destroy_non_typed_data_wrapper_setup, destroy_non_typed_data_wrapper_teardown),
        cmocka_unit_test_setup_teardown(
            destroy_non_typed_data_wrapper_calls_ast_destroy_ast_children_and_calls_free_when_arg_is_valid,
            destroy_non_typed_data_wrapper_setup, destroy_non_typed_data_wrapper_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(create_typed_data_int_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_int_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_typed_data_string_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_string_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_typed_data_symbol_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_symbol_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_typed_data_wrapper_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_ast_children_arr_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_ast_children_arr_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_ast_children_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_non_typed_data_wrapper_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_non_typed_data_wrapper_arr_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_non_typed_data_wrapper_var_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_non_typed_data_wrapper_tests, NULL, NULL);

    return failed;
}
