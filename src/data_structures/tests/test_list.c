// src/data_structures/tests/test_list.c


// ============================================================================
//  Test suite for linked list implementation: memory management and safety
//  ------------------------------------------------------------------------
//  * Covers all legitimate behaviors (NULL, malloc fail, static/dynamic data).
//  * Includes anti-examples (invalid free, memory leak) as commented blocks,
//    with detailed explanations (NOT to be activated).
//  * Follows best practices for tests and documentation in C (cmocka).
//  * All behaviors not enforceable at runtime (e.g. "never free static memory")
//    are explicitly documented as preconditions.
// ============================================================================


#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "logger.h"
#include "memory_allocator.h"

#include "list.h"


//-----------------------------------------------------------------------------
// CONSTS
//-----------------------------------------------------------------------------

typedef unsigned char boolean;
#define TRUE 1
#define FALSE 0
#define DUMMY_BOOLEAN_VALUE 0

static const char dummy;
static const list LIST_DEFINED_IN_SETUP = (list) & dummy;
static const void *ELEMENT_DEFINED_IN_SETUP = (void *) &dummy;
#define EMPTY_LIST NULL
#define LIST_LENGTH 2
static const char STATIC_CHAR_A = 'A';
static const char STATIC_CHAR_B = 'B';
static const char STATIC_CHAR_C = 'C';
#define MALLOC_ERROR_CODE NULL
static const void *const DUMMY_NOT_APPLICABLE = (void *) &dummy;
static const void *const CURRENT_FREE_DEFINED_IN_SETUP = (void *) &dummy;


//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES, DUMMIES
//-----------------------------------------------------------------------------

static cons fake_cons[3];
static char *fake_char_ptr[3];
static char fake_char[3];

void *mock_malloc(size_t size) {
    check_expected(size);
    return mock_type(void *);
}

static void *FAKE_MALLOC_RETURNED_PTR;

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
}

void destroy_fn_with_current_free(void *item, void *user_data) {
    (void) user_data; // unused
    get_current_free()(item);
}

void dummy_destroy(void *ptr) {
    check_expected_ptr(ptr);
}


//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------


list make_list_n(void **ptrs, size_t n) {
    list head = NULL, prev = NULL;
    for (size_t i = 0; i < n; ++i) {
        list node = malloc(sizeof(cons));
        assert_non_null(node);
        node->car = ptrs[i];
        node->cdr = NULL;
        if (!head) head = node;
        if (prev) prev->cdr = node;
        prev = node;
    }
    return head;
}

// optionals are char
char **create_dynamic_char_ptr_array_n(size_t size, ...) {
    va_list args;
    va_start(args, size);
    char **ptrs = malloc(sizeof(char *) * size);
    assert_non_null(ptrs);
    for (size_t i = 0; i < size; ++i) {
        ptrs[i] = malloc(sizeof(char));
        assert_non_null(ptrs[i]);
        *ptrs[i] = (char) va_arg(args, int);
    }
    va_end(args);
    return ptrs;
}

void destroy_dynamic_char_ptr_array_n(char **ptrs, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        free(ptrs[i]);
    }
    free(ptrs);
}

// optionals are char * on statically allocated octet
char **create_static_char_ptr_array_n(size_t size, ...) {
    va_list args;
    va_start(args, size);
    char **ptrs = malloc(sizeof(char *) * size);
    assert_non_null(ptrs);
    for (size_t i = 0; i < size; ++i) {
        ptrs[i] = va_arg(args, char *);
    }
    va_end(args);
    return ptrs;
}


//-----------------------------------------------------------------------------
// list_push TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
    const char *label;
    boolean chars_are_dynamically_allocated;
    char **char_ptrs;
	boolean l_is_defined_in_set_up;
    list l;
    void *e;
} list_push_test_params_t;


//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------


static list list_push_param_l(void **state) {
    return ((list_push_test_params_t *) *state)->l;
}

static list list_push_param_e(void **state) {
    return ((list_push_test_params_t *) *state)->e;
}

