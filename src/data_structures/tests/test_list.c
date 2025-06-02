// src/data_structures/tests/test_list.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "logger.h"

#include "list.h"



//-----------------------------------------------------------------------------
// CONSTS
//-----------------------------------------------------------------------------

typedef unsigned char boolean;
#define TRUE 1
#define FALSE 0

static const char dummy;
static const list LIST_DEFINED_IN_SETUP = (list) &dummy;
static const void* ELEMENT_DEFINED_IN_SETUP = (void *) &dummy;
#define EMPTY_LIST NULL
#define LIST_LENGTH 2
static const char STATIC_CHAR_A = 'A';
static const char STATIC_CHAR_B = 'B';
static const char STATIC_CHAR_C = 'C';
#define MALLOC_ERROR_CODE NULL
static const void *DUMMY_NOT_APPLICABLE = (void *) &dummy;



//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES, DUMMIES
//-----------------------------------------------------------------------------

static cons fake_cons[3];
static char *fake_char_ptr[3];
static char fake_char[3];

void * mock_malloc(size_t size) {
    check_expected(size);
    return mock_type(void *);
}

static void * FAKE_MALLOC_RETURNED_PTR;

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
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
char **create_dynamic_char_ptr_array_n(size_t size,...) {
    va_list args;
    va_start(args, size);
    char **ptrs = malloc(sizeof(char *) * size);
    assert_non_null(ptrs);
    for (size_t i = 0; i < size; ++i) {
        ptrs[i] = malloc(sizeof(char));
        assert_non_null(ptrs[i]);
        * ptrs[i] = (char) va_arg(args, int);
    }
    va_end(args);
    return ptrs;
}

void destroy_dynamic_char_ptr_array_n(char ** ptrs, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        free(ptrs[i]);
    }
    free(ptrs);
}

// optionals are char * on statically allocated octet
char **create_static_char_ptr_array_n(size_t size,...) {
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
    .l = NULL,
    .e = NULL,
};

static list_push_test_params_t l_not_null_e_null_statically_allocated = {
    .label = "l != NULL, e == NULL, chars are statically allocated",
	.chars_are_dynamically_allocated = FALSE,
    .l = LIST_DEFINED_IN_SETUP,
    .e = NULL,
};

static list_push_test_params_t l_not_null_e_null_dynamically_allocated = {
    .label = "l != NULL, e == NULL, chars are dynamically allocated",
	.chars_are_dynamically_allocated = TRUE,
    .l = LIST_DEFINED_IN_SETUP,
    .e = NULL,
};

static list_push_test_params_t l_null_e_not_null_statically_allocated = {
    .label = "l == NULL, e != NULL, chars are statically allocated",
	.chars_are_dynamically_allocated = FALSE,
    .l = NULL,
    .e = (void *) &STATIC_CHAR_C,
};

static list_push_test_params_t l_null_e_not_null_dynamically_allocated = {
    .label = "l == NULL, e != NULL, chars are dynamically allocated",
	.chars_are_dynamically_allocated = TRUE,
    .l = NULL,
    .e = (void *) &STATIC_CHAR_C,
};

static list_push_test_params_t l_not_null_e_not_null_statically_allocated = {
    .label = "l == NULL, e != NULL, chars are statically allocated",
	.chars_are_dynamically_allocated = FALSE,
    .l = LIST_DEFINED_IN_SETUP,
    .e = (void *) &STATIC_CHAR_C,
};

