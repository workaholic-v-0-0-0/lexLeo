// src/ast/tests/test_ast.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "memory_allocator.h"
#include "string_utils.h"
#include "logger.h"
#include "list.h"

#include "internal/ast_test_utils.h"


//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static list collected_ptr_to_be_freed = NULL;

static char dummy[13];
static const void *DUMMY_MALLOC_RETURNED_VALUE = (void *) &dummy[0];
static const void *DUMMY_STRDUP_RETURNED_VALUE = &dummy[1];
#define MALLOC_ERROR_CODE NULL
static const int DUMMY_INT = 7;
static char *const DUMMY_STRING = "dummy string";
static char *const DUMMY_SYMBOL_NAME = "symbol_name";
static void *const DUMMY_SYMBOL = (void *) &dummy[2];
#define STRDUP_ERROR_CODE NULL
static typed_data *const DUMMY_TYPED_DATA_P = (typed_data *) &dummy[3];
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
static ast *const DUMMY_DATA_WRAPPER = (ast *) &dummy[4];
static const int DUMMY_CHILDREN_NB = 777;
static ast **const DUMMY_AST_CHILDREN = (ast **) &dummy[5];
static ast **const AST_CHILDREN_DEFINED_IN_SETUP = (ast **) &dummy[6];
static ast dummy_ast_child_one;
static ast dummy_ast_child_two;
static ast *DUMMY_CHILDREN_ARRAY_OF_SIZE_ONE[] = {&dummy_ast_child_one};
static ast *DUMMY_CHILDREN_ARRAY_OF_SIZE_TWO[] = {&dummy_ast_child_one, &dummy_ast_child_two};
static ast_children_t *const AST_CHILDREN_T_DEFINED_IN_SETUP = (ast_children_t *) &dummy[7];
static ast_children_t *const DUMMY_AST_CHILDREN_T_P = (ast_children_t *) &dummy[8];
static const int UNSUPPORTED_AST_TYPE = AST_TYPE_NB_TYPES;
static const int SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER = AST_TYPE_TRANSLATION_UNIT;
static ast *ast_not_data_wrapper = NULL;
#define DUMMY_ERROR_TYPE 2
static char *DUMMY_ERROR_MESSAGE = "This is a dummy error message";
static char too_long_error_message[MAXIMUM_ERROR_MESSAGE_LENGTH + 1];
static char *valid_error_message = "This is a valid error message";
static ast *ast_error_node = NULL;
static error_info *error = NULL;
static ast *AST_ERROR_SENTINEL;
static const size_t DUMMY_CAPACITY = 77;
static ast_children_t *ast_children_p = NULL;
static ast *const DUMMY_AST_CHILD_0 = (ast *) &dummy[9];
static ast *const DUMMY_AST_CHILD_1 = (ast *) &dummy[10];
static ast *const DUMMY_AST_CHILD_TO_BE_ADDED = (ast *) &dummy[11];
static ast *children_node_p = NULL;
static error_info *const DUMMY_ERROR_INFO_P = (error_info *) &dummy[12];
static const data_type STUB_DATA_TYPE = TYPE_INT;


//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


void *mock_malloc(size_t size) {
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

void *mock_realloc(void *ptr, size_t size) {
    check_expected_ptr(ptr);
    check_expected(size);
    return mock_type(void *);
}

static void *fake_malloc_returned_value_for_a_typed_data_int = NULL;
static void *fake_malloc_returned_value_for_a_typed_data_string = NULL;
static void *fake_malloc_returned_value_for_a_typed_data_symbol_name = NULL;
static void *fake_malloc_returned_value_for_a_typed_data_symbol = NULL;
static void *fake_malloc_returned_value_for_an_ast = NULL;
static void *fake_malloc_returned_value_for_an_ast_children_t = NULL;
static void *fake_malloc_returned_value_for_a_double_ast_pointer = NULL;
static void *fake_malloc_returned_value_for_an_error_info = NULL;
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

void mock_ast_destroy_children_node(ast *children_node) {
    check_expected_ptr(children_node);
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

static char *fake_strdup_returned_value_for_error_message;

void *fake_broken_malloc(size_t size) {
    (void) size;
    return NULL;
}

void fake_broken_free(void *ptr) {
    (void) ptr;
}

char *fake_broken_strdup(const char *s) {
    (void) s;
    return NULL;
}

#define FAKE_HEAP_SIZE 1024
static uint8_t fake_heap[FAKE_HEAP_SIZE] = {0};
static uint8_t *fake_heap_top_pointer;
static uint8_t *const FAKE_HEAP_END = &fake_heap[FAKE_HEAP_SIZE];

void fake_heap_reset(void) {
    memset(fake_heap, 0, sizeof(fake_heap));
    fake_heap_top_pointer = fake_heap;
}

void *fake_operational_malloc(size_t size) {
    assert(size > 0);
    assert(size <= (size_t) (FAKE_HEAP_END - fake_heap_top_pointer));
    void *ret = fake_heap_top_pointer;
    fake_heap_top_pointer += size;
    return ret;
}

void fake_operational_free(void *ptr) {
    (void) ptr;
}

char *fake_operational_strdup(const char *s) {
    assert_non_null(s);
    size_t size = strlen(s) + 1;
    char *ret = (char *) fake_operational_malloc(size);
    assert_non_null(ret);
    memcpy(ret, s, size);
    return ret;
}

static void *fake_realloc_returned_value = NULL;


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
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_int, sizeof(typed_data));
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
    alloc_and_save_address_to_be_freed((void **) &fake_typed_data_int_p, sizeof(typed_data));
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

// Given:
//  - malloc succeeds
//  - s == NULL
// Expected:
//  - letting ret denote the allocated typed_data:
//    - ret->type == TYPE_STRING
//    - ret->data.string_value == NULL
//  - returns the allocated typed_data
static void ast_create_typed_data_string_initializes_and_returns_malloced_typed_data_when_malloc_succeeds_and_s_null(
    void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_string,
                                       sizeof(typed_data));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_string);
    typed_data *ret = ast_create_typed_data_string(NULL);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_typed_data_string);
    assert_int_equal(ret->type, TYPE_STRING);
    assert_ptr_equal(ret->data.string_value, NULL);
}

// Given:
//  - malloc succeeds
//  - s != NULL
// Expected: calls strdup with s
static void ast_create_typed_data_string_calls_strdup_when_malloc_succeeds_and_s_not_null(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);
    ast_create_typed_data_string(DUMMY_STRING);
}

// Given: strdup fails
// Expected: calls free with pointer returned by malloc for a typed data and returns null
static void ast_create_typed_data_string_calls_free_and_returns_null_when_strdup_fails(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_string,
                                       sizeof(typed_data));
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
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_string,
                                       sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **) &fake_strdup_returned_value_for_string_value,
                                       strlen(DUMMY_STRING) + 1);
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
    alloc_and_save_address_to_be_freed((void **) &typed_data_string_p, sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **) &string_value, strlen(DUMMY_STRING) + 1);
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
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_symbol,
                                       sizeof(typed_data));
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
    alloc_and_save_address_to_be_freed((void **) &typed_data_symbol_p, sizeof(typed_data));
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
// Expected: calls malloc with sizeof(ast)
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
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
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
    alloc_and_save_address_to_be_freed((void **) &(params->typed_data_wrapper), sizeof(ast));
    params->typed_data_wrapper->type = AST_TYPE_DATA_WRAPPER;
    switch (params->type) {
        case TYPE_INT:
            alloc_and_save_address_to_be_freed((void **) &(params->typed_data_int), sizeof(typed_data));
            params->typed_data_int->type = TYPE_INT;
            params->typed_data_int->data.int_value = DUMMY_INT;
            params->typed_data_wrapper->data = params->typed_data_int;
            break;
        case TYPE_STRING:
            alloc_and_save_address_to_be_freed((void **) &(params->typed_data_string), sizeof(typed_data));
            params->typed_data_string->type = TYPE_STRING;
            alloc_and_save_address_to_be_freed((void **) &(params->typed_data_string->data.string_value),
                                               strlen(DUMMY_STRING) + 1);
            memcpy(params->typed_data_string->data.string_value, DUMMY_STRING, strlen(DUMMY_STRING) + 1);
            params->typed_data_wrapper->data = params->typed_data_string;
            break;
        case TYPE_SYMBOL:
            alloc_and_save_address_to_be_freed((void **) &(params->typed_data_symbol), sizeof(typed_data));
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
    if (state == (void **) &destroy_typed_data_wrapper_not_a_data_wrapper) {
        ast *not_a_data_wrapper = NULL;
        alloc_and_save_address_to_be_freed((void *) &not_a_data_wrapper, sizeof(ast));
        not_a_data_wrapper->type = AST_TYPE_ADDITION;
        not_a_data_wrapper->children->children_nb = DUMMY_CHILDREN_NB;
        not_a_data_wrapper->children->children = DUMMY_AST_CHILDREN;
        *state = not_a_data_wrapper;
    } else {
        alloc_and_save_address_to_be_freed((void **) &params, sizeof(destroy_typed_data_wrapper_params_t));
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
    ast_destroy_typed_data_wrapper((ast *) *state);
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
    ast *typed_data_wrapper = ((destroy_typed_data_wrapper_params_t *) (*state))->typed_data_wrapper;
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
    alloc_and_save_address_to_be_freed((void **) &params, sizeof(create_ast_children_arr_params_t));
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
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast_children_t,
                                       sizeof(ast_children_t));
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast_children_t);
    ast_children_t *ret = ast_create_ast_children_arr(params->children_nb, params->ast_children);
    assert_int_equal(ret->children_nb, params->children_nb);
    assert_int_equal(ret->capacity, ret->children_nb);
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


static const create_ast_children_var_params_t create_ast_children_var_params_no_child =
        create_ast_children_arr_params_no_child;
static const create_ast_children_var_params_t create_ast_children_var_params_template_one_child =
        create_ast_children_arr_params_template_one_child;
static const create_ast_children_var_params_t create_ast_children_var_params_template_two_children =
        create_ast_children_arr_params_template_two_children;


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
static void create_ast_children_var_calls_malloc_for_a_double_pointer_of_ast_when_malloc_for_an_ast_children_t_succeeds(
    void **state) {
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
static void
create_ast_children_var_calls_free_with_pointer_for_ast_children_t_when_malloc_for_double_pointer_of_ast_fails(
    void **state) {
    create_ast_children_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast_children_t,
                                       sizeof(ast_children_t));
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
static void
create_ast_children_var_initializes_and_returns_malloced_ast_children_t_when_malloc_for_double_pointer_of_ast_succeeds_and_one_arg(
    void **state) {
    create_ast_children_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast_children_t,
                                       sizeof(ast_children_t));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_double_ast_pointer,
                                       params->children_nb * sizeof(ast *));
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast_children_t);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_double_ast_pointer);
    ast_children_t *ret = ast_create_ast_children_var(params->children_nb, (params->ast_children)[0]);
    assert_int_equal(ret->children_nb, params->children_nb);
    assert_int_equal(ret->capacity, ret->children_nb);
    assert_ptr_equal(ret->children[0], params->ast_children[0]);
    assert_ptr_not_equal(ret->children, params->ast_children); // double pointer of ast has been malloced
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast_children_t);
}