static void list_push_expect_no_side_effect(void **state, boolean malloc_is_called, void *malloc_returned_value) {
    list l = list_push_param_l(state);
    void *e = list_push_param_e(state);
    if (l) {
        memcpy(&fake_cons[0], l, sizeof(cons));
        memcpy(&fake_cons[1], l->cdr, sizeof(cons));
        memcpy(&fake_char_ptr[0], &(l->car), sizeof(char *));
        memcpy(&fake_char_ptr[1], &((l->cdr)->car), sizeof(char *));
        memcpy(&(fake_char[0]), l->car, sizeof(char));
        memcpy(&(fake_char[1]), (l->cdr)->car, sizeof(char));
    }
    if (e) {
        memcpy(&(fake_char[2]), e, sizeof(char));
    }
    if (malloc_is_called) {
        expect_value(mock_malloc, size, sizeof(cons));
        will_return(mock_malloc, malloc_returned_value);
    }
    list_push(list_push_param_l(state), list_push_param_e(state));
    if (l) {
        assert_memory_equal(&fake_cons[0], l, sizeof(cons));
        assert_memory_equal(&fake_cons[1], l->cdr, sizeof(cons));
        assert_memory_equal(&fake_char_ptr[0], &(l->car), sizeof(char *));
        assert_memory_equal(&fake_char_ptr[1], &((l->cdr)->car), sizeof(char *));
        assert_memory_equal(&(fake_char[0]), l->car, sizeof(char));
        assert_memory_equal(&(fake_char[1]), (l->cdr)->car, sizeof(char));
    }
    if (e) {
        assert_memory_equal(&(fake_char[2]), e, sizeof(char));
    }
}


//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------


static list_push_test_params_t l_null_e_null = {
    .label = "l == NULL, e == NULL",
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = false,
    .l = NULL,
    .e = NULL,
};

static list_push_test_params_t l_not_null_e_null_statically_allocated = {
    .label = "l != NULL, e == NULL, chars are statically allocated",
    .chars_are_dynamically_allocated = FALSE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = true,
    .l = NULL,
    .e = NULL,
};

static list_push_test_params_t l_not_null_e_null_dynamically_allocated = {
    .label = "l != NULL, e == NULL, chars are dynamically allocated",
    .chars_are_dynamically_allocated = TRUE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = true,
    .l = NULL,
    .e = NULL,
};

static list_push_test_params_t l_null_e_not_null_statically_allocated = {
    .label = "l == NULL, e != NULL, chars are statically allocated",
    .chars_are_dynamically_allocated = FALSE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = false,
    .l = NULL,
    .e = (void *) &STATIC_CHAR_C,
};

static list_push_test_params_t l_null_e_not_null_dynamically_allocated = {
    .label = "l == NULL, e != NULL, chars are dynamically allocated",
    .chars_are_dynamically_allocated = TRUE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = false,
    .l = NULL,
    .e = (void *) &STATIC_CHAR_C,
};

static list_push_test_params_t l_not_null_e_not_null_statically_allocated = {
    .label = "l == NULL, e != NULL, chars are statically allocated",
    .chars_are_dynamically_allocated = FALSE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = true,
    .l = NULL,
    .e = (void *) &STATIC_CHAR_C,
};

static list_push_test_params_t l_not_null_e_not_null_dynamically_allocated = {
    .label = "l == NULL, e != NULL, chars are dynamically allocated",
    .chars_are_dynamically_allocated = TRUE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = true,
    .l = NULL,
    .e = (void *) &STATIC_CHAR_C,
};


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int list_push_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    list_push_test_params_t *params = (list_push_test_params_t *) *state;
    if (params->l_is_defined_in_set_up) {
        if (params->chars_are_dynamically_allocated) {
            params->char_ptrs = create_dynamic_char_ptr_array_n(LIST_LENGTH, 'A', 'B');
        } else {
            params->char_ptrs = create_static_char_ptr_array_n(LIST_LENGTH, &STATIC_CHAR_A, &STATIC_CHAR_B);
        }
        assert_non_null(params->char_ptrs);
        params->l = make_list_n((void **) params->char_ptrs, LIST_LENGTH);
    }
    if (params->e == ELEMENT_DEFINED_IN_SETUP) {
        if (params->chars_are_dynamically_allocated) {
            params->e = malloc(sizeof(char));
            assert_non_null(params->e);
        }
    }
    FAKE_MALLOC_RETURNED_PTR = malloc(sizeof(cons));
    memset(FAKE_MALLOC_RETURNED_PTR, 0, sizeof(cons));
    return 0;
}