static list_push_test_params_t l_not_null_e_not_null_dynamically_allocated = {
    .label = "l == NULL, e != NULL, chars are dynamically allocated",
	.chars_are_dynamically_allocated = TRUE,
    .l = LIST_DEFINED_IN_SETUP,
    .e = (void *) &STATIC_CHAR_C,
};



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int list_push_setup(void **state) {
	set_allocators(mock_malloc, mock_free);
    list_push_test_params_t *params = (list_push_test_params_t *) *state;
    if (params->l == LIST_DEFINED_IN_SETUP) {
        if (params->chars_are_dynamically_allocated) {
			params->char_ptrs = create_dynamic_char_ptr_array_n(LIST_LENGTH, 'A', 'B');
        }
		else {
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
    list_push_test_params_t *params = (list_push_test_params_t *) *state;
    if (params->l == LIST_DEFINED_IN_SETUP) {
        list next = NULL;
        while (params->l) {
            next = (params->l)->cdr;
            free(params->l);
            params->l = next;
        }
        if (params->chars_are_dynamically_allocated)
            destroy_dynamic_char_ptr_array_n(params->char_ptrs, LIST_LENGTH);
        else
            free(params->char_ptrs);
			params->char_ptrs = NULL;
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
// Expected : do not call malloc
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
// mock_free TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/*
static int mock_free_list_setup(void **state) {
    (void) *state;  // unused
    list_length = 3;
    ptrs_on_static_chars =
        create_static_char_ptr_array_n(
            list_length,
            &STATIC_CHAR_A,
            &STATIC_CHAR_B,
            &STATIC_CHAR_C);
    l = make_list_n((void **) ptrs_on_static_chars, list_length);
    return 0;
}

static int mock_free_list_teardown(void **state) {
    (void) *state;
    list next = NULL;
    while (l) {
        next = l->cdr;
        free(l);
        l = next;
    }
    free(ptrs_on_static_chars);
    return 0;
}
*/

/*
static int mock_free_list_setup(void **state) {
    (void) *state;  // unused
    INT_1 = 1;
    dummy_ptr = malloc(sizeof(char));
    A_LIST_WITH_2_ELEMENTS = malloc(sizeof(cons));
    assert_non_null(A_LIST_WITH_2_ELEMENTS);
    A_LIST_WITH_2_ELEMENTS->car = &dummy_ptr;
    A_LIST_WITH_2_ELEMENTS->cdr = (list) &A_LIST_WITH_1_ELEMENT;
    return 0;
}

static int mock_free_list_teardown(void **state) {
    free(A_LIST_WITH_2_ELEMENTS);
    free(dummy_ptr);
    return 0;
}
*/



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------




/*

// Given: l == A_LIST_WITH_2_ELEMENTS, destroy_fn_t == NULL
// Expected : mock_free is called with l
static void mock_free_list_CallFreeWithRightParam_WhenLNotNull_AndDestroyNull(void **state) {
    (void) *state; // unused
    expect_value(mock_free, ptr, A_LIST_WITH_2_ELEMENTS);
    mock_free_list(A_LIST_WITH_2_ELEMENTS, NULL);
}

// Given: l == A_LIST_WITH_2_ELEMENTS, destroy_fn_t == dummy_destroy
// Expected : mock_free is called with l
static void mock_free_list_CallFreeWithRightParam_WhenLNotNull_AndDestroyIs_dummy_destroy(void **state) {
    (void) *state; // unused
    expect_value(mock_free, ptr, A_LIST_WITH_2_ELEMENTS);
    expect_value(dummy_destroy, ptr, A_LIST_WITH_2_ELEMENTS->car);
    mock_free_list(A_LIST_WITH_2_ELEMENTS, dummy_destroy);
}

// Given: l == A_LIST_WITH_2_ELEMENTS, destroy_fn_t == dummy_destroy
// Expected : dummy_destroy is called with l->car
static void mock_free_list_Call_dummy_destroy_WithRightParam_WhenLNotNull_AndDestroyIs_dummy_destroy(void **state) {
    (void) *state; // unused
    expect_value(mock_free, ptr, A_LIST_WITH_2_ELEMENTS);
    expect_value(dummy_destroy, ptr, A_LIST_WITH_2_ELEMENTS->car);
    mock_free_list(A_LIST_WITH_2_ELEMENTS, dummy_destroy);
}

*/



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

    const struct CMUnitTest mock_free_list_tests[] = {

/*
        cmocka_unit_test_setup_teardown(
            mock_free_list_CallFreeWithRightParam_WhenLNotNull_AndDestroyNull,
            mock_free_list_setup, mock_free_list_teardown),
        cmocka_unit_test_setup_teardown(
            mock_free_list_CallFreeWithRightParam_WhenLNotNull_AndDestroyIs_dummy_destroy,
            mock_free_list_setup, mock_free_list_teardown),
        cmocka_unit_test_setup_teardown(
            mock_free_list_Call_dummy_destroy_WithRightParam_WhenLNotNull_AndDestroyIs_dummy_destroy,
            mock_free_list_setup, mock_free_list_teardown),
*/
    };

    int failed = 0;
    failed += cmocka_run_group_tests(list_push_tests, NULL, NULL);
    failed += cmocka_run_group_tests(mock_free_list_tests, NULL, NULL);
    return failed;
}