// Given: malloc for a double pointer of ast succeeds, children_nb = 2
// Expected: the malloc'ed ast_children_t is initialized and returned
// params:
//  - create_ast_children_var_params_template_two_child
static void
create_ast_children_var_initializes_and_returns_malloced_ast_children_t_when_malloc_for_double_pointer_of_ast_succeeds_and_two_args(
    void **state) {
    create_ast_children_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast_children_t,
                                       sizeof(ast_children_t));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_double_ast_pointer,
                                       params->children_nb * sizeof(ast *));
    expect_value(mock_malloc, size, sizeof(ast_children_t));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast_children_t);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_double_ast_pointer);
    ast_children_t *ret = ast_create_ast_children_var(params->children_nb, (params->ast_children)[0],
                                                      (params->ast_children)[1]);
    assert_int_equal(ret->children_nb, params->children_nb);
    assert_int_equal(ret->capacity, ret->children_nb);
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
        alloc_and_save_address_to_be_freed((void **) &(params->ast_children), sizeof(ast_children_t));
        params->ast_children->children_nb = 0;
        params->ast_children->children = DUMMY_AST_CHILDREN;
    }
    if (params->children_nb == 1) {
        alloc_and_save_address_to_be_freed((void **) &params->ast_children, sizeof(ast_children_t));
        params->ast_children->children_nb = 1;
        alloc_and_save_address_to_be_freed((void **) &(params->ast_children->children), 1 * sizeof(ast *));
        alloc_and_save_address_to_be_freed((void **) &((params->ast_children->children)[0]), sizeof(ast));
        ((params->ast_children->children)[0])->type = AST_TYPE_DATA_WRAPPER;
        ((params->ast_children->children)[0])->data = DUMMY_TYPED_DATA_P;
    }
    if (params->children_nb == 2) {
        alloc_and_save_address_to_be_freed((void **) &params->ast_children, sizeof(ast_children_t));
        params->ast_children->children_nb = 2;
        alloc_and_save_address_to_be_freed((void **) &(params->ast_children->children), 2 * sizeof(ast *));
        alloc_and_save_address_to_be_freed((void **) &((params->ast_children->children)[0]), sizeof(ast));
        alloc_and_save_address_to_be_freed((void **) &((params->ast_children->children)[1]), sizeof(ast));
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
    alloc_and_save_address_to_be_freed((void **) &params, sizeof(destroy_ast_children_params_t));
    *params = *model;
    initialize_ast_children_dynamically(params);
    *state = params;
    set_allocators(mock_malloc, mock_free);
    set_ast_destroy_typed_data_wrapper(mock_ast_destroy_typed_data_wrapper);
    set_ast_destroy_children_node(mock_ast_destroy_children_node);
    return 0;
}

static int destroy_ast_children_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_ast_destroy_typed_data_wrapper(NULL);
    set_ast_destroy_children_node(NULL);
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
//  - calls ast_destroy_children_node with ast_children->children[1]
//  - calls free with ast_children->children
//  - calls free with ast_children
// params: destroy_ast_children_params_template_two_child
static void destroy_ast_children_calls_destroy_properly_when_two_children_one_data_wrapper_other_not(void **state) {
    destroy_ast_children_params_t *params = *state;
    expect_value(mock_ast_destroy_typed_data_wrapper, ast_data_wrapper, params->ast_children->children[0]);
    expect_value(mock_ast_destroy_children_node, children_node, params->ast_children->children[1]);
    expect_value(mock_free, ptr, params->ast_children->children);
    expect_value(mock_free, ptr, params->ast_children);
    ast_destroy_ast_children(params->ast_children);
}


//-----------------------------------------------------------------------------
// ast_create_children_node TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_children_node_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_children_node_teardown(void **state) {
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
static void create_children_node_returns_null_when_type_data_wrapper_or_unsupported(void **state) {
    assert_null(ast_create_children_node(AST_TYPE_DATA_WRAPPER, DUMMY_AST_CHILDREN_T_P));
    assert_null(ast_create_children_node(UNSUPPORTED_AST_TYPE, DUMMY_AST_CHILDREN_T_P));
}

// Given: type is supported and not AST_TYPE_DATA_WRAPPER, ast_children is null
// Expected: returns null
static void create_children_node_returns_null_when_ast_children_null(void **state) {
    assert_null(ast_create_children_node(AST_TYPE_TRANSLATION_UNIT, NULL));
}

// Given: type is supported, ast_children is not null
// Expected: calls malloc with sizeof(ast)
static void create_children_node_calls_malloc_for_an_ast_when_args_valid(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    ast_create_children_node(AST_TYPE_TRANSLATION_UNIT, DUMMY_AST_CHILDREN_T_P);
}

// Given: malloc fails
// Expected: returns null
static void create_children_node_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_children_node(AST_TYPE_TRANSLATION_UNIT, DUMMY_AST_CHILDREN_T_P));
}

// Given: malloc succeeds
// Expected: the malloc'ed ast is initialized and returned
static void create_children_node_initializes_and_returns_malloced_ast_when_malloc_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    ast *ret = ast_create_children_node(AST_TYPE_TRANSLATION_UNIT, DUMMY_AST_CHILDREN_T_P);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, AST_TYPE_TRANSLATION_UNIT);
    assert_ptr_equal(ret->children, DUMMY_AST_CHILDREN_T_P);
}


//-----------------------------------------------------------------------------
// ast_create_children_node_arr TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_children_node_arr_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    set_ast_create_ast_children_arr(mock_ast_create_ast_children_arr);
    return 0;
}

static int create_children_node_arr_teardown(void **state) {
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
static void create_children_node_arr_returns_null_when_type_data_wrapper_or_unsupported(void **state) {
    assert_null(ast_create_children_node_arr(AST_TYPE_DATA_WRAPPER, DUMMY_INT, DUMMY_AST_CHILDREN));
    assert_null(ast_create_children_node_arr(UNSUPPORTED_AST_TYPE, DUMMY_INT, DUMMY_AST_CHILDREN));
}

// Given: type is supported and not AST_TYPE_DATA_WRAPPER
// Expected: calls ast_create_ast_children_arr with children_nb and children
static void
create_children_node_arr_calls_ast_create_ast_children_arr_with_last_args_when_type_supported_and_not_data_wrapper(
    void **state) {
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, NULL); // to avoid more mock calls
    ast_create_children_node_arr(AST_TYPE_TRANSLATION_UNIT, DUMMY_INT, DUMMY_AST_CHILDREN);
}

// Given: ast_create_ast_children_arr fails (ie returns NULL)
// Expected: returns NULL
static void create_children_node_arr_returns_null_when_ast_create_ast_children_arr_fails(void **state) {
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, NULL);
    assert_null(ast_create_children_node_arr(AST_TYPE_TRANSLATION_UNIT, DUMMY_INT, DUMMY_AST_CHILDREN));
}

// Given: ast_create_ast_children_arr succeeds (ie does not return NULL)
// Expected: calls malloc with sizeof(ast)
static void create_children_node_arr_calls_malloc_for_an_ast_when_ast_create_ast_children_arr_succeeds(void **state) {
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    ast_create_children_node_arr(AST_TYPE_TRANSLATION_UNIT, DUMMY_INT, DUMMY_AST_CHILDREN);
}

// Given: calls malloc with sizeof(ast) fails
// Expected: calls free with pointer returned by ast_create_ast_children_arr and returns null
static void create_children_node_arr_calls_free_and_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_AST_CHILDREN_T_P);
    assert_null(ast_create_children_node_arr(AST_TYPE_TRANSLATION_UNIT, DUMMY_INT, DUMMY_AST_CHILDREN));
}

// Given: calls malloc with sizeof(ast) succeeds
// Expected: the malloc'ed ast is initialized and returned
static void create_children_node_arr_initializes_and_returns_malloced_ast_when_malloc_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_ast_create_ast_children_arr_returned_value,
                                       sizeof(ast_children_t));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_ast_create_ast_children_arr, children_nb, DUMMY_INT);
    expect_value(mock_ast_create_ast_children_arr, children, DUMMY_AST_CHILDREN);
    will_return(mock_ast_create_ast_children_arr, fake_ast_create_ast_children_arr_returned_value);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    ast *ret = ast_create_children_node_arr(AST_TYPE_TRANSLATION_UNIT, DUMMY_INT, DUMMY_AST_CHILDREN);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, AST_TYPE_TRANSLATION_UNIT);
    assert_ptr_equal(ret->children, fake_ast_create_ast_children_arr_returned_value);
}


//-----------------------------------------------------------------------------
// ast_create_children_node_var TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURES
//----------------------------------------------------------------------------

#define create_children_node_var_params_t create_ast_children_arr_params_t


//-----------------------------------------------------------------------------
// PARAM CASES
//-----------------------------------------------------------------------------


static const create_children_node_var_params_t create_children_node_var_params_no_child =
        create_ast_children_arr_params_no_child;
static const create_children_node_var_params_t create_children_node_var_params_template_one_child =
        create_ast_children_arr_params_template_one_child;
static const create_children_node_var_params_t create_children_node_var_params_template_two_children =
        create_ast_children_arr_params_template_two_children;


//-----------------------------------------------------------------------------
// HELPER
//-----------------------------------------------------------------------------


// use initialize_ast_children define in "ast_create_ast_children_arr TESTS" section


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_children_node_var_setup(void **state) {
    const create_children_node_var_params_t *model = *state;
    create_children_node_var_params_t *params = NULL;
    alloc_and_save_address_to_be_freed((void **) &params, sizeof(create_children_node_var_params_t));
    *params = *model;
    initialize_ast_children(params);
    *state = params;
    set_allocators(mock_malloc, mock_free);
    set_ast_create_ast_children_arr(mock_ast_create_ast_children_arr);
    return 0;
}

static int create_children_node_var_teardown(void **state) {
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
static void create_children_node_var_returns_null_when_type_data_wrapper_or_unsupported(void **state) {
    assert_null(ast_create_children_node_var(AST_TYPE_DATA_WRAPPER, DUMMY_INT, DUMMY_AST_CHILDREN));
    assert_null(ast_create_children_node_var(UNSUPPORTED_AST_TYPE, DUMMY_INT, DUMMY_AST_CHILDREN));
}

// Given: type is supported and not AST_TYPE_DATA_WRAPPER, children_nb = 0, 1 or 2
// Expected:
//  - calls malloc with sizeof(ast)
// params:
//  - create_children_node_var_params_no_child
//  - create_children_node_var_params_template_one_child;
//  - create_children_node_var_params_template_two_children
static void create_children_node_var_calls_malloc_for_an_ast_when_type_supported_and_not_data_wrapper(void **state) {
    create_children_node_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid more mock calls
    if (params->children_nb == 0) {
        ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            0);
    }
    if (params->children_nb == 1) {
        ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0]);
    }
    if (params->children_nb == 2) {
        ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1]);
    }
}

// Given: malloc for an ast fails
// Expected: returns null
// params:
//  - create_children_node_var_params_no_child
//  - create_children_node_var_params_template_one_child
//  - create_children_node_var_params_template_two_children
static void create_children_node_var_returns_null_when_malloc_for_an_ast_fails(void **state) {
    create_children_node_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    ast *ret = NULL;
    if (params->children_nb == 0) {
        ret = ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            0);
    }
    if (params->children_nb == 1) {
        ret = ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0]);
    }
    if (params->children_nb == 2) {
        ret = ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1]);
    }
    assert_null(ret);
}

// Given: malloc for an ast succeeds, children_nb = 0
// Expected: calls ast_create_ast_children_arr with 0 and NULL (and then get an ast_children_t pointing to {0, NULL})
// params: create_children_node_var_params_no_child
static void
create_children_node_var_calls_ast_create_ast_children_arr_when_malloc_for_an_ast_succeeds_and_children_nb_0(
    void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_ast_create_ast_children_arr, children_nb, 0);
    expect_value(mock_ast_create_ast_children_arr, children, NULL);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    ast_create_children_node_var(
        SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
        0);
}