static int list_push_teardown(void **state) {
    set_allocators(NULL, NULL);
    list_push_test_params_t *params = (list_push_test_params_t *) *state;
    if (params->l_is_defined_in_set_up) {
        list next = NULL;
        while (params->l) {
            next = (params->l)->cdr;
            free(params->l);
            params->l = next;
        }
        if (params->chars_are_dynamically_allocated)
            destroy_dynamic_char_ptr_array_n(params->char_ptrs, LIST_LENGTH);
        else {
            free(params->char_ptrs);
            params->char_ptrs = NULL;
        }
    }
    free(FAKE_MALLOC_RETURNED_PTR);
    FAKE_MALLOC_RETURNED_PTR = NULL;
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: e == NULL
// Expected: returns NULL
// param:
//	- l_null_e_null
//	- l_not_null_e_null_statically_allocated
//	- l_not_null_e_null_dynamically_allocated
static void list_push_returns_null_when_e_null(void **state) {
    assert_null(list_push(list_push_param_l(state), list_push_param_e(state)));
}

// Given: e == NULL
// Expected : does not call malloc
// param:
//	- l_null_e_null
//	- l_not_null_e_null_statically_allocated
//	- l_not_null_e_null_dynamically_allocated
static void list_push_do_not_call_malloc_when_e_null(void **state) {
    list_push(list_push_param_l(state), list_push_param_e(state));
}

// Given: e != 0
// Expected : call malloc(sizeof(cons))
//	- l_null_e_not_null_statically_allocated
//	- l_null_e_not_null_dynamically_allocated
//	- l_not_null_e_not_null_statically_allocated
//	- l_not_null_e_not_null_dynamically_allocated
static void list_push_call_malloc_when_e_not_null(void **state) {
    expect_value(mock_malloc, size, sizeof(cons));
    will_return(mock_malloc, FAKE_MALLOC_RETURNED_PTR);
    list_push(list_push_param_l(state), list_push_param_e(state));
}

// Given: malloc fail
// Expected: returns NULL
// param:
//	- l_null_e_not_null_statically_allocated
//	- l_null_e_not_null_dynamically_allocated
//	- l_not_null_e_not_null_statically_allocated
//	- l_not_null_e_not_null_dynamically_allocated
static void list_push_returns_null_when_malloc_fail(void **state) {
    expect_value(mock_malloc, size, sizeof(cons));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(list_push(list_push_param_l(state), list_push_param_e(state)));
}

// Given: do not call malloc
// Expected: no side effect
// param:
//	- l_null_e_null
//	- l_not_null_e_null_statically_allocated
//	- l_not_null_e_null_dynamically_allocated
static void list_push_no_side_effect_when_do_not_call_malloc(void **state) {
    list_push_expect_no_side_effect(state, FALSE, (void *) DUMMY_NOT_APPLICABLE);
}

// Given: malloc fail
// Expected: no side effect
// param:
//	- l_null_e_not_null_statically_allocated
//	- l_null_e_not_null_dynamically_allocated
//	- l_not_null_e_not_null_statically_allocated
//	- l_not_null_e_not_null_dynamically_allocated
static void list_push_no_side_effect_when_malloc_fail(void **state) {
    list_push_expect_no_side_effect(state, TRUE, MALLOC_ERROR_CODE);
}

// Given: malloc success
// Expected: no side effect
// param:
//	- l_null_e_not_null_statically_allocated
//	- l_null_e_not_null_dynamically_allocated
//	- l_not_null_e_not_null_statically_allocated
//	- l_not_null_e_not_null_dynamically_allocated
static void list_push_no_side_effect_when_malloc_success(void **state) {
    list_push_expect_no_side_effect(state, TRUE, FAKE_MALLOC_RETURNED_PTR);
}

// Given: malloc success
// Expected: returns the address returned by malloc
// param:
//	- l_null_e_not_null_statically_allocated
//	- l_null_e_not_null_dynamically_allocated
//	- l_not_null_e_not_null_statically_allocated
//	- l_not_null_e_not_null_dynamically_allocated
static void list_push_returns_expected_value_when_malloc_success(void **state) {
    expect_value(mock_malloc, size, sizeof(cons));
    will_return(mock_malloc, FAKE_MALLOC_RETURNED_PTR);
    list ret = list_push(list_push_param_l(state), list_push_param_e(state));
    assert_ptr_equal(ret, FAKE_MALLOC_RETURNED_PTR);
}

// Given: malloc success
// Expected: writes {e, l} at the address returned by malloc
// param:
//	- l_null_e_not_null_statically_allocated
//	- l_null_e_not_null_dynamically_allocated
//	- l_not_null_e_not_null_statically_allocated
//	- l_not_null_e_not_null_dynamically_allocated
static void list_push_makes_expected_side_effect_when_malloc_success(void **state) {
    expect_value(mock_malloc, size, sizeof(cons));
    will_return(mock_malloc, FAKE_MALLOC_RETURNED_PTR);
    cons expected_memory_state = {list_push_param_e(state), list_push_param_l(state)};
    list_push(list_push_param_l(state), list_push_param_e(state));
    assert_memory_equal(FAKE_MALLOC_RETURNED_PTR, &expected_memory_state, sizeof(cons));
}


//-----------------------------------------------------------------------------
// list_free_list TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
    const char *label;
    boolean chars_are_dynamically_allocated;
    char **char_ptrs;
	boolean l_is_defined_in_set_up;
    list l;
    void (*f)(void *, void *);
} list_free_list_test_params_t;


