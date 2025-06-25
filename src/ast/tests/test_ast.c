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
static typed_data *const DUMMY_TYPED_DATA = (typed_data*)&dummy[3];
static char *string_value = NULL;
static char *symbol_value = NULL;
static typed_data *typed_data_int = NULL;
static typed_data *typed_data_string = NULL;
static typed_data *typed_data_symbol = NULL;
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
static void *fake_typed_data_int = NULL;
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
    alloc_and_save_address_to_be_freed((void **)&fake_typed_data_int, sizeof(typed_data));
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
    expect_value(mock_free, ptr, fake_typed_data_int);
    ast_destroy_typed_data_int(fake_typed_data_int);
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
    alloc_and_save_address_to_be_freed((void **)&typed_data_string, sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **)&string_value, strlen(DUMMY_STRING)+1);
    typed_data_string->type = TYPE_STRING;
    typed_data_string->data.string_value = string_value;
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
// Expected: calls free with typed_data_string->data.string_value and then with typed_data_string
static void destroy_typed_data_string_calls_free_for_string_value_field_then_for_typed_data_string(void **state) {
    expect_value(mock_free, ptr, typed_data_string->data.string_value);
    expect_value(mock_free, ptr, typed_data_string);
    ast_destroy_typed_data_string(typed_data_string);
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
    alloc_and_save_address_to_be_freed((void **)&typed_data_symbol, sizeof(typed_data));
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
    expect_value(mock_free, ptr, typed_data_symbol);
    ast_destroy_typed_data_symbol(typed_data_symbol);
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
    ast_create_typed_data_wrapper(DUMMY_TYPED_DATA);
}

// Given: malloc fails
// Expected: return NULL
static void create_typed_data_wrapper_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_typed_data_wrapper(DUMMY_TYPED_DATA));
}

// Given: malloc succeeds
// Expected: the malloc'ed typed_data is initialized and returned
static void create_typed_data_wrapper_initializes_and_returns_malloced_ast_when_malloc_succeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    ast *ret = ast_create_typed_data_wrapper(DUMMY_TYPED_DATA);
    assert_int_equal(ret->type, AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->data, DUMMY_TYPED_DATA);
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
        not_a_data_wrapper->children.children_nb = DUMMY_CHILDREN_NB;
        not_a_data_wrapper->children.children = DUMMY_AST_CHILDREN;
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
    .label = "children_nb = 1",
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

// Given: children_nb = 0
// Expected: returns null
// params: create_ast_children_arr_params_no_child
static void create_ast_children_arr_return_null_when_children_nb_0(void **state) {
    create_ast_children_arr_params_t *params = *state;
    assert_null(ast_create_ast_children_arr(params->children_nb, params->ast_children));
}

// Given: children_nb = 1 or 2
// Expected: calls malloc with sizeof(ast_children_t)
// params:
//  - create_ast_children_arr_params_template_one_child
//  - create_ast_children_arr_params_template_two_children
static void create_ast_children_arr_calls_malloc_for_an_ast_children_t_when_children_nb_1_or_2(void **state) {
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

// Given: children_nb = 0
// Expected: returns null
// params: create_ast_children_var_params_no_child
static void create_ast_children_var_return_null_when_children_nb_0(void **state) {
    create_ast_children_var_params_t *params = *state;
    assert_null(ast_create_ast_children_var(params->children_nb));
}

// Given: children_nb = 1
// Expected: calls malloc with sizeof(ast_children_t)
// params:
//  - create_ast_children_var_params_template_one_child
//  - create_ast_children_var_params_template_two_children
static void create_ast_children_var_calls_malloc_for_an_ast_children_t_when_children_nb_1_or_2(void **state) {
    create_ast_children_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid mock call
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
// Expected: the malloc'ed typed_data is initialized and returned
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
// Expected: the malloc'ed typed_data is initialized and returned
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
            create_ast_children_arr_return_null_when_children_nb_0,
            create_ast_children_arr_setup, create_ast_children_arr_teardown, (void *)&create_ast_children_arr_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_calls_malloc_for_an_ast_children_t_when_children_nb_1_or_2,
            create_ast_children_arr_setup, create_ast_children_arr_teardown, (void *)&create_ast_children_arr_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_calls_malloc_for_an_ast_children_t_when_children_nb_1_or_2,
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
            create_ast_children_var_return_null_when_children_nb_0,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_an_ast_children_t_when_children_nb_1_or_2,
            create_ast_children_var_setup, create_ast_children_var_teardown, (void *)&create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_an_ast_children_t_when_children_nb_1_or_2,
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
    failed += cmocka_run_group_tests(ast_create_ast_children_var_tests, NULL, NULL);

    return failed;
}