// Given: calls ast_create_ast_children_arr with 0 and NULL fails
// Expected: calls free with pointer returned by malloc for an ast and return NULL
// params: create_children_node_var_params_no_child
static void
create_children_node_var_calls_free_and_returns_null_when_ast_create_ast_children_arr_fails_and_children_nb_0(
    void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_ast_create_ast_children_arr, children_nb, 0);
    expect_value(mock_ast_create_ast_children_arr, children, NULL);
    will_return(mock_ast_create_ast_children_arr, NULL);
    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE);
    assert_null(
        ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            0));
}

// Given: calls ast_create_ast_children_arr with 0 and NULL succeeds
// Expected: the malloc'ed ast is initialized and returned
// params: create_children_node_var_params_no_child
static void
create_children_node_var_initializes_and_returns_malloced_ast_when_ast_create_ast_children_arr_succeeds_and_children_nb_0(
    void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_ast_create_ast_children_arr, children_nb, 0);
    expect_value(mock_ast_create_ast_children_arr, children, NULL);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    ast *ret =
            ast_create_children_node_var(
                SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
                0);
    assert_ptr_equal(ret, DUMMY_MALLOC_RETURNED_VALUE);
    assert_int_equal(ret->type, SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->children, DUMMY_AST_CHILDREN_T_P);
}

// Given: malloc for an ast succeeds, children_nb = 1
// Expected: calls malloc with children_nb * sizeof() for an array of ast pointer
// params:
//  - create_children_node_var_params_template_one_child
//  - create_children_node_var_params_template_two_children
static void
create_children_node_var_calls_malloc_for_an_array_of_pointer_of_ast_when_malloc_for_an_ast_succeeds_and_children_nb_not_0(
    void **state) {
    create_children_node_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid more mock calls
    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE); // to avoid more mock calls
    if (params->children_nb == 1)
        ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0]);
    if (params->children_nb == 2)
        ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1]);
}

// Given: malloc for an array of ast pointer fails
// Expected: calls free with pointer returned by malloc for an ast and return NULL
// params:
//  - create_children_node_var_params_template_one_child
//  - create_children_node_var_params_template_two_children
static void create_children_node_var_calls_free_and_returns_null_when_malloc_for_an_array_of_pointer_of_ast_fails(
    void **state) {
    create_children_node_var_params_t *params = *state;
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE);
    ast *ret = NULL;
    if (params->children_nb == 1)
        ret = ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0]);
    if (params->children_nb == 2)
        ret = ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1]);
    assert_null(ret);
}

// Given: malloc for an array of ast pointer succeeds
// Expected:
//  - initializes malloc'ed array of ast pointer with variadic arguments
//  - calls ast_create_ast_children_arr with children_nb and the array of ast pointer
// params:
//  - create_children_node_var_params_template_one_child
//  - create_children_node_var_params_template_two_children
static void
create_children_node_var_initializes_ast_p_array_and_calls_ast_create_ast_children_arr_when_malloc_for_ast_p_array_succeeds(
    void **state) {
    create_children_node_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_double_ast_pointer,
                                       params->children_nb * sizeof(ast *));
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
        ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0]);
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **) fake_malloc_returned_value_for_a_double_ast_pointer)[0]);
    }
    if (params->children_nb == 2) {
        ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1]);
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **) fake_malloc_returned_value_for_a_double_ast_pointer)[0]);
        assert_ptr_equal(
            (params->ast_children)[1],
            ((ast **) fake_malloc_returned_value_for_a_double_ast_pointer)[1]);
    }
}

// Given: ast_create_ast_children_arr with children_nb and the array of ast pointer fails
// Expected:
//  - calls free with the array of ast pointer
//  - calls free with the pointer for an ast
//  - returns NULL
// params:
//  - create_children_node_var_params_template_one_child
//  - create_children_node_var_params_template_two_children
static void create_children_node_var_frees_two_malloced_pointers_when_ast_create_ast_children_arr_fails(void **state) {
    create_children_node_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_double_ast_pointer,
                                       params->children_nb * sizeof(ast *));
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
        ret = ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0]);
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **) fake_malloc_returned_value_for_a_double_ast_pointer)[0]);
    }
    if (params->children_nb == 2) {
        ret = ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1]);
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **) fake_malloc_returned_value_for_a_double_ast_pointer)[0]);
        assert_ptr_equal(
            (params->ast_children)[1],
            ((ast **) fake_malloc_returned_value_for_a_double_ast_pointer)[1]);
    }
    assert_null(ret);
}

// Given: ast_create_ast_children_arr with children_nb and the array of ast pointer succeeds
// Expected:
//  - initializes the malloc'ed ast
//  - returns the malloc'ed ast
// params:
//  - create_children_node_var_params_template_one_child
//  - create_children_node_var_params_template_two_children
static void create_children_node_var_initializes_and_returns_malloced_ast_when_ast_create_ast_children_arr_succeeds(
    void **state) {
    create_children_node_var_params_t *params = *state;
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_double_ast_pointer,
                                       params->children_nb * sizeof(ast *));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    expect_value(mock_malloc, size, params->children_nb * sizeof(ast *));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_double_ast_pointer);
    expect_value(mock_ast_create_ast_children_arr, children_nb, params->children_nb);
    expect_value(mock_ast_create_ast_children_arr, children, fake_malloc_returned_value_for_a_double_ast_pointer);
    will_return(mock_ast_create_ast_children_arr, DUMMY_AST_CHILDREN_T_P);
    ast *ret = NULL;
    if (params->children_nb == 1) {
        ret = ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            1,
            (params->ast_children)[0]);
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **) fake_malloc_returned_value_for_a_double_ast_pointer)[0]);
    }
    if (params->children_nb == 2) {
        ret = ast_create_children_node_var(
            SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER,
            2,
            (params->ast_children)[0],
            (params->ast_children)[1]);
        assert_ptr_equal(
            (params->ast_children)[0],
            ((ast **) fake_malloc_returned_value_for_a_double_ast_pointer)[0]);
        assert_ptr_equal(
            (params->ast_children)[1],
            ((ast **) fake_malloc_returned_value_for_a_double_ast_pointer)[1]);
    }
    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, SUPPORTED_AST_TYPE_BUT_NOT_AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->children, DUMMY_AST_CHILDREN_T_P);
}


//-----------------------------------------------------------------------------
// ast_destroy_children_node TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_children_node_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **) &ast_not_data_wrapper, sizeof(ast));
    set_allocators(mock_malloc, mock_free);
    set_ast_destroy_ast_children(mock_ast_destroy_ast_children);
    return 0;
}

static int destroy_children_node_teardown(void **state) {
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


// Given: children_node = NULL
// Expected: do nothing
static void destroy_children_node_do_nothing_when_arg_null(void **state) {
    ast_destroy_children_node(NULL);
}

// Given: children_node != NULL, children_node->type == AST_TYPE_DATA_WRAPPER
// Expected: do nothing
static void destroy_children_node_do_nothing_when_arg_is_typed_data_wrapper(void **state) {
    ast_not_data_wrapper->type = AST_TYPE_DATA_WRAPPER;
    ast_not_data_wrapper->children = DUMMY_AST_CHILDREN_T_P;
    ast_destroy_children_node(ast_not_data_wrapper);
}

// Given: children_node->type is not AST_TYPE_DATA_WRAPPER (and can even be not supported)
// Expected:
//  - calls ast_destroy_ast_children with children_node->children
//  - calls free with children_node
static void destroy_children_node_calls_ast_destroy_ast_children_and_calls_free_when_arg_is_valid(void **state) {
    ast_not_data_wrapper->type = AST_TYPE_ADDITION;
    ast_not_data_wrapper->children = DUMMY_AST_CHILDREN_T_P;
    expect_value(mock_ast_destroy_ast_children, ast_children, DUMMY_AST_CHILDREN_T_P);
    expect_value(mock_free, ptr, ast_not_data_wrapper);
    ast_destroy_children_node(ast_not_data_wrapper);

    ast_not_data_wrapper->type = UNSUPPORTED_AST_TYPE;
    expect_value(mock_ast_destroy_ast_children, ast_children, DUMMY_AST_CHILDREN_T_P);
    expect_value(mock_free, ptr, ast_not_data_wrapper);
    ast_destroy_children_node(ast_not_data_wrapper);
}


//-----------------------------------------------------------------------------
// ast_destroy TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_setup(void **state) {
    set_ast_destroy_typed_data_wrapper(mock_ast_destroy_typed_data_wrapper);
    set_ast_destroy_children_node(mock_ast_destroy_children_node);
    return 0;
}

static int destroy_teardown(void **state) {
    set_ast_destroy_typed_data_wrapper(NULL);
    set_ast_destroy_children_node(NULL);
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: root = NULL
// Expected: do nothing
static void destroy_do_nothing_when_root_null(void **state) {
    ast_destroy(NULL);
}

// Given: root != NULL, root->type = AST_TYPE_DATA_WRAPPER
// Expected: calls ast_destroy_typed_data_wrapper(root);
static void destroy_calls_ast_destroy_typed_data_wrapper_when_root_is_data_wrapper(void **state) {
    ast *root = NULL;
    alloc_and_save_address_to_be_freed((void **) &root, sizeof(ast));
    root->type = AST_TYPE_DATA_WRAPPER;
    root->data = DUMMY_TYPED_DATA_P;
    expect_value(mock_ast_destroy_typed_data_wrapper, ast_data_wrapper, root);
    ast_destroy(root);
}

// Given: root != NULL, root->type != AST_TYPE_DATA_WRAPPER
// Expected: calls ast_destroy_children_node(root);
static void destroy_calls_ast_destroy_children_node_when_root_is_not_data_wrapper(void **state) {
    ast *root = NULL;
    alloc_and_save_address_to_be_freed((void **) &root, sizeof(ast));
    root->type = AST_TYPE_ADDITION;
    root->children = DUMMY_AST_CHILDREN_T_P;
    expect_value(mock_ast_destroy_children_node, children_node, root);
    ast_destroy(root);
}


//-----------------------------------------------------------------------------
// ast_create_error_node TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_error_node_setup(void **state) {
    memset(too_long_error_message, 'a', MAXIMUM_ERROR_MESSAGE_LENGTH + 1);
    too_long_error_message[MAXIMUM_ERROR_MESSAGE_LENGTH + 1] = '\0';
    set_allocators(mock_malloc, mock_free);
    set_string_duplicate(mock_strdup);
    return 0;
}

static int create_error_node_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
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


// Given: message == NULL
// Expected: returns NULL
static void create_error_node_returns_null_when_message_null(void **state) {
    assert_null(ast_create_error_node(DUMMY_ERROR_TYPE, NULL));
}

// Given: message length exceeds MAXIMUM_ERROR_MESSAGE_LENGTH
// Expected: returns NULL
static void create_error_node_returns_null_when_message_is_too_long(void **state) {
    assert_null(ast_create_error_node(DUMMY_ERROR_TYPE, too_long_error_message));
}

// Given: message is valid
// Expected: calls malloc with sizeof(ast)
static void create_error_node_calls_malloc_for_an_ast_when_message_is_valid(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid some mock calls
    ast_create_error_node(DUMMY_ERROR_TYPE, DUMMY_ERROR_MESSAGE);
}

// Given: malloc fails
// Expected: return NULL
static void create_error_node_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_error_node(DUMMY_ERROR_TYPE, DUMMY_ERROR_MESSAGE));
}

// Given: malloc succeeds
// Expected: calls strdup with message
static void create_error_node_calls_strdup_with_message_when_malloc_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    expect_value(mock_strdup, s, valid_error_message);
    will_return(mock_strdup, STRDUP_ERROR_CODE); // to avoid some mock calls
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_an_ast); // see later
    ast_create_error_node(DUMMY_ERROR_TYPE, valid_error_message);
}