//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------


static list list_free_list_param_l(void **state) {
    return ((list_free_list_test_params_t *) *state)->l;
}

static void *list_free_list_param_f(void **state) {
    return ((list_free_list_test_params_t *) *state)->f;
}


//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------

static list_free_list_test_params_t l_null_f_null = {
    .label = "l == NULL, f == NULL",
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = false,
    .l = NULL,
    .f = NULL,
};

static list_free_list_test_params_t l_null_f_not_null = {
    .label = "l == NULL, f != NULL",
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = false,
    .l = NULL,
    .f = destroy_fn_with_current_free,
};

static list_free_list_test_params_t l_not_null_f_null_statically_allocated = {
    .label = "l != NULL, f == NULL, chars are statically allocated",
    .chars_are_dynamically_allocated = FALSE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = true,
    .l = NULL,
    .f = NULL,
};

static list_free_list_test_params_t l_not_null_f_null_dynamically_allocated = {
    .label = "l != NULL, f == NULL, chars are dynamically allocated",
    .chars_are_dynamically_allocated = TRUE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = true,
    .l = NULL,
    .f = NULL,
};

static list_free_list_test_params_t l_not_null_f_current_free_statically_allocated = {
    .label = "l != NULL, f == current_free, chars are statically allocated",
    .chars_are_dynamically_allocated = FALSE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = true,
    .l = NULL,
    .f = CURRENT_FREE_DEFINED_IN_SETUP,
};

static list_free_list_test_params_t l_not_null_f_current_free_dynamically_allocated = {
    .label = "l != NULL, f == current_free, chars are dynamically allocated",
    .chars_are_dynamically_allocated = TRUE,
    .char_ptrs = NULL,
	.l_is_defined_in_set_up = true,
    .l = NULL,
    .f = CURRENT_FREE_DEFINED_IN_SETUP,
};


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int list_free_list_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    list_free_list_test_params_t *params = (list_free_list_test_params_t *) *state;
    if (params->l_is_defined_in_set_up) {
        if (params->chars_are_dynamically_allocated) {
            params->char_ptrs = create_dynamic_char_ptr_array_n(LIST_LENGTH, 'A', 'B');
        } else {
            params->char_ptrs = create_static_char_ptr_array_n(LIST_LENGTH, &STATIC_CHAR_A, &STATIC_CHAR_B);
        }
        assert_non_null(params->char_ptrs);
        params->l = make_list_n((void **) params->char_ptrs, LIST_LENGTH);
    }
    if (params->f == CURRENT_FREE_DEFINED_IN_SETUP) {
        params->f = destroy_fn_with_current_free;
    }
    return 0;
}