// Given: strdup fails
// Expected: calls free with pointer returned by malloc for an ast and returns null
static void create_error_node_calls_free_and_returns_null_when_strdup_fails(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    expect_value(mock_strdup, s, valid_error_message);
    will_return(mock_strdup, STRDUP_ERROR_CODE);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_an_ast);
    assert_null(ast_create_error_node(DUMMY_ERROR_TYPE, valid_error_message));
}

// Given: strdup succeeds
// Expected: calls malloc with sizeof(error_info)
static void create_error_node_calls_malloc_for_an_error_info_when_strdup_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    alloc_and_save_address_to_be_freed((void **) &fake_strdup_returned_value_for_error_message,
                                       strlen(valid_error_message) + 1);
    expect_value(mock_strdup, s, valid_error_message);
    will_return(mock_strdup, fake_strdup_returned_value_for_error_message);
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_error_info, sizeof(error_info));
    expect_value(mock_malloc, size, sizeof(error_info));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_error_info);
    ast_create_error_node(DUMMY_ERROR_TYPE, valid_error_message);
}

// Given: malloc for an error_info fails
// Expected:
//  - frees return value of strdup
//  - frees malloced ast
//  - returns NULL
static void create_error_node_cleanup_and_return_null_when_malloc_for_error_info_fails(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    alloc_and_save_address_to_be_freed((void **) &fake_strdup_returned_value_for_error_message,
                                       strlen(valid_error_message) + 1);
    expect_value(mock_strdup, s, valid_error_message);
    will_return(mock_strdup, fake_strdup_returned_value_for_error_message);
    //alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_an_error_info, sizeof(error_info));
    expect_value(mock_malloc, size, sizeof(error_info));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, fake_strdup_returned_value_for_error_message);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_an_ast);
    assert_null(ast_create_error_node(DUMMY_ERROR_TYPE, valid_error_message));
}

// Given: strdup succeeds
// Expected: the malloc'ed ast is initialized and returned
static void create_error_node_initializes_and_returns_malloced_ast_when_malloc_for_error_info_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);
    alloc_and_save_address_to_be_freed((void **) &fake_strdup_returned_value_for_error_message,
                                       strlen(valid_error_message) + 1);
    expect_value(mock_strdup, s, valid_error_message);
    will_return(mock_strdup, fake_strdup_returned_value_for_error_message);
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_error_info, sizeof(error_info));
    expect_value(mock_malloc, size, sizeof(error_info));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_error_info);

    ast *ret = ast_create_error_node(DUMMY_ERROR_TYPE, valid_error_message);

    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_ptr_equal(ret->type, AST_TYPE_ERROR);
    assert_ptr_equal(ret->error, fake_malloc_returned_value_for_an_error_info);
    assert_ptr_equal(ret->error->code, DUMMY_ERROR_TYPE);
    assert_ptr_equal(ret->error->message, fake_strdup_returned_value_for_error_message);
    assert_ptr_equal(ret->error->is_sentinel, false);
}


//-----------------------------------------------------------------------------
// ast_destroy_error_node TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_error_node_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **) &ast_error_node, sizeof(ast));
    alloc_and_save_address_to_be_freed((void **) &error, sizeof(error_info));
    ast_error_node->type = AST_TYPE_ERROR;
    error->code = DUMMY_INT;
    error->message = DUMMY_STRING;
    error->is_sentinel = false;
    ast_error_node->error = error;
    AST_ERROR_SENTINEL = ast_error_sentinel();
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int destroy_error_node_teardown(void **state) {
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


// Given: ast_error_node is null
// Expected: does not any side effect
static void destroy_error_node_do_nothing_when_argument_null(void **state) {
    ast_destroy_error_node(NULL);
}

// Given: ast_error_node == AST_ERROR_SENTINEL
// Expected: does not any side effect
static void destroy_error_node_do_nothing_when_argument_is_error_sentinel(void **state) {
    ast_destroy_error_node(AST_ERROR_SENTINEL);
}

// Given: ast_error_node is not null and well-formed
// Expected:
//  - frees ast_error_node->error->message
//  - frees ast_error_node->error
//  - frees ast_error_node
static void destroy_error_node__when_argument_not_null_and_well_formed(void **state) {
    expect_value(mock_free, ptr, ast_error_node->error->message);
    expect_value(mock_free, ptr, ast_error_node->error);
    expect_value(mock_free, ptr, ast_error_node);
    ast_destroy_error_node(ast_error_node);
}


//-----------------------------------------------------------------------------
// ast_create_int_node TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// ast_create_int_node
// ast_create_typed_data_int
// ast_destroy_typed_data_int
// ast_create_typed_data_wrapper

// mocked: memory allocators


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_int_node_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_int_node_teardown(void **state) {
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
//  - i == DUMMY_INT (7)
//  - the allocation of the typed_data will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - returns NULL
static void create_int_node_returns_null_when_first_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);

    assert_ptr_equal(NULL, ast_create_int_node(DUMMY_INT));
}

// Given:
//  - i == DUMMY_INT (7)
//  - the allocation of the typed_data will succeed
//  - the allocation of the ast will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - malloc is called with argument sizeof(ast)
//  - free is called with argument the malloced typed_data
//  - returns NULL
static void create_int_node_cleans_up_and_returns_null_when_second_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_TYPED_DATA_P);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_TYPED_DATA_P);

    assert_ptr_equal(NULL, ast_create_int_node(DUMMY_INT));
}

// Given:
//  - i == DUMMY_INT (7)
//  - allocation of typed_data succeeds
//  - allocation of ast succeeds
// Expected:
//  - malloc is called with sizeof(typed_data)
//  - malloc is called with sizeof(ast)
//  - letting td and res denote the allocated typed_data and ast:
//    - res->type == AST_TYPE_DATA_WRAPPER
//    - res->data == td
//    - td->type == TYPE_INT
//    - td->data.int_value == DUMMY_INT
//  - returns the allocated ast
static void create_int_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_int, sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_int);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);

    ast *res = ast_create_int_node(DUMMY_INT);

    assert_ptr_equal(res, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(res->type, AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(res->data, fake_malloc_returned_value_for_a_typed_data_int);
    assert_int_equal(res->data->type, TYPE_INT);
    assert_int_equal(res->data->data.int_value, DUMMY_INT);
}


//-----------------------------------------------------------------------------
// ast_create_string_node TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// ast_create_string_node
// ast_create_typed_data_string
// ast_destroy_typed_data_string
// ast_create_typed_data_wrapper

// mocked: memory allocators, string duplicator


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_string_node_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    set_string_duplicate(mock_strdup);
    return 0;
}

static int create_string_node_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
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
//  - str == DUMMY_STRING ("dummy string")
//  - the allocation of the typed_data will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - returns NULL
static void create_string_node_returns_null_when_first_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);

    assert_ptr_equal(NULL, ast_create_string_node(DUMMY_STRING));
}

// Given:
//  - str == DUMMY_STRING ("dummy string")
//  - the allocation of the typed_data will succeed
//  - the duplication of the string str will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - strdup is called with argument str
//  - free is called with argument the malloced typed_data
//  - returns NULL
static void create_string_node_cleans_up_and_returns_null_when_strdup_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_TYPED_DATA_P);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, STRDUP_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_TYPED_DATA_P);

    assert_ptr_equal(NULL, ast_create_string_node(DUMMY_STRING));
}

// Given:
//  - str == DUMMY_STRING ("dummy string")
//  - the allocation of the typed_data will succeed
//  - the duplication of the string str will succeed
//  - the allocation of the ast will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - strdup is called with argument str
//  - malloc is called with argument sizeof(ast)
//  - free is called with argument the string copy
//  - free is called with argument the malloced typed_data
//  - returns NULL
static void create_string_node_cleans_up_and_returns_null_when_second_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_TYPED_DATA_P);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_STRDUP_RETURNED_VALUE);
    expect_value(mock_free, ptr, DUMMY_TYPED_DATA_P);

    assert_ptr_equal(NULL, ast_create_string_node(DUMMY_STRING));
}

// Given:
//  - str == DUMMY_STRING ("dummy string")
//  - the allocation of the typed_data will succeed
//  - the duplication of the string str will succeed
//  - the allocation of the ast will succeed
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - strdup is called with argument str
//  - malloc is called with argument sizeof(ast)
//  - letting duplicated, td and ret denote the allocated string, typed_data and ast:
//    - ret->type == AST_TYPE_DATA_WRAPPER
//    - ret->data == td
//    - td->type == TYPE_STRING
//    - td->data.string_value == duplicated
//  - returns the allocated ast
static void create_string_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_string,
                                       sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_string);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);

    ast *ret = ast_create_string_node(DUMMY_STRING);

    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->data, fake_malloc_returned_value_for_a_typed_data_string);
    assert_int_equal(ret->data->type, TYPE_STRING);
    assert_ptr_equal(ret->data->data.string_value, DUMMY_STRDUP_RETURNED_VALUE);
}

// Given:
//  - str == NULL
//  - the allocation of the typed_data will succeed
//  - THERE IS NO DUPLICATION OF THE STRING str
//  - the allocation of the ast will succeed
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - strdup IS NOT CALLED
//  - malloc is called with argument sizeof(ast)
//  - letting td and ret denote the allocated typed_data and ast:
//    - ret->type == AST_TYPE_DATA_WRAPPER
//    - ret->data == td
//    - td->type == TYPE_STRING
//    - td->data.string_value == NULL
//  - returns the allocated ast
static void create_string_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds_and_str_null(
    void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_string,
                                       sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_string);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);

    ast *ret = ast_create_string_node(NULL);

    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->data, fake_malloc_returned_value_for_a_typed_data_string);
    assert_int_equal(ret->data->type, TYPE_STRING);
    assert_ptr_equal(ret->data->data.string_value, NULL);
}


//-----------------------------------------------------------------------------
// ast_create_symbol_name_node TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// ast_create_symbol_name_node
// ast_create_typed_data_symbol_name
// ast_destroy_typed_data_symbol_name
// ast_create_typed_data_wrapper

// mocked: memory allocators, string duplicator


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_symbol_name_node_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    set_string_duplicate(mock_strdup);
    return 0;
}

static int create_symbol_name_node_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
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
//  - str == NULL
// Expected:
//  - returns NULL
static void create_symbol_name_node_returns_null_when_str_null(void **state) {
    assert_ptr_equal(NULL, ast_create_symbol_name_node(NULL));
}

// Given:
//  - str == DUMMY_SYMBOL_NAME ("symbol_name")
//  - the allocation of the typed_data will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - returns NULL
static void create_symbol_name_node_returns_null_when_first_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);

    assert_ptr_equal(NULL, ast_create_symbol_name_node(DUMMY_SYMBOL_NAME));
}

// Given:
//  - str == DUMMY_SYMBOL_NAME ("symbol_name")
//  - the allocation of the typed_data will succeed
//  - the duplication of the string str will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - strdup is called with argument str
//  - free is called with argument the malloced typed_data
//  - returns NULL
static void create_symbol_name_node_cleans_up_and_returns_null_when_strdup_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_TYPED_DATA_P);
    expect_value(mock_strdup, s, DUMMY_SYMBOL_NAME);
    will_return(mock_strdup, STRDUP_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_TYPED_DATA_P);

    assert_ptr_equal(NULL, ast_create_symbol_name_node(DUMMY_SYMBOL_NAME));
}

// Given:
//  - str == DUMMY_SYMBOL_NAME ("symbol_name")
//  - the allocation of the typed_data will succeed
//  - the duplication of the string str will succeed
//  - the allocation of the ast will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - strdup is called with argument str
//  - malloc is called with argument sizeof(ast)
//  - free is called with argument the string copy
//  - free is called with argument the malloced typed_data
//  - returns NULL
static void create_symbol_name_node_cleans_up_and_returns_null_when_second_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_TYPED_DATA_P);
    expect_value(mock_strdup, s, DUMMY_SYMBOL_NAME);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_STRDUP_RETURNED_VALUE);
    expect_value(mock_free, ptr, DUMMY_TYPED_DATA_P);

    assert_ptr_equal(NULL, ast_create_symbol_name_node(DUMMY_SYMBOL_NAME));
}

// Given:
//  - str == DUMMY_SYMBOL_NAME ("symbol_name")
//  - the allocation of the typed_data will succeed
//  - the duplication of the string str will succeed
//  - the allocation of the ast will succeed
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - strdup is called with argument str
//  - malloc is called with argument sizeof(ast)
//  - letting duplicated, td and ret denote the allocated string, typed_data and ast:
//    - ret->type == AST_TYPE_DATA_WRAPPER
//    - ret->data == td
//    - td->type == TYPE_SYMBOL_NAME
//    - td->data.string_value == duplicated
//  - returns the allocated ast
static void create_symbol_name_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_symbol_name,
                                       sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_symbol_name);
    expect_value(mock_strdup, s, DUMMY_SYMBOL_NAME);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);

    ast *ret = ast_create_symbol_name_node(DUMMY_SYMBOL_NAME);

    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->data, fake_malloc_returned_value_for_a_typed_data_symbol_name);
    assert_int_equal(ret->data->type, TYPE_SYMBOL_NAME);
    assert_ptr_equal(ret->data->data.string_value, DUMMY_STRDUP_RETURNED_VALUE);
}


//-----------------------------------------------------------------------------
// ast_create_symbol_node TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// ast_create_symbol_node
// ast_create_typed_data_symbol
// ast_destroy_typed_data_symbol
// ast_create_typed_data_wrapper

// mocked: memory allocators


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_symbol_node_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_symbol_node_teardown(void **state) {
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
//  - sym == NULL
// Expected:
//  - returns NULL
static void create_symbol_node_returns_null_when_sym_null(void **state) {
    assert_ptr_equal(NULL, ast_create_symbol_node(NULL));
}

// Given:
//  - sym == DUMMY_SYMBOL (not NULL)
//  - the allocation of the typed_data will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - returns NULL
static void create_symbol_node_returns_null_when_first_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);

    assert_ptr_equal(NULL, ast_create_symbol_node(DUMMY_SYMBOL));
}

// Given:
//  - sym == DUMMY_SYMBOL (not NULL)
//  - the allocation of the typed_data will succeed
//  - the allocation of the ast will fail
// Expected:
//  - malloc is called with argument sizeof(typed_data)
//  - malloc is called with argument sizeof(ast)
//  - free is called with argument the malloced typed_data
//  - returns NULL
static void create_symbol_node_cleans_up_and_returns_null_when_second_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_TYPED_DATA_P);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, DUMMY_TYPED_DATA_P);

    assert_ptr_equal(NULL, ast_create_symbol_node(DUMMY_SYMBOL));
}

// Given:
//  - sym == DUMMY_SYMBOL (not NULL)
//  - allocation of typed_data succeeds
//  - allocation of ast succeeds
// Expected:
//  - malloc is called with sizeof(typed_data)
//  - malloc is called with sizeof(ast)
//  - letting td and ret denote the allocated typed_data and ast:
//    - ret->type == AST_TYPE_DATA_WRAPPER
//    - ret->data == td
//    - td->type == TYPE_SYMBOL
//    - td->data.symbol_value == DUMMY_SYMBOL
//  - returns the allocated ast
static void create_symbol_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_a_typed_data_symbol,
                                       sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **) &fake_malloc_returned_value_for_an_ast, sizeof(ast));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_symbol);
    expect_value(mock_malloc, size, sizeof(ast));
    will_return(mock_malloc, fake_malloc_returned_value_for_an_ast);

    ast *ret = ast_create_symbol_node(DUMMY_SYMBOL);

    assert_ptr_equal(ret, fake_malloc_returned_value_for_an_ast);
    assert_int_equal(ret->type, AST_TYPE_DATA_WRAPPER);
    assert_ptr_equal(ret->data, fake_malloc_returned_value_for_a_typed_data_symbol);
    assert_int_equal(ret->data->type, TYPE_SYMBOL);
    assert_ptr_equal(ret->data->data.symbol_value, DUMMY_SYMBOL);
}


//-----------------------------------------------------------------------------
// ast_create_error_node_or_sentinel TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// ast_create_error_node_or_sentinel
// ast_create_error_node
// ast_error_sentinel

// doubled: memory allocator, string duplicator


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_error_node_or_sentinel_setup(void **state) {
    memset(too_long_error_message, 'a', MAXIMUM_ERROR_MESSAGE_LENGTH + 1);
    too_long_error_message[MAXIMUM_ERROR_MESSAGE_LENGTH + 1] = '\0';
    fake_heap_reset();
    return 0;
}

static int create_error_node_or_sentinel_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    fake_heap_reset();
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every tests
// code == DUMMY_ERROR_TYPE


// Given:
//  - message == NULL
// Expected:
//  - returns ast_error_sentinel()
static void create_error_node_or_sentinel_returns_sentinel_when_message_null(void **state) {
    assert_ptr_equal(ast_error_sentinel(), ast_create_error_node_or_sentinel(DUMMY_ERROR_TYPE, NULL));
}

// Given:
//  - message length exceeds MAXIMUM_ERROR_MESSAGE_LENGTH
// Expected:
//  - returns ast_error_sentinel()
static void create_error_node_returns_sentinel_when_message_is_too_long(void **state) {
    assert_ptr_equal(ast_error_sentinel(), ast_create_error_node_or_sentinel(DUMMY_ERROR_TYPE, too_long_error_message));
}

// Given:
//  - message is valid
//  - memory allocator fails
//  - string duplication succeeds
// Expected:
//  - returns ast_error_sentinel()
static void create_error_node_returns_sentinel_when_malloc_fails(void **state) {
    set_allocators(fake_broken_malloc, fake_broken_free);
    set_string_duplicate(fake_operational_strdup);

    assert_ptr_equal(ast_error_sentinel(), ast_create_error_node_or_sentinel(DUMMY_ERROR_TYPE, DUMMY_ERROR_MESSAGE));
}

// Given:
//  - message is valid
//  - memory allocator succeeds
//  - string duplication fails
// Expected:
//  - returns ast_error_sentinel()
static void create_error_node_returns_sentinel_when_strdup_fails(void **state) {
    set_allocators(fake_operational_malloc, fake_operational_free);
    set_string_duplicate(fake_broken_strdup);

    assert_ptr_equal(ast_error_sentinel(), ast_create_error_node_or_sentinel(DUMMY_ERROR_TYPE, DUMMY_ERROR_MESSAGE));
}

// Given:
//  - message is valid
//  - memory allocator fails
//  - string duplication fails
// Expected:
//  - returns ast_error_sentinel()
static void create_error_node_returns_sentinel_when_strdup_and_malloc_fails(void **state) {
    set_allocators(fake_broken_malloc, fake_broken_free);
    set_string_duplicate(fake_broken_strdup);

    assert_ptr_equal(ast_error_sentinel(), ast_create_error_node_or_sentinel(DUMMY_ERROR_TYPE, DUMMY_ERROR_MESSAGE));
}

// Given:
//  - message == DUMMY_ERROR_MESSAGE
//  - memory allocator succeeds
//  - string duplication succeeds
// Expected:
// - returns an ast pointer ret initialized as follows:
//   - ret->type == AST_TYPE_ERROR
//   - ret->error->code == DUMMY_ERROR_TYPE
//   - ret->error->message == a copy of DUMMY_ERROR_MESSAGE
//   - ret->error->is_sentinel == false
static void create_error_node_initializes_and_returns_error_node_when_strdup_and_malloc_succeeds(void **state) {
    set_allocators(fake_operational_malloc, fake_operational_free);
    set_string_duplicate(fake_operational_strdup);

    ast *ret = ast_create_error_node_or_sentinel(DUMMY_ERROR_TYPE, DUMMY_ERROR_MESSAGE);

    assert_non_null(ret);
    assert_non_null(ret->error);
    assert_non_null(ret->error->message);
    assert_int_equal(ret->type, AST_TYPE_ERROR);
    assert_int_equal(ret->error->code, DUMMY_ERROR_TYPE);
    assert_memory_equal(DUMMY_ERROR_MESSAGE, ret->error->message, strlen(DUMMY_ERROR_MESSAGE) + 1);
    assert_int_equal(ret->error->is_sentinel, false);
}


//-----------------------------------------------------------------------------
// ast_children_reserve TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// ast_children_reserve

// mocked:
//  - memory reallocator


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int children_reserve_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **) &ast_children_p, sizeof(ast_children_t));
    ast_children_p->children_nb = 2;
    ast_children_p->capacity = 7;
    alloc_and_save_address_to_be_freed(
        (void **) &ast_children_p->children,
        ast_children_p->children_nb * sizeof(ast *));
    (ast_children_p->children)[0] = DUMMY_AST_CHILD_0;
    (ast_children_p->children)[1] = DUMMY_AST_CHILD_1;
    set_reallocator(mock_realloc);
    return 0;
}