static int list_free_list_teardown(void **state) {
    set_allocators(NULL, NULL);
    list_free_list_test_params_t *params = (list_free_list_test_params_t *) *state;
    if (params->l_is_defined_in_set_up) {
        list next = NULL;
        while (params->l) {
            next = (params->l)->cdr;
            free(params->l);
            params->l = next;
        }
        if (params->chars_are_dynamically_allocated)
            destroy_dynamic_char_ptr_array_n(params->char_ptrs, LIST_LENGTH);
        else {
            free(params->char_ptrs);
            params->char_ptrs = NULL;
        }
    }

    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: l == NULL
// Expected: no side effect
// param:
//	- l_null_f_null
//	- l_null_f_not_null
static void list_free_list_no_side_effect_when_l_null(void **state) {
    list_free_list(list_free_list_param_l(state), list_free_list_param_f(state), NULL);
}

// Given: l != NULL, f == null
// Expected: l and its cdrs are freed
// param:
//	- l_not_null_f_null_statically_allocated
//	- l_not_null_f_null_dynamically_allocated
static void list_free_list_free_l_when_l_not_null_f_null(void **state) {
    list l = list_free_list_param_l(state);
    expect_value(mock_free, ptr, l);
    expect_value(mock_free, ptr, l->cdr);
    list_free_list(list_free_list_param_l(state), list_free_list_param_f(state), NULL);
}

// Given: l != NULL, f == current_free, chars are dynamically allocated
// Expected: frees l's cars
// param:
//	- l_not_null_f_current_free_dynamically_allocated
static void list_free_list_free_l_when_l_not_null_f_current_free_dynamically_allocated(void **state) {
    list l = list_free_list_param_l(state);
    expect_value(mock_free, ptr, l->car);
    expect_value(mock_free, ptr, l);
    expect_value(mock_free, ptr, (l->cdr)->car);
    expect_value(mock_free, ptr, l->cdr);
    list_free_list(list_free_list_param_l(state), list_free_list_param_f(state), NULL);
}

// EXAMPLES OF TESTS THAT DO NOT RESPECT BEST PRACTICES (DO NOT USE!)
//
// The following tests illustrate *incorrect* approaches in C memory management
// and are COMMENTED OUT or should NOT be included in a professional tests suite.
//
// 1. Testing/expecting invalid frees (undefined behavior).
// 2. Testing/validating a memory leak as an expected outcome.
//
// Both are poor practices: one encourages dangerous code, the other
// encourages not managing memory properly. The right approach is to
// document/prevent these cases, not tests them as “expected” behavior.
// -----------------------------------------------------------------------------
//
// 1. BAD PRACTICE: Test that expects invalid free of statically-allocated memory
//
// In C, calling free() on a static/global/stack variable is undefined behavior.
// A tests should NEVER expect or require this.
// Purpose: Example of what NOT to do.
//
// Given: l != NULL, f == current_free, cars are statically allocated
// Expected: (INCORRECT) invalid free of l's cars
// param:
//	- l_not_null_f_current_free_statically_allocated
/*
static void list_free_list_invalid_free_of_cars_when_l_not_null_f_current_free(void **state) {
    list l = list_free_list_param_l(state);
    expect_value(mock_free, ptr, &STATIC_CHAR_A); // invalid free! BAD PRACTICE
    expect_value(mock_free, ptr, l);
    expect_value(mock_free, ptr, &STATIC_CHAR_B); // invalid free! BAD PRACTICE
    expect_value(mock_free, ptr, l->cdr);
    list_free_list(list_free_list_param_l(state), list_free_list_param_f(state), NULL);
}
*/
//
// 2. BAD PRACTICE: Test that validates a memory leak as “success”
//
// In C, a memory leak is always a sign that memory management is not handled.
// You can document that a function will leak if used incorrectly, but you
// should NOT write a tests to “validate” a leak.
//
// Given: l != NULL, f == NULL, chars are dynamically allocated
// Expected: (INCORRECT) memory leaks due to not freeing l's cars
// param:
//	- l_not_null_f_null_dynamically_allocated
/*
static void list_free_list_memory_leaks_at_cars_when_l_not_null_f_null_dynamically_allocated(void **state) {
    list l = list_free_list_param_l(state);
    char *element_1 = (char *)l->car;
    char *element_2 = (char *)(l->cdr)->car;
    expect_value(mock_free, ptr, l);
    expect_value(mock_free, ptr, l->cdr);
    list_free_list(list_free_list_param_l(state), list_free_list_param_f(state), NULL);
    // The following does not "prove" a leak; it just means memory was not freed.
    // BAD PRACTICE: Validating a leak is not the job of a unit tests.
    assert_non_null(element_1);
    assert_non_null(element_2);
    *element_1 = dummy;
    *element_2 = dummy;
}
*/


//-----------------------------------------------------------------------------
// list_pop TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int list_pop_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int list_pop_teardown(void **state) {
    set_allocators(NULL, NULL);
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - l_p == NULL
// Expected:
//  - return NULL
static void list_pop_returns_null_when_l_p_null(void **state) {
    assert_null(list_pop(NULL));
}

// Given:
//  - l_p != NULL && *l_p == NULL
// Expected:
//  - return NULL
static void list_pop_returns_null_when_list_empty(void **state) {
    list *l_p = malloc(sizeof(list *));
    assert_non_null(l_p);
    *l_p = NULL;

    assert_null(list_pop(l_p));

	free(l_p);
}

// Given:
//  - l_p != NULL && *l_p != NULL && (*l_p)->cdr == NULL
// Expected:
//  - (*l_p) == (*old_l_p)->cdr
//  - calls free with (*old_l_p)
//  - return (*old_l_p)->car
static void list_pop_returns_car_and_frees_when_singleton_list(void **state) {
    list l = malloc(sizeof(cons));
    assert_non_null(l);
    void *stub_element_p = (void *) 0xDEADBEEF;
    l->car = stub_element_p;
    l->cdr = NULL;
    expect_value(mock_free, ptr, l);
	cons *mem = l;

    void *ret = list_pop(&l);

    assert_ptr_equal(ret, stub_element_p);
    assert_null(l);

	free(mem);
	free(l);
}

// Given:
//  - l_p points to two-element list
// Expected:
//  - (*l_p) == (*old_l_p)->cdr
//  - calls free with (*old_l_p)
//  - return (*old_l_p)->car
static void list_pop_returns_car_and_frees_and_advances_when_two_elements(void **state) {
    void *stub_element_1_p = (void *) 0xDEADBEEE;
    void *stub_element_2_p = (void *) 0xDEADBEEF;
    list l_end;
    list l = malloc(sizeof(cons));
    assert_non_null(l);
    l->car = stub_element_2_p;
    l->cdr = NULL;
    l_end = l;
    l = malloc(sizeof(cons));
    assert_non_null(l);
    l->car = stub_element_1_p;
    l->cdr = l_end;
    expect_value(mock_free, ptr, l);
	cons *mem = l;

    void *ret = list_pop(&l);

    assert_ptr_equal(ret, stub_element_1_p);
    assert_ptr_equal(l, l_end);
    assert_ptr_equal(l->car, stub_element_2_p);

	free(mem);
	free(l);
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest list_push_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_null_when_e_null,
            list_push_setup, list_push_teardown, &l_null_e_null),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_null_when_e_null,
            list_push_setup, list_push_teardown, &l_not_null_e_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_null_when_e_null,
            list_push_setup, list_push_teardown, &l_not_null_e_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_do_not_call_malloc_when_e_null,
            list_push_setup, list_push_teardown, &l_null_e_null),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_do_not_call_malloc_when_e_null,
            list_push_setup, list_push_teardown, &l_not_null_e_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_do_not_call_malloc_when_e_null,
            list_push_setup, list_push_teardown, &l_not_null_e_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_call_malloc_when_e_not_null,
            list_push_setup, list_push_teardown, &l_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_call_malloc_when_e_not_null,
            list_push_setup, list_push_teardown, &l_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_call_malloc_when_e_not_null,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_call_malloc_when_e_not_null,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_null_when_malloc_fail,
            list_push_setup, list_push_teardown, &l_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_null_when_malloc_fail,
            list_push_setup, list_push_teardown, &l_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_null_when_malloc_fail,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_null_when_malloc_fail,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_do_not_call_malloc,
            list_push_setup, list_push_teardown, &l_null_e_null),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_do_not_call_malloc,
            list_push_setup, list_push_teardown, &l_not_null_e_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_do_not_call_malloc,
            list_push_setup, list_push_teardown, &l_not_null_e_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_malloc_fail,
            list_push_setup, list_push_teardown, &l_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_malloc_fail,
            list_push_setup, list_push_teardown, &l_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_malloc_fail,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_malloc_fail,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_malloc_success,
            list_push_setup, list_push_teardown, &l_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_malloc_success,
            list_push_setup, list_push_teardown, &l_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_malloc_success,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_no_side_effect_when_malloc_success,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_expected_value_when_malloc_success,
            list_push_setup, list_push_teardown, &l_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_expected_value_when_malloc_success,
            list_push_setup, list_push_teardown, &l_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_expected_value_when_malloc_success,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_returns_expected_value_when_malloc_success,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_makes_expected_side_effect_when_malloc_success,
            list_push_setup, list_push_teardown, &l_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_makes_expected_side_effect_when_malloc_success,
            list_push_setup, list_push_teardown, &l_null_e_not_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_makes_expected_side_effect_when_malloc_success,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_push_makes_expected_side_effect_when_malloc_success,
            list_push_setup, list_push_teardown, &l_not_null_e_not_null_dynamically_allocated),
    };

    const struct CMUnitTest list_free_list_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            list_free_list_no_side_effect_when_l_null,
            list_free_list_setup, list_free_list_teardown, &l_null_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            list_free_list_no_side_effect_when_l_null,
            list_free_list_setup, list_free_list_teardown, &l_null_f_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            list_free_list_free_l_when_l_not_null_f_null,
            list_free_list_setup, list_free_list_teardown, &l_not_null_f_null_statically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_free_list_free_l_when_l_not_null_f_null,
            list_free_list_setup, list_free_list_teardown, &l_not_null_f_null_dynamically_allocated),
        cmocka_unit_test_prestate_setup_teardown(
            list_free_list_free_l_when_l_not_null_f_current_free_dynamically_allocated,
            list_free_list_setup, list_free_list_teardown, &l_not_null_f_current_free_dynamically_allocated),

        // -------------------------------------------------------------------------
        // EXAMPLES OF TESTS THAT DO NOT RESPECT BEST PRACTICES (DO NOT USE!)
        // -------------------------------------------------------------------------
        // 1. This tests expects invalid frees of statically-allocated memory,
        //    which is undefined behavior in C. Kept commented as a pedagogical anti-example.
        // 2. The next tests validates a memory leak as an expected outcome,
        //    which is also not a proper use of unit tests.
        // -------------------------------------------------------------------------
        /*
    cmocka_unit_test_prestate_setup_teardown(
        list_free_list_invalid_free_of_cars_when_l_not_null_f_current_free,
        list_free_list_setup, list_free_list_teardown, &l_not_null_f_current_free_statically_allocated),

    cmocka_unit_test_prestate_setup_teardown(
        list_free_list_memory_leaks_at_cars_when_l_not_null_f_null_dynamically_allocated,
        list_free_list_setup, list_free_list_teardown, &l_not_null_f_null_dynamically_allocated),
*/

    };

    const struct CMUnitTest list_pop_tests[] = {
        cmocka_unit_test(list_pop_returns_null_when_l_p_null),
        cmocka_unit_test(list_pop_returns_null_when_list_empty),
        cmocka_unit_test_setup_teardown(
            list_pop_returns_car_and_frees_when_singleton_list,
            list_pop_setup, list_pop_teardown),
        cmocka_unit_test_setup_teardown(
            list_pop_returns_car_and_frees_and_advances_when_two_elements,
            list_pop_setup, list_pop_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(list_push_tests, NULL, NULL);
    failed += cmocka_run_group_tests(list_free_list_tests, NULL, NULL);
    failed += cmocka_run_group_tests(list_pop_tests, NULL, NULL);

    return failed;
}