static int children_reserve_teardown(void **state) {
    set_reallocator(NULL);
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


// At every tests but the first:
// Given:
//  - ast_children == ast_children_p initializes in setup such as:
//    - ast_children_p->children_nb == 2
//    - ast_children_p->capacity == 7
//    - (ast_children_p->children)[0] == DUMMY_AST_CHILD_0;
//    - (ast_children_p->children)[1] == DUMMY_AST_CHILD_1;

// Given:
//  - ast_children == NULL
// Expected:
//  - returns false
static void children_reserve_returns_false_when_ast_children_null(void **state) {
    assert_int_equal(false, ast_children_reserve(NULL, DUMMY_CAPACITY));
}

// Given:
//  - capacity == ast_children_p->children_nb - 1
// Expected:
//  - returns false
static void children_reserve_returns_false_when_insufficient_capacity(void **state) {
    assert_int_equal(
        false,
        ast_children_reserve(
            ast_children_p,
            ast_children_p->children_nb - 1));
}

// Given:
//  - capacity >= ast_children_p->children_nb
//  - capacity <= ast_children_p->capacity
// Expected:
//  - MEMORY REALLOCATOR WILL NOT BE CALLED
//  - NO SIDE EFFECT ; in particular:
//    - ast_children_p->capacity DOES NOT CHANGE
//    - ast_children_p->children DOES NOT CHANGE
//  - returns true
static void children_reserve_no_side_effect_and_returns_true_when_capacity_sufficient_and_lesser_than_before(
    void **state) {
    size_t old_capacity = ast_children_p->capacity;
    ast **old_children = ast_children_p->children;
    assert_int_equal(
        true,
        ast_children_reserve(
            ast_children_p,
            ast_children_p->children_nb + 1));
    assert_int_equal(
        ast_children_p->capacity,
        old_capacity);
    assert_ptr_equal(
        ast_children_p->children,
        old_children);
}

// Given:
//  - capacity > ast_children_p->capacity
//  - realloc will fail
// Expected:
//  - calls realloc with:
//    - ptr: ast_children_p->children
//    - size: capacity * sizeof(ast *)
//  - NO SIDE EFFECT ; in particular:
//    - ast_children_p->capacity DOES NOT CHANGE
//    - ast_children_p->children DOES NOT CHANGE
//  - returns false
static void children_reserve_no_side_effect_and_returns_false_when_realloc_fails(void **state) {
    size_t old_capacity = ast_children_p->capacity;
    ast **old_children = ast_children_p->children;
    expect_value(mock_realloc, ptr, ast_children_p->children);
    expect_value(
        mock_realloc,
        size,
        (ast_children_p->capacity + 1) * sizeof(ast *));
    will_return(mock_realloc, NULL);
    assert_int_equal(
        false,
        ast_children_reserve(
            ast_children_p,
            ast_children_p->capacity + 1));
    assert_int_equal(
        ast_children_p->capacity,
        old_capacity);
    assert_ptr_equal(
        ast_children_p->children,
        old_children);
}

// Given:
//  - capacity > ast_children_p->capacity
//  - realloc will succeed
// Expected:
//  - calls realloc with:
//    - ptr: ast_children_p->children
//    - size: capacity * sizeof(ast *)
//  - ast_children_p->capacity == capacity
//  - ast_children_p->children == realloc returned value
//  - returns true
static void children_reserve_reallocates_children_update_capacity_and_returns_true_when_realloc_succeeds(void **state) {
    size_t capacity_param = ast_children_p->capacity + 1;
    expect_value(mock_realloc, ptr, ast_children_p->children);
    expect_value(
        mock_realloc,
        size,
        capacity_param * sizeof(ast *));
    will_return(mock_realloc, DUMMY_AST_CHILDREN);
    assert_int_equal(
        true,
        ast_children_reserve(
            ast_children_p,
            capacity_param));
    assert_int_equal(
        ast_children_p->capacity,
        capacity_param);
    assert_ptr_equal(
        ast_children_p->children,
        DUMMY_AST_CHILDREN);
}


//-----------------------------------------------------------------------------
// ast_children_append_take TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// ast_children_append_take
// ast_children_reserve

// mocked:
//  - memory reallocator


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int children_append_take_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **) &children_node_p, sizeof(ast));
    children_node_p->type = AST_TYPE_TRANSLATION_UNIT;
    alloc_and_save_address_to_be_freed((void **) &children_node_p->children, sizeof(ast_children_t));
    children_node_p->children->children_nb = 2;
    children_node_p->children->capacity = 2;
    alloc_and_save_address_to_be_freed((void **) &children_node_p->children->children, 3 * sizeof(ast *));
    (children_node_p->children->children)[0] = DUMMY_AST_CHILD_0;
    (children_node_p->children->children)[1] = DUMMY_AST_CHILD_1;
    set_reallocator(mock_realloc);
    return 0;
}

static int children_append_take_teardown(void **state) {
    set_reallocator(NULL);
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


// At every tests:
// Given:
//   - when child not mentionned NULL:
//     - child == DUMMY_AST_CHILD_TO_BE_ADDED
//   - when parent not mentionned in "Given":
//     - parent == children_node_p
//       - parent->type == AST_TYPE_TRANSLATION_UNIT
//         - parent->children->children_nb == 2
//         - parent->children->capacity
//                   == 2 IF NOT PUT TO 3 IN TEST
//           - (parent->children->children)[0] == DUMMY_AST_CHILD_0
//           - (parent->children->children)[1] == DUMMY_AST_CHILD_1
//           - the next slot is yet allocated by the system in setup,
//             ie capacity can move to 3,
//             ie no segfaut with (parent->children->children)[2]
//   - params are NULL or well-formed as the client has to ensure ;
//     in particular children_nb and capacity fields are valid

// Given:
//   - parent == NULL
// Expected:
//   - MEMORY REALLOCATOR WILL NOT BE CALLED
//   - returns false
static void children_append_take_returns_false_when_parent_null(void **state) {
    assert_false(ast_children_append_take(NULL, DUMMY_AST_CHILD_TO_BE_ADDED));
}

// Given:
//   - child == NULL
// Expected:
//   - MEMORY REALLOCATOR WILL NOT BE CALLED
//   - no side effect
//   - returns false
static void children_append_take_does_nothing_and_returns_false_when_child_null(void **state) {
    ast_type old_type = children_node_p->type;
    ast_children_t *old_children_info = children_node_p->children;
    size_t old_children_nb = children_node_p->children->children_nb;
    size_t old_capacity = children_node_p->children->capacity;
    ast **old_children = children_node_p->children->children;
    ast *old_child_0 = old_children[0];
    ast *old_child_1 = old_children[1];
    assert_false(ast_children_append_take(children_node_p, NULL));
    assert_int_equal(old_type, children_node_p->type);
    assert_ptr_equal(old_children_info, children_node_p->children);
    assert_int_equal(old_children_nb, children_node_p->children->children_nb);
    assert_int_equal(old_capacity, children_node_p->children->capacity);
    assert_ptr_equal(old_children, children_node_p->children->children);
    assert_ptr_equal(old_child_0, old_children[0]);
    assert_ptr_equal(old_child_1, old_children[1]);
}

// Given:
//   - parent->type == AST_TYPE_DATA_WRAPPER
//     OR
//   - parent->type == AST_TYPE_ERROR
// Expected:
//   - MEMORY REALLOCATOR WILL NOT BE CALLED
//   - no side effect
//   - returns false
static void children_append_take_does_nothing_and_returns_false_when_parent_not_a_child_node(void **state) {
    ast *ast_data_wrapper = NULL;
    alloc_and_save_address_to_be_freed((void **) &ast_data_wrapper, sizeof(ast));
    ast_data_wrapper->type = AST_TYPE_DATA_WRAPPER;
    ast_data_wrapper->data = DUMMY_TYPED_DATA_P;
    ast *ast_error_node = NULL;
    alloc_and_save_address_to_be_freed((void **) &ast_error_node, sizeof(ast));
    ast_error_node->type = AST_TYPE_ERROR;
    ast_error_node->error = DUMMY_ERROR_INFO_P;

    assert_false(ast_children_append_take(ast_data_wrapper, DUMMY_AST_CHILD_TO_BE_ADDED));
    assert_false(ast_children_append_take(ast_error_node, DUMMY_AST_CHILD_TO_BE_ADDED));
    assert_int_equal(ast_data_wrapper->type, AST_TYPE_DATA_WRAPPER);
    assert_int_equal(ast_error_node->type, AST_TYPE_ERROR);
    assert_ptr_equal(ast_data_wrapper->data, DUMMY_TYPED_DATA_P);
    assert_ptr_equal(ast_error_node->error, DUMMY_ERROR_INFO_P);
}

// Given:
//   - parent->children->capacity > parent->children->children_nb
// Expected:
//   - MEMORY REALLOCATOR WILL NOT BE CALLED
//   - parent->children->children_nb == 3
//   - (parent->children->children)[2] == DUMMY_AST_CHILD_TO_BE_ADDED
//   - no other side effect
//   - returns true
static void children_append_take_append_child_and_returns_true_when_capacity_greater_than_children_nb(void **state) {
    children_node_p->children->capacity = 3; // see paragraph "At every tests" written before these tests

    ast_type old_type = children_node_p->type;
    ast_children_t *old_children_info = children_node_p->children;
    size_t old_children_nb = children_node_p->children->children_nb;
    size_t old_capacity = children_node_p->children->capacity;
    ast **old_children = children_node_p->children->children;
    ast *old_child_0 = old_children[0];
    ast *old_child_1 = old_children[1];

    assert_true(ast_children_append_take(children_node_p, DUMMY_AST_CHILD_TO_BE_ADDED));
    assert_int_equal(old_type, children_node_p->type);
    assert_ptr_equal(old_children_info, children_node_p->children);
    assert_int_equal(
        children_node_p->children->children_nb,
        old_children_nb + 1);
    assert_int_equal(old_capacity, children_node_p->children->capacity);
    assert_ptr_equal(old_children, children_node_p->children->children);
    assert_ptr_equal(old_children[0], old_child_0);
    assert_ptr_equal(old_children[1], old_child_1);
    assert_ptr_equal(
        old_children[2],
        DUMMY_AST_CHILD_TO_BE_ADDED);
}

// Given:
//   - parent->children->capacity == parent->children->children_nb
//   - realloc will fail
// Expected:
//   - calls realloc with:
//     - ptr: parent->children->children
//     - size: (1 + 2 * parent->children->capacity) * sizeof(ast *)
//   - no side effect
//   - returns false
static void children_append_take_returns_false_when_realloc_fails(void **state) {
    size_t wanted_capacity = 1 + 2 * children_node_p->children->capacity;
    expect_value(mock_realloc, ptr, children_node_p->children->children);
    expect_value(
        mock_realloc,
        size,
        wanted_capacity * sizeof(ast *));
    will_return(mock_realloc, NULL);
    ast_type old_type = children_node_p->type;
    ast_children_t *old_children_info = children_node_p->children;
    size_t old_children_nb = children_node_p->children->children_nb;
    size_t old_capacity = children_node_p->children->capacity;
    ast **old_children = children_node_p->children->children;
    ast *old_child_0 = old_children[0];
    ast *old_child_1 = old_children[1];

    assert_false(ast_children_append_take(children_node_p, DUMMY_AST_CHILD_TO_BE_ADDED));
    assert_int_equal(old_type, children_node_p->type);
    assert_ptr_equal(old_children_info, children_node_p->children);
    assert_int_equal(old_children_nb, children_node_p->children->children_nb);
    assert_int_equal(old_capacity, children_node_p->children->capacity);
    assert_ptr_equal(old_children, children_node_p->children->children);
    assert_ptr_equal(old_child_0, old_children[0]);
    assert_ptr_equal(old_child_1, old_children[1]);
}

// Given:
//   - parent->children->capacity == parent->children->children_nb
//   - realloc will succeed
// Expected:
//   - calls realloc with:
//     - ptr: parent->children->children
//     - size: (1 + 2 * parent->children->capacity) * sizeof(ast *)
//   - parent->children->capacity *= 2
//   - parent->children->children_nb += 1
//   - parent->children->children == realloc returned value
//   - no other side effect
//   - returns true
static void children_append_take_append_child_returns_true_when_realloc_succeeds(void **state) {
    size_t wanted_capacity = 1 + 2 * children_node_p->children->capacity;
    alloc_and_save_address_to_be_freed(
        (void **) &fake_realloc_returned_value,
        wanted_capacity * sizeof(ast *));
    expect_value(mock_realloc, ptr, children_node_p->children->children);
    expect_value(mock_realloc, size, wanted_capacity * sizeof(ast *));
    will_return(mock_realloc, fake_realloc_returned_value);
    ast_type old_type = children_node_p->type;
    ast_children_t *old_children_info = children_node_p->children;
    size_t old_children_nb = children_node_p->children->children_nb;
    size_t old_capacity = children_node_p->children->capacity;
    ast **old_children = children_node_p->children->children;
    ast *old_child_0 = old_children[0];
    ast *old_child_1 = old_children[1];

    assert_true(ast_children_append_take(children_node_p, DUMMY_AST_CHILD_TO_BE_ADDED));
    assert_int_equal(old_type, children_node_p->type);
    assert_ptr_equal(old_children_info, children_node_p->children);
    assert_int_equal(
        children_node_p->children->children_nb,
        old_children_nb + 1);
    assert_int_equal(
        children_node_p->children->capacity,
        1 + old_capacity * 2);
    assert_ptr_equal(
        children_node_p->children->children,
        fake_realloc_returned_value);
    assert_ptr_equal(old_child_0, old_children[0]);
    assert_ptr_equal(old_child_1, old_children[1]);
    assert_ptr_equal(
        (children_node_p->children->children)[2],
        DUMMY_AST_CHILD_TO_BE_ADDED);
}


//-----------------------------------------------------------------------------
// ast_type_has_children TESTS
//-----------------------------------------------------------------------------


// Given:
//   - a container type
// Expected:
//   - return true
static void ast_type_has_children_returns_true_when_container_type(void **state) {
    assert_true(ast_type_has_children(AST_TYPE_BINDING));
    assert_true(ast_type_has_children(AST_TYPE_READING));
    assert_true(ast_type_has_children(AST_TYPE_WRITING));
    assert_true(ast_type_has_children(AST_TYPE_TRANSLATION_UNIT));
    assert_true(ast_type_has_children(AST_TYPE_COMPUTATION));
    assert_true(ast_type_has_children(AST_TYPE_ADDITION));
}

// Given:
//   - a leaf type
// Expected:
//   - return false
static void ast_type_has_children_returns_false_when_leaf_type(void **state) {
    assert_false(ast_type_has_children(AST_TYPE_DATA_WRAPPER));
    assert_false(ast_type_has_children(AST_TYPE_ERROR));
}


//-----------------------------------------------------------------------------
// ast_can_have_children TESTS
//-----------------------------------------------------------------------------


// Given:
//   - a == NULL
// Expected:
//   - return false
static void can_have_children_returns_false_when_a_null(void **state) {
    assert_false(ast_can_have_children(NULL));
}

// Given:
//   - an ast of type AST_TYPE_TRANSLATION_UNIT type with 0 child
// Expected:
//   - return true
static void can_have_children_returns_true_when_container_type(void **state) {
    ast *a = malloc(sizeof(ast));
    assert_non_null(a);
    a->type = AST_TYPE_TRANSLATION_UNIT;
    a->children = malloc(sizeof(ast_children_t));
    assert_non_null(a->children);
    a->children->children_nb = 0;
    a->children->capacity = 0;
    a->children->children = NULL;

    assert_true(ast_can_have_children(a));

    free(a->children);
    free(a);
}

// Given:
//   - an number data wrapper ast
// Expected:
//   - return false
static void can_have_children_returns_false_when_data_wrapper_ast(void **state) {
    ast *a = malloc(sizeof(ast));
    assert_non_null(a);
    a->type = AST_TYPE_DATA_WRAPPER;
    a->data = malloc(sizeof(typed_data));
    assert_non_null(a->data);
    a->data->type = TYPE_INT;
    a->data->data.int_value = 7;

    assert_false(ast_can_have_children(a));

    free(a->data);
    free(a);
}


//-----------------------------------------------------------------------------
// ast_has_any_child TESTS
//-----------------------------------------------------------------------------


// Given:
//   - a == NULL
// Expected:
//   - return false
static void has_any_child_returns_false_when_a_null(void **state) {
    assert_false(ast_has_any_child(NULL));
}

// Given:
//   - an ast of type AST_TYPE_TRANSLATION_UNIT type with 0 child
// Expected:
//   - return false
static void has_any_child_returns_false_when_container_type_with_no_child(void **state) {
    ast *a = malloc(sizeof(ast));
    assert_non_null(a);
    a->type = AST_TYPE_TRANSLATION_UNIT;
    a->children = malloc(sizeof(ast_children_t));
    assert_non_null(a->children);
    a->children->children_nb = 0;
    a->children->capacity = 0;
    a->children->children = NULL;

    assert_false(ast_has_any_child(a));

    free(a->children);
    free(a);
}

// Given:
//   - an number data wrapper ast
// Expected:
//   - return false
static void ast_has_any_child_returns_false_when_data_wrapper_ast(void **state) {
    ast *a = malloc(sizeof(ast));
    assert_non_null(a);
    a->type = AST_TYPE_DATA_WRAPPER;
    a->data = malloc(sizeof(typed_data));
    assert_non_null(a->data);
    a->data->type = TYPE_INT;
    a->data->data.int_value = 7;

    assert_false(ast_has_any_child(a));

    free(a->data);
    free(a);
}


//-----------------------------------------------------------------------------
// ast_is_data_of TESTS
//-----------------------------------------------------------------------------


// Given:
//   - a == NULL
// Expected:
//   - return false
static void is_data_of_returns_false_when_a_null(void **state) {
    assert_false(ast_is_data_of(NULL, STUB_DATA_TYPE));
}

// Given:
//   - a == an ast of type AST_TYPE_TRANSLATION_UNIT type with 0 child
// Expected:
//   - return false
static void is_data_of_returns_false_when_a_is_container_type(void **state) {
    ast *a = malloc(sizeof(ast));
    assert_non_null(a);
    a->type = AST_TYPE_TRANSLATION_UNIT;
    a->children = malloc(sizeof(ast_children_t));
    assert_non_null(a->children);
    a->children->children_nb = 0;
    a->children->capacity = 0;
    a->children->children = NULL;

    assert_false(ast_is_data_of(a, STUB_DATA_TYPE));

    free(a->children);
    free(a);
}

// Given:
//   - a == a number data wrapper ast
//   - dt == TYPE_SYMBOL_NAME
// Expected:
//   - return false
static void ast_is_data_of_returns_false_when_type_mismatch(void **state) {
    ast *a = malloc(sizeof(ast));
    assert_non_null(a);
    a->type = AST_TYPE_DATA_WRAPPER;
    a->data = malloc(sizeof(typed_data));
    assert_non_null(a->data);
    a->data->type = TYPE_INT;
    a->data->data.int_value = 7;

    assert_false(ast_is_data_of(a, TYPE_SYMBOL_NAME));

    free(a->data);
    free(a);
}

// Given:
//   - a == a number data wrapper ast
//   - dt == TYPE_INT
// Expected:
//   - return true
static void ast_is_data_of_returns_true_when_type_match(void **state) {
    ast *a = malloc(sizeof(ast));
    assert_non_null(a);
    a->type = AST_TYPE_DATA_WRAPPER;
    a->data = malloc(sizeof(typed_data));
    assert_non_null(a->data);
    a->data->type = TYPE_INT;
    a->data->data.int_value = 7;

    assert_true(ast_is_data_of(a, TYPE_INT));

    free(a->data);
    free(a);
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
            ast_create_typed_data_string_initializes_and_returns_malloced_typed_data_when_malloc_succeeds_and_s_null,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_calls_strdup_when_malloc_succeeds_and_s_not_null,
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
            destroy_typed_data_wrapper_setup, destroy_typed_data_wrapper_teardown,
            (void *) &destroy_typed_data_wrapper_not_a_data_wrapper),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper,
            destroy_typed_data_wrapper_setup, destroy_typed_data_wrapper_teardown,
            (void *) &destroy_typed_data_wrapper_params_template_int),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper,
            destroy_typed_data_wrapper_setup, destroy_typed_data_wrapper_teardown,
            (void *) &destroy_typed_data_wrapper_params_template_string),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper,
            destroy_typed_data_wrapper_setup, destroy_typed_data_wrapper_teardown,
            (void *) &destroy_typed_data_wrapper_params_template_symbol),
    };

    const struct CMUnitTest ast_create_ast_children_arr_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_calls_malloc_for_an_ast_children_t,
            create_ast_children_arr_setup, create_ast_children_arr_teardown,
            (void *) &create_ast_children_arr_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_calls_malloc_for_an_ast_children_t,
            create_ast_children_arr_setup, create_ast_children_arr_teardown,
            (void *) &create_ast_children_arr_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_calls_malloc_for_an_ast_children_t,
            create_ast_children_arr_setup, create_ast_children_arr_teardown,
            (void *) &create_ast_children_arr_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_return_null_when_malloc_fails,
            create_ast_children_arr_setup, create_ast_children_arr_teardown,
            (void *) &create_ast_children_arr_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_return_null_when_malloc_fails,
            create_ast_children_arr_setup, create_ast_children_arr_teardown,
            (void *) &create_ast_children_arr_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_initializes_and_returns_malloced_ast_children_t_when_malloc_succeds,
            create_ast_children_arr_setup, create_ast_children_arr_teardown,
            (void *) &create_ast_children_arr_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_arr_initializes_and_returns_malloced_ast_children_t_when_malloc_succeds,
            create_ast_children_arr_setup, create_ast_children_arr_teardown,
            (void *) &create_ast_children_arr_params_template_two_children),
    };

    const struct CMUnitTest ast_create_ast_children_var_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_an_ast_children_t,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_an_ast_children_t,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_an_ast_children_t,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_return_null_when_malloc_fails,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_return_null_when_malloc_fails,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_a_double_pointer_of_ast_when_malloc_for_an_ast_children_t_succeeds,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_malloc_for_a_double_pointer_of_ast_when_malloc_for_an_ast_children_t_succeeds,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_free_with_pointer_for_ast_children_t_when_malloc_for_double_pointer_of_ast_fails,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_calls_free_with_pointer_for_ast_children_t_when_malloc_for_double_pointer_of_ast_fails,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_initializes_and_returns_malloced_ast_children_t_when_malloc_for_double_pointer_of_ast_succeeds_and_one_arg,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_ast_children_var_initializes_and_returns_malloced_ast_children_t_when_malloc_for_double_pointer_of_ast_succeeds_and_two_args,
            create_ast_children_var_setup, create_ast_children_var_teardown,
            (void *) &create_ast_children_var_params_template_two_children),
    };

    const struct CMUnitTest ast_destroy_ast_children_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            destroy_ast_children_do_nothing_when_argument_null,
            destroy_ast_children_setup, destroy_ast_children_teardown, (void *) &destroy_ast_children_params_null),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_ast_children_calls_destroy_properly_when_one_child_which_is_data_wrapper,
            destroy_ast_children_setup, destroy_ast_children_teardown,
            (void *) &destroy_ast_children_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_ast_children_calls_destroy_properly_when_two_children_one_data_wrapper_other_not,
            destroy_ast_children_setup, destroy_ast_children_teardown,
            (void *) &destroy_ast_children_params_template_two_child),
    };

    const struct CMUnitTest ast_create_children_node_tests[] = {
        cmocka_unit_test(create_children_node_returns_null_when_type_data_wrapper_or_unsupported),
        cmocka_unit_test(create_children_node_returns_null_when_ast_children_null),
        cmocka_unit_test_setup_teardown(
            create_children_node_returns_null_when_malloc_fails,
            create_children_node_setup, create_children_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_children_node_initializes_and_returns_malloced_ast_when_malloc_succeeds,
            create_children_node_setup, create_children_node_teardown),
    };

    const struct CMUnitTest ast_create_children_node_arr_tests[] = {
        cmocka_unit_test(create_children_node_arr_returns_null_when_type_data_wrapper_or_unsupported),
        cmocka_unit_test_setup_teardown(
            create_children_node_arr_calls_ast_create_ast_children_arr_with_last_args_when_type_supported_and_not_data_wrapper,
            create_children_node_arr_setup, create_children_node_arr_teardown),
        cmocka_unit_test_setup_teardown(
            create_children_node_arr_returns_null_when_ast_create_ast_children_arr_fails,
            create_children_node_arr_setup, create_children_node_arr_teardown),
        cmocka_unit_test_setup_teardown(
            create_children_node_arr_calls_malloc_for_an_ast_when_ast_create_ast_children_arr_succeeds,
            create_children_node_arr_setup, create_children_node_arr_teardown),
        cmocka_unit_test_setup_teardown(
            create_children_node_arr_calls_free_and_returns_null_when_malloc_fails,
            create_children_node_arr_setup, create_children_node_arr_teardown),
        cmocka_unit_test_setup_teardown(
            create_children_node_arr_initializes_and_returns_malloced_ast_when_malloc_succeeds,
            create_children_node_arr_setup, create_children_node_arr_teardown),
    };

    const struct CMUnitTest ast_create_children_node_var_tests[] = {
        cmocka_unit_test(create_children_node_var_returns_null_when_type_data_wrapper_or_unsupported),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_calls_malloc_for_an_ast_when_type_supported_and_not_data_wrapper,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_calls_malloc_for_an_ast_when_type_supported_and_not_data_wrapper,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_calls_malloc_for_an_ast_when_type_supported_and_not_data_wrapper,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_returns_null_when_malloc_for_an_ast_fails,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_returns_null_when_malloc_for_an_ast_fails,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_returns_null_when_malloc_for_an_ast_fails,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_calls_ast_create_ast_children_arr_when_malloc_for_an_ast_succeeds_and_children_nb_0,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_calls_free_and_returns_null_when_ast_create_ast_children_arr_fails_and_children_nb_0,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_initializes_and_returns_malloced_ast_when_ast_create_ast_children_arr_succeeds_and_children_nb_0,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_no_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_calls_malloc_for_an_array_of_pointer_of_ast_when_malloc_for_an_ast_succeeds_and_children_nb_not_0,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_calls_malloc_for_an_array_of_pointer_of_ast_when_malloc_for_an_ast_succeeds_and_children_nb_not_0,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_calls_free_and_returns_null_when_malloc_for_an_array_of_pointer_of_ast_fails,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_calls_free_and_returns_null_when_malloc_for_an_array_of_pointer_of_ast_fails,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_initializes_ast_p_array_and_calls_ast_create_ast_children_arr_when_malloc_for_ast_p_array_succeeds,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_initializes_ast_p_array_and_calls_ast_create_ast_children_arr_when_malloc_for_ast_p_array_succeeds,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_frees_two_malloced_pointers_when_ast_create_ast_children_arr_fails,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_frees_two_malloced_pointers_when_ast_create_ast_children_arr_fails,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_two_children),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_initializes_and_returns_malloced_ast_when_ast_create_ast_children_arr_succeeds,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_one_child),
        cmocka_unit_test_prestate_setup_teardown(
            create_children_node_var_initializes_and_returns_malloced_ast_when_ast_create_ast_children_arr_succeeds,
            create_children_node_var_setup, create_children_node_var_teardown,
            (void *) &create_children_node_var_params_template_two_children),
    };

    const struct CMUnitTest ast_destroy_children_node_tests[] = {
        cmocka_unit_test(destroy_children_node_do_nothing_when_arg_null),
        cmocka_unit_test_setup_teardown(
            destroy_children_node_do_nothing_when_arg_is_typed_data_wrapper,
            destroy_children_node_setup, destroy_children_node_teardown),
        cmocka_unit_test_setup_teardown(
            destroy_children_node_calls_ast_destroy_ast_children_and_calls_free_when_arg_is_valid,
            destroy_children_node_setup, destroy_children_node_teardown),
    };

    const struct CMUnitTest ast_destroy_tests[] = {
        cmocka_unit_test_setup_teardown(
            destroy_do_nothing_when_root_null,
            destroy_setup, destroy_teardown),
        cmocka_unit_test_setup_teardown(
            destroy_calls_ast_destroy_typed_data_wrapper_when_root_is_data_wrapper,
            destroy_setup, destroy_teardown),
        cmocka_unit_test_setup_teardown(
            destroy_calls_ast_destroy_children_node_when_root_is_not_data_wrapper,
            destroy_setup, destroy_teardown),
    };

    const struct CMUnitTest ast_create_error_node_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_error_node_returns_null_when_message_null,
            create_error_node_setup, create_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_returns_null_when_message_is_too_long,
            create_error_node_setup, create_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_calls_malloc_for_an_ast_when_message_is_valid,
            create_error_node_setup, create_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_returns_null_when_malloc_fails,
            create_error_node_setup, create_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_calls_strdup_with_message_when_malloc_succeeds,
            create_error_node_setup, create_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_calls_free_and_returns_null_when_strdup_fails,
            create_error_node_setup, create_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_calls_malloc_for_an_error_info_when_strdup_succeeds,
            create_error_node_setup, create_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_cleanup_and_return_null_when_malloc_for_error_info_fails,
            create_error_node_setup, create_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_initializes_and_returns_malloced_ast_when_malloc_for_error_info_succeeds,
            create_error_node_setup, create_error_node_teardown),
    };

    const struct CMUnitTest ast_destroy_error_node_tests[] = {
        cmocka_unit_test_setup_teardown(
            destroy_error_node_do_nothing_when_argument_null,
            destroy_error_node_setup, destroy_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            destroy_error_node_do_nothing_when_argument_is_error_sentinel,
            destroy_error_node_setup, destroy_error_node_teardown),
        cmocka_unit_test_setup_teardown(
            destroy_error_node__when_argument_not_null_and_well_formed,
            destroy_error_node_setup, destroy_error_node_teardown),
    };

    const struct CMUnitTest ast_create_int_node_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_int_node_returns_null_when_first_malloc_fails,
            create_int_node_setup, create_int_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_int_node_cleans_up_and_returns_null_when_second_malloc_fails,
            create_int_node_setup, create_int_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_int_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds,
            create_int_node_setup, create_int_node_teardown),
    };

    const struct CMUnitTest ast_create_string_node_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_string_node_returns_null_when_first_malloc_fails,
            create_string_node_setup, create_string_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_string_node_cleans_up_and_returns_null_when_strdup_fails,
            create_string_node_setup, create_string_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_string_node_cleans_up_and_returns_null_when_second_malloc_fails,
            create_string_node_setup, create_string_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_string_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds,
            create_string_node_setup, create_string_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_string_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds_and_str_null,
            create_string_node_setup, create_string_node_teardown),
    };

    const struct CMUnitTest ast_create_symbol_name_node_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_symbol_name_node_returns_null_when_str_null,
            create_symbol_name_node_setup, create_symbol_name_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_symbol_name_node_returns_null_when_first_malloc_fails,
            create_symbol_name_node_setup, create_symbol_name_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_symbol_name_node_cleans_up_and_returns_null_when_strdup_fails,
            create_symbol_name_node_setup, create_symbol_name_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_symbol_name_node_cleans_up_and_returns_null_when_second_malloc_fails,
            create_symbol_name_node_setup, create_symbol_name_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_symbol_name_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds,
            create_symbol_name_node_setup, create_symbol_name_node_teardown),
    };

    const struct CMUnitTest ast_create_symbol_node_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_symbol_node_returns_null_when_sym_null,
            create_symbol_node_setup, create_symbol_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_symbol_node_returns_null_when_first_malloc_fails,
            create_symbol_node_setup, create_symbol_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_symbol_node_cleans_up_and_returns_null_when_second_malloc_fails,
            create_symbol_node_setup, create_symbol_node_teardown),
        cmocka_unit_test_setup_teardown(
            create_symbol_node_initializes_and_returns_malloced_ast_when_second_malloc_succeeds,
            create_symbol_node_setup, create_symbol_node_teardown),
    };

    const struct CMUnitTest ast_create_error_node_or_sentinel_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_error_node_or_sentinel_returns_sentinel_when_message_null,
            create_error_node_or_sentinel_setup, create_error_node_or_sentinel_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_returns_sentinel_when_message_is_too_long,
            create_error_node_or_sentinel_setup, create_error_node_or_sentinel_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_returns_sentinel_when_malloc_fails,
            create_error_node_or_sentinel_setup, create_error_node_or_sentinel_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_returns_sentinel_when_strdup_fails,
            create_error_node_or_sentinel_setup, create_error_node_or_sentinel_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_returns_sentinel_when_strdup_and_malloc_fails,
            create_error_node_or_sentinel_setup, create_error_node_or_sentinel_teardown),
        cmocka_unit_test_setup_teardown(
            create_error_node_initializes_and_returns_error_node_when_strdup_and_malloc_succeeds,
            create_error_node_or_sentinel_setup, create_error_node_or_sentinel_teardown),
    };

    const struct CMUnitTest ast_children_reserve_tests[] = {
        cmocka_unit_test_setup_teardown(
            children_reserve_returns_false_when_ast_children_null,
            children_reserve_setup, children_reserve_teardown),
        cmocka_unit_test_setup_teardown(
            children_reserve_returns_false_when_insufficient_capacity,
            children_reserve_setup, children_reserve_teardown),
        cmocka_unit_test_setup_teardown(
            children_reserve_no_side_effect_and_returns_true_when_capacity_sufficient_and_lesser_than_before,
            children_reserve_setup, children_reserve_teardown),
        cmocka_unit_test_setup_teardown(
            children_reserve_no_side_effect_and_returns_false_when_realloc_fails,
            children_reserve_setup, children_reserve_teardown),
        cmocka_unit_test_setup_teardown(
            children_reserve_reallocates_children_update_capacity_and_returns_true_when_realloc_succeeds,
            children_reserve_setup, children_reserve_teardown),
    };

    const struct CMUnitTest ast_children_append_take_tests[] = {
        cmocka_unit_test_setup_teardown(
            children_append_take_returns_false_when_parent_null,
            children_append_take_setup, children_append_take_teardown),
        cmocka_unit_test_setup_teardown(
            children_append_take_does_nothing_and_returns_false_when_child_null,
            children_append_take_setup, children_append_take_teardown),
        cmocka_unit_test_setup_teardown(
            children_append_take_does_nothing_and_returns_false_when_parent_not_a_child_node,
            children_append_take_setup, children_append_take_teardown),
        cmocka_unit_test_setup_teardown(
            children_append_take_append_child_and_returns_true_when_capacity_greater_than_children_nb,
            children_append_take_setup, children_append_take_teardown),
        cmocka_unit_test_setup_teardown(
            children_append_take_returns_false_when_realloc_fails,
            children_append_take_setup, children_append_take_teardown),
    };

    const struct CMUnitTest ast_type_has_children_tests[] = {
        cmocka_unit_test(ast_type_has_children_returns_true_when_container_type),
        cmocka_unit_test(ast_type_has_children_returns_false_when_leaf_type),
    };

    const struct CMUnitTest ast_can_have_children_tests[] = {
        cmocka_unit_test(can_have_children_returns_false_when_a_null),
        cmocka_unit_test(can_have_children_returns_true_when_container_type),
        cmocka_unit_test(can_have_children_returns_false_when_data_wrapper_ast),
    };

    const struct CMUnitTest ast_has_any_child_tests[] = {
        cmocka_unit_test(has_any_child_returns_false_when_a_null),
        cmocka_unit_test(has_any_child_returns_false_when_container_type_with_no_child),
        cmocka_unit_test(ast_has_any_child_returns_false_when_data_wrapper_ast),
    };

    const struct CMUnitTest ast_is_data_of_tests[] = {
        cmocka_unit_test(is_data_of_returns_false_when_a_null),
        cmocka_unit_test(is_data_of_returns_false_when_a_is_container_type),
        cmocka_unit_test(ast_is_data_of_returns_false_when_type_mismatch),
        cmocka_unit_test(ast_is_data_of_returns_true_when_type_match),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(create_typed_data_int_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_int_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_typed_data_string_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_string_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_typed_data_symbol_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_symbol_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_typed_data_wrapper_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_wrapper_calls_destroy_then_free_when_data_wrapper_tests,
                                     NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_ast_children_arr_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_ast_children_arr_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_ast_children_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_children_node_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_children_node_arr_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_children_node_var_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_children_node_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_error_node_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_error_node_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_int_node_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_string_node_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_symbol_name_node_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_symbol_node_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_error_node_or_sentinel_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_children_reserve_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_children_append_take_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_type_has_children_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_can_have_children_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_has_any_child_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_is_data_of_tests, NULL, NULL);

    return failed;
}