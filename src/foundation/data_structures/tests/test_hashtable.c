// src/foundation/data_structures/tests/test_hashtable.c

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

#include "internal/hashtable_internal.h"
#include "internal/hashtable_test_utils.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


typedef unsigned char boolean;
#define TRUE 1
#define FALSE 0
static const boolean DUMMY_BOOLEAN_VALUE = 0;

static list collected_ptr_to_be_freed = NULL;

typedef struct {
    const char *key;
    void *value;
} key_value_pair;

static char dummy;
static void *const DUMMY_PTR = &dummy;
static const void *DUMMY_MALLOC_RETURNED_VALUE = (void *) &dummy;
static const void *DUMMY_STRDUP_RETURNED_VALUE = &dummy;
static const hashtable_destroy_value_fn_t DUMMY_HASHTABLE_DESTROY_VALUE_FN = (hashtable_destroy_value_fn_t) &dummy;
static char *const DUMMY_KEY = DUMMY_PTR;
static void *const DUMMY_VALUE = (void *) &dummy;
static key_value_pair DUMMY_KEY_VALUE_PAIR = {DUMMY_KEY, DUMMY_VALUE};
static key_value_pair * const DUMMY_KEY_VALUE_PAIRS = &DUMMY_KEY_VALUE_PAIR;
#define MALLOC_ERROR_CODE NULL
#define STRDUP_ERROR_CODE NULL

static const int DUMMY_INT_VALUE = 0;
static void *const DUMMY_POINTER_VALUE = NULL;
static const size_t DUMMY_SIZE_T_VALUE = 0;
static hashtable * const HASHTABLE_DEFINED_IN_SETUP = (hashtable *) &dummy;
static entry ** const ENTRIES_DEFINED_IN_SETUP = (entry **) &dummy;
static key_value_pair * const KEY_VALUE_PAIRS_TO_BE_ADDED_DEFINED_IN_SET_UP = (key_value_pair *) &dummy;

static const char STATIC_CHAR_A = 'A';
static const char STATIC_CHAR_B = 'B';
static const char STATIC_CHAR_C = 'C';
static const char STATIC_CHAR_D = 'D';
static char STATIC_CHARS[4] = {STATIC_CHAR_A, STATIC_CHAR_B, STATIC_CHAR_C, STATIC_CHAR_D};
static const char * const KEY_VALUES[4] = {"key_for_A", "key_for_B", "key_for_C", "key_for_D"};
static const char * const A_KEY_NOT_IN_USE = "a key not in use";
static const char * const A_KEY_IN_USE = "key_for_A";
static void * VALUE_CORRESPONDING_TO_A_KEY_IN_USE;
static const size_t INDEX_OF_A_KEY_IN_USE = 0;
static const size_t INDEX_OF_A_KEY_NOT_IN_USE = 3;
static const size_t DUMMY_INDEX_OF_A_DUMMY_KEY = 3;
static const hashtable *DUMMY_HASHTABLE_P = (hashtable *) &dummy;

static key_value_pair key_value_pairs_with_key_null = {
    .key = NULL,
    .value = DUMMY_VALUE,
};

static hashtable_key_type STUB_KEY_TYPE = HASHTABLE_KEY_TYPE_STRING;



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


void * mock_malloc(size_t size) {
    check_expected(size);
    return mock_type(void *);
}

static void *fake_malloc_returned_value_for_hashtable;
static void *fake_malloc_returned_value_for_buckets;
static void *fake_malloc_returned_value_for_a_new_entry_in_hashtable;
static char *fake_strdup_returned_value_for_key_of_hashtable;
static void *fake_malloc_returned_value_for_a_new_cons_for_a_bucket;

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
}

void destroy_fn_with_current_free(void *item, void *user_data){
    (void)user_data; // unused
    get_current_free()(item);
}

int mock_strcmp(const char *s1, const char *s2) {
    check_expected_ptr(s1);
    check_expected_ptr(s2);
    return mock_type(int);
}

char *mock_strdup(const char *s) {
    check_expected_ptr(s);
    return mock_type(char *);
}

static char *fake_strdup_returned_value_for_key_of_hashtable;

unsigned long mock_hash_djb2(const void *key, hashtable_key_type key_type) {
    return INDEX_OF_A_KEY_IN_USE;
}
unsigned long mock_hash_djb2_dummy_return_0(const void *key, hashtable_key_type key_type) {
    return 0;
}
unsigned long mock_hash_djb2_dummy_return_1(const void *key, hashtable_key_type key_type) {
    return 1;
}



//-----------------------------------------------------------------------------
// GENERAL PARAMETRIC TEST STRUCTURES
//----------------------------------------------------------------------------


typedef struct {
    const char *label;
    size_t s; // 1 or 2
    hashtable_destroy_value_fn_t f;
    int number_of_entries; // 0,1,2 or 3
    boolean there_is_a_collision; // at the first entry and only one collision
    boolean chars_are_dynamically_allocated;
    entry **entries; // values are chars (in rodata or heap)
} hashtable_config;

typedef struct {
    const char *label;
    hashtable *ht;
    entry **entries; // values are chars
    hashtable_config config;
} hashtable_params_t;

typedef struct {
    const char *label;
    boolean key_is_already_in_use;
    key_value_pair *key_value_pairs_to_be_added;
} key_value_pairs_params_t;

typedef struct {
    const char *label;
    const hashtable_params_t *hashtable_params;
    const key_value_pairs_params_t *key_value_pairs_params;
} params_t;

typedef struct {
    const char *label;
    hashtable_params_t *hashtable_params;
    key_value_pairs_params_t *key_value_pairs_params;
} params_instance_t;



//-----------------------------------------------------------------------------
// GENERAL CONFIG CASES
//-----------------------------------------------------------------------------


// ----- Hashtable config

static const hashtable_config DUMMY_CONFIG = {0,0,0,0,0,};

static const hashtable_config config_ht_null = {
    .label = "ht == NULL",
    .s = DUMMY_SIZE_T_VALUE,
    .f = DUMMY_POINTER_VALUE,
    .number_of_entries = DUMMY_INT_VALUE,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
};

static const hashtable_config config_s_1_n_0_f_null = {
    .label = "hashtable size == 1, number of entries == 0, destroy_value_fn = NULL",
    .s = 1,
    .f = NULL,
    .number_of_entries = 0,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
};

static const hashtable_config config_s_1_n_0_f_free = {
    .label = "hashtable size == 1, number of entries == 0, destroy_value_fn = mock_free",
    .s = 1,
    .f = mock_free,
    .number_of_entries = 0,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
};

static const hashtable_config config_s_1_n_1_static = {
    .label = "hashtable size == 1, number of entries == 1, static allocation, destroy_value_fn = NULL",
    .s = 1,
    .f = NULL,
    .number_of_entries = 1,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = FALSE,
};

static const hashtable_config config_s_1_n_1_dynamic = {
    .label = "hashtable size == 1, number of entries == 1, dynamic allocation, destroy_value_fn = mock_free",
    .s = 1,
    .f = mock_free,
    .number_of_entries = 1,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = TRUE,
};

static const hashtable_config config_s_1_n_2_collision_static = {
    .label = "hashtable size == 1, number of entries == 2, collision, static allocation, destroy_value_fn = NULL",
    .s = 1,
    .f = NULL,
    .number_of_entries = 2,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = FALSE,
};

static const hashtable_config config_s_1_n_2_collision_dynamic = {
    .label = "hashtable size == 1, number of entries == 2, collision, dynamic allocation, destroy_value_fn = mock_free",
    .s = 1,
    .f = mock_free,
    .number_of_entries = 2,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = TRUE,
};

static const hashtable_config config_s_2_n_0_f_null = {
    .label = "hashtable size == 2, number of entries == 0, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 0,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
};

static const hashtable_config config_s_2_n_0_f_free = {
    .label = "hashtable size == 2, number of entries == 0, destroy_value_fn = mock_free",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 0,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = DUMMY_BOOLEAN_VALUE,
};

static const hashtable_config config_s_2_n_1_static = {
    .label = "hashtable size == 2, number of entries == 1, static allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 1,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = FALSE,
};

static const hashtable_config config_s_2_n_1_dynamic = {
    .label = "hashtable size == 2, number of entries == 1, dynamic allocation, destroy_value_fn = mock_free",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 1,
    .there_is_a_collision = DUMMY_BOOLEAN_VALUE,
    .chars_are_dynamically_allocated = TRUE,
};

static const hashtable_config config_s_2_n_2_no_collision_static = {
    .label = "hashtable size == 2, number of entries == 2, no collision, static allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 2,
    .there_is_a_collision = FALSE,
    .chars_are_dynamically_allocated = FALSE,
};

static const hashtable_config config_s_2_n_2_collision_static = {
    .label = "hashtable size == 2, number of entries == 2, collision, static allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 2,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = FALSE,
};

static const hashtable_config config_s_2_n_2_no_collision_dynamic = {
    .label = "hashtable size == 2, number of entries == 2, no collision, dynamic allocation, destroy_value_fn = mock_free",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 2,
    .there_is_a_collision = FALSE,
    .chars_are_dynamically_allocated = TRUE,
};

static const hashtable_config config_s_2_n_2_collision_dynamic = {
    .label = "hashtable size == 2, number of entries == 2, collision, dynamic allocation, destroy_value_fn = mock_free",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 2,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = TRUE,
};

static const hashtable_config config_s_2_n_3_collision_static = {
    .label = "hashtable size == 2, number of entries == 3, collision, static allocation, destroy_value_fn = NULL",
    .s = 2,
    .f = NULL,
    .number_of_entries = 3,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = FALSE,
};

static const hashtable_config config_s_2_n_3_collision_dynamic = {
    .label = "hashtable size == 2, number of entries == 3, collision, dynamic allocation, destroy_value_fn = mock_free",
    .s = 2,
    .f = mock_free,
    .number_of_entries = 3,
    .there_is_a_collision = TRUE,
    .chars_are_dynamically_allocated = TRUE,
};

/*
description of non trivial cases that will tested:
note : entries put from the first bucket and collision only at first bucket

cases s == 1, number_of_entries == 0: (2 cases)
    label:
    s: 1
    f: null or mock_free
    number_of_entries: 0
    there_is_a_collision: dummy
    chars_are_dynamically_allocated: dummy
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 1, number_of_entries == 1: (2 cases)
    label:
    s: 1
    f: null or mock_free
    number_of_entries: 1 (put at first place)
    there_is_a_collision: dummy
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 1, number_of_entries == 2: (2 cases)
    label:
    s: 1
    f: null or mock_free
    number_of_entries: 2
    there_is_a_collision: TRUE
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 2, number_of_entries == 0: (2 cases)
    label:
    s: 2
    f: null or mock_free
    number_of_entries: 0
    there_is_a_collision: dummy
    chars_are_dynamically_allocated: dummy
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 2, number_of_entries == 1: (2 cases)
    label:
    s: 2
    f: null or mock_free
    number_of_entries: 1
    there_is_a_collision: dummy
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 2, number_of_entries == 2: (4 cases)
    label:
    s: 2
    f: null or mock_free
    number_of_entries: 2
    there_is_a_collision: TRUE or FALSE
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP

cases s == 2, number_of_entries == 3: (2 cases)
    label:
    s: 2
    f: null or mock_free
    number_of_entries: 3
    there_is_a_collision: TRUE
    chars_are_dynamically_allocated: TRUE or FALSE
    ht: HASHTABLE_DEFINED_IN_SETUP
    entries: ENTRIES_DEFINED_IN_SETUP
*/

// ----- hashtable_params_t

static const hashtable_params_t hashtable_params_ht_null = {
    .label = "ht == NULL",
    .ht = NULL,
    .config = config_ht_null,
};

static const hashtable_params_t hashtable_params_template_s_1_n_0_f_null = {
    .label = "hashtable size == 1, number of entries == 0, destroy_value_fn = NULL",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = DUMMY_POINTER_VALUE,
    .config = config_s_1_n_0_f_null,
};

static const hashtable_params_t hashtable_params_template_s_1_n_0_f_free = {
    .label = "hashtable size == 1, number of entries == 0, destroy_value_fn = mock_free",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = DUMMY_POINTER_VALUE,
    .config = config_s_1_n_0_f_free,
};

static const hashtable_params_t hashtable_params_template_s_1_n_1_static = {
    .label = "hashtable size == 1, number of entries == 1, static allocation, destroy_value_fn = NULL",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_1_n_1_static,
};

static const hashtable_params_t hashtable_params_template_s_1_n_1_dynamic = {
    .label = "hashtable size == 1, number of entries == 1, dynamic allocation, destroy_value_fn = mock_free",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_1_n_1_dynamic,
};

static const hashtable_params_t hashtable_params_template_s_1_n_2_collision_static = {
    .label = "hashtable size == 1, number of entries == 2, collision, static allocation, destroy_value_fn = NULL",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_1_n_2_collision_static,
};

static const hashtable_params_t hashtable_params_template_s_1_n_2_collision_dynamic = {
    .label = "hashtable size == 1, number of entries == 2, collision, dynamic allocation, destroy_value_fn = mock_free",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_1_n_2_collision_dynamic,
};

static const hashtable_params_t hashtable_params_template_s_2_n_0_f_null = {
    .label = "hashtable size == 2, number of entries == 0, destroy_value_fn = NULL",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = DUMMY_POINTER_VALUE,
    .config = config_s_2_n_0_f_null,
};

static const hashtable_params_t hashtable_params_template_s_2_n_0_f_free = {
    .label = "hashtable size == 2, number of entries == 0, destroy_value_fn = mock_free",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = DUMMY_POINTER_VALUE,
    .config = config_s_2_n_0_f_free,
};

static const hashtable_params_t hashtable_params_template_s_2_n_1_static = {
    .label = "hashtable size == 2, number of entries == 1, static allocation, destroy_value_fn = NULL",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_2_n_1_static,
};

static const hashtable_params_t hashtable_params_template_s_2_n_1_dynamic = {
    .label = "hashtable size == 2, number of entries == 1, dynamic allocation, destroy_value_fn = mock_free",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_2_n_1_dynamic,
};

static const hashtable_params_t hashtable_params_template_s_2_n_2_no_collision_static = {
    .label = "hashtable size == 2, number of entries == 2, no collision, static allocation, destroy_value_fn = NULL",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_2_n_2_no_collision_static,
};

static const hashtable_params_t hashtable_params_template_s_2_n_2_collision_static = {
    .label = "hashtable size == 2, number of entries == 2, collision, static allocation, destroy_value_fn = NULL",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_2_n_2_collision_static,
};

static const hashtable_params_t hashtable_params_template_s_2_n_2_no_collision_dynamic = {
    .label = "hashtable size == 2, number of entries == 2, no collision, dynamic allocation, destroy_value_fn = mock_free",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_2_n_2_no_collision_dynamic,
};

static const hashtable_params_t hashtable_params_template_s_2_n_2_collision_dynamic = {
    .label = "hashtable size == 2, number of entries == 2, collision, dynamic allocation, destroy_value_fn = mock_free",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_2_n_2_collision_dynamic,
};

static const hashtable_params_t hashtable_params_template_s_2_n_3_collision_static = {
    .label = "hashtable size == 2, number of entries == 3, collision, static allocation, destroy_value_fn = NULL",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_2_n_3_collision_static,
};

static const hashtable_params_t hashtable_params_template_s_2_n_3_collision_dynamic = {
    .label = "hashtable size == 2, number of entries == 3, collision, dynamic allocation, destroy_value_fn = mock_free",
    .ht = HASHTABLE_DEFINED_IN_SETUP,
    .entries = ENTRIES_DEFINED_IN_SETUP,
    .config = config_s_2_n_3_collision_dynamic,
};

// ----- key_value_pairs_params_t

static const key_value_pairs_params_t dummy_key_value_pairs_params = {
    .label = "dummy",
    .key_is_already_in_use = DUMMY_BOOLEAN_VALUE,
    .key_value_pairs_to_be_added = DUMMY_KEY_VALUE_PAIRS,
};

static const key_value_pairs_params_t key_value_pairs_params_key_null = {
    .label = "key null",
    .key_is_already_in_use = DUMMY_BOOLEAN_VALUE,
    .key_value_pairs_to_be_added = &key_value_pairs_with_key_null,
};

static const key_value_pairs_params_t key_value_pairs_params_template_key_already_in_use = {
    .label = "key already in use",
    .key_is_already_in_use = TRUE,
    .key_value_pairs_to_be_added = KEY_VALUE_PAIRS_TO_BE_ADDED_DEFINED_IN_SET_UP,
};

static const key_value_pairs_params_t key_value_pairs_params_template_key_not_already_in_use = {
    .label = "key not already in use",
    .key_is_already_in_use = FALSE,
    .key_value_pairs_to_be_added = KEY_VALUE_PAIRS_TO_BE_ADDED_DEFINED_IN_SET_UP,
};

// ----- params_t

static const params_t params_ht_null = {
    .label = "ht == NULL",
    .hashtable_params = &hashtable_params_ht_null,
    .key_value_pairs_params = &dummy_key_value_pairs_params,
};

static const params_t params_key_null = {
    .label = "key == NULL",
    .hashtable_params = &hashtable_params_template_s_1_n_0_f_null,
    .key_value_pairs_params = &key_value_pairs_params_key_null,
};

static const params_t params_template_s_1_n_0_f_null_key_not_in_use = {
    .label = "hashtable size == 1, number of entries == 0, destroy_value_fn = NULL, key not in use",
    .hashtable_params = &hashtable_params_template_s_1_n_0_f_null,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_1_n_0_f_free_key_not_in_use = {
    .label = "hashtable size == 1, number of entries == 0, destroy_value_fn = mock_free, key not in use",
    .hashtable_params = &hashtable_params_template_s_1_n_0_f_free,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_1_n_1_static_key_in_use = {
    .label = "hashtable size == 1, number of entries == 1, static allocation, destroy_value_fn = NULL, key in use",
    .hashtable_params = &hashtable_params_template_s_1_n_1_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_1_n_1_static_key_not_in_use = {
    .label = "hashtable size == 1, number of entries == 1, static allocation, destroy_value_fn = NULL, key not in use",
    .hashtable_params = &hashtable_params_template_s_1_n_1_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_1_n_1_dynamic_key_in_use = {
    .label = ", key in use",
    .hashtable_params = &hashtable_params_template_s_1_n_1_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_1_n_1_dynamic_key_not_in_use = {
    .label = ", key not in use",
    .hashtable_params = &hashtable_params_template_s_1_n_1_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_1_n_2_collision_static_key_in_use = {
    .label = "hashtable size == 1, number of entries == 2, collision, static allocation, destroy_value_fn = NULL, key in use",
    .hashtable_params = &hashtable_params_template_s_1_n_2_collision_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_1_n_2_collision_static_key_not_in_use = {
    .label = "hashtable size == 1, number of entries == 2, collision, static allocation, destroy_value_fn = NULL, key not in use",
    .hashtable_params = &hashtable_params_template_s_1_n_2_collision_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_1_n_2_collision_dynamic_key_in_use = {
    .label = "hashtable size == 1, number of entries == 2, collision, dynamic allocation, destroy_value_fn = mock_free, key in use",
    .hashtable_params = &hashtable_params_template_s_1_n_2_collision_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_1_n_2_collision_dynamic_key_not_in_use = {
    .label = "hashtable size == 1, number of entries == 2, collision, dynamic allocation, destroy_value_fn = mock_free, key not in use",
    .hashtable_params = &hashtable_params_template_s_1_n_2_collision_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_0_f_null_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 0, destroy_value_fn = NULL, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_0_f_null,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_0_f_free_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 0, destroy_value_fn = mock_free, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_0_f_free,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_1_static_key_in_use = {
    .label = "hashtable size == 2, number of entries == 1, static allocation, destroy_value_fn = NULL, key in use",
    .hashtable_params = &hashtable_params_template_s_2_n_1_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_2_n_1_static_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 1, static allocation, destroy_value_fn = NULL, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_1_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_1_dynamic_key_in_use = {
    .label = "hashtable size == 2, number of entries == 1, dynamic allocation, destroy_value_fn = mock_free, key in use",
    .hashtable_params = &hashtable_params_template_s_2_n_1_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_2_n_1_dynamic_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 1, dynamic allocation, destroy_value_fn = mock_free, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_1_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_2_no_collision_static_key_in_use = {
    .label = "hashtable size == 2, number of entries == 2, no collision, static allocation, destroy_value_fn = NULL, key in use",
    .hashtable_params = &hashtable_params_template_s_2_n_2_no_collision_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_2_n_2_no_collision_static_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 2, no collision, static allocation, destroy_value_fn = NULL, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_2_no_collision_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_2_collision_static_key_in_use = {
    .label = "hashtable size == 2, number of entries == 2, collision, static allocation, destroy_value_fn = NULL, key in use",
    .hashtable_params = &hashtable_params_template_s_2_n_2_collision_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_2_n_2_collision_static_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 2, collision, static allocation, destroy_value_fn = NULL, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_2_collision_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_2_no_collision_dynamic_key_in_use = {
    .label = "hashtable size == 2, number of entries == 2, no collision, dynamic allocation, destroy_value_fn = mock_free, key in use",
    .hashtable_params = &hashtable_params_template_s_2_n_2_no_collision_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_2_n_2_no_collision_dynamic_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 2, no collision, dynamic allocation, destroy_value_fn = mock_free, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_2_no_collision_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_2_collision_dynamic_key_in_use = {
    .label = "hashtable size == 2, number of entries == 2, collision, dynamic allocation, destroy_value_fn = mock_free, key in use",
    .hashtable_params = &hashtable_params_template_s_2_n_2_collision_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_2_n_2_collision_dynamic_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 2, collision, dynamic allocation, destroy_value_fn = mock_free, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_2_collision_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_3_collision_static_key_in_use = {
    .label = "hashtable size == 2, number of entries == 3, collision, static allocation, destroy_value_fn = NULL, key in use",
    .hashtable_params = &hashtable_params_template_s_2_n_3_collision_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_2_n_3_collision_static_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 3, collision, static allocation, destroy_value_fn = NULL, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_3_collision_static,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};

static const params_t params_template_s_2_n_3_collision_dynamic_key_in_use = {
    .label = "hashtable size == 2, number of entries == 3, collision, dynamic allocation, destroy_value_fn = mock_freehashtable size == 2, number of entries == 3, collision, dynamic allocation, destroy_value_fn = mock_free, key in use",
    .hashtable_params = &hashtable_params_template_s_2_n_3_collision_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_already_in_use,
};

static const params_t params_template_s_2_n_3_collision_dynamic_key_not_in_use = {
    .label = "hashtable size == 2, number of entries == 3, collision, dynamic allocation, destroy_value_fn = mock_free, key not in use",
    .hashtable_params = &hashtable_params_template_s_2_n_3_collision_dynamic,
    .key_value_pairs_params = &key_value_pairs_params_template_key_not_already_in_use,
};



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

static void initialize_hashtable(hashtable_params_t *params) {
	size_t s = (params->config).s;
	hashtable *ht = NULL;
	if (s > 0) {
        alloc_and_save_address_to_be_freed((void *)&ht, sizeof(hashtable));
		assert_non_null(ht);
		ht->size = s;
	    ht->key_type = HASHTABLE_KEY_TYPE_STRING;
        ht->destroy_value_fn = (params->config).f;
		alloc_and_save_address_to_be_freed((void *)&(ht->buckets), sizeof(list) * s);
		assert_non_null(ht->buckets);
		for (size_t i = 0; i < s; ++i) {
			ht->buckets[i] = NULL;
		}
	}
	params->ht = ht;
}

static void initialize_entries(hashtable_params_t *params) {
    int n = (params->config).number_of_entries;
	entry **entries = NULL;
    if (n > 0) {
        alloc_and_save_address_to_be_freed((void *)&entries, n * sizeof(entry *));
    	assert_non_null(entries);
    	for (size_t i = 0; i < n; i++) {
            alloc_and_save_address_to_be_freed((void*)&(entries[i]), sizeof(entry));;
            assert_non_null(entries[i]);
            alloc_and_save_address_to_be_freed((void*)&(entries[i]->key), strlen(KEY_VALUES[i])+1);
            memcpy((void *) entries[i]->key, KEY_VALUES[i], strlen(KEY_VALUES[i])+1);
        	//assert_non_null(entries[i]->key);
        	if ((params->config).chars_are_dynamically_allocated) {
                alloc_and_save_address_to_be_freed((void*)&(entries[i]->value), sizeof(char));
            	assert_non_null(entries[i]->value);
            	* (char *) (entries[i]->value) = STATIC_CHARS[i];
        	}
        	else {
            	entries[i]->value = &STATIC_CHARS[i];
        	}
    	}
	}
	params->entries = entries;
}

static void put_an_entry_in_hashtable(hashtable *ht, entry **entries, size_t entry_index, size_t bucket_index) {
    cons *c = NULL;
    alloc_and_save_address_to_be_freed((void*)&c, sizeof(cons));
    assert_non_null(c);
    c->car = entries[entry_index];
    c->cdr = ht->buckets[bucket_index];
    ht->buckets[bucket_index] = c;
}

static void put_entries_in_hashtable(hashtable_params_t *params) {
    hashtable *ht = params->ht;
    assert_non_null(ht);
    int number_of_entries = (params->config).number_of_entries;
    if (number_of_entries == 0)
        return;
    entry **entries = params->entries;
    if (number_of_entries >= 1) {
        put_an_entry_in_hashtable(ht, entries, 0, 0);
    }
    if (number_of_entries >= 2) {
        if ((params->config).there_is_a_collision) {
            put_an_entry_in_hashtable(ht, entries, 1, 0);
        }
        else {
            put_an_entry_in_hashtable(ht, entries, 1, 1);
        }
    }
    if (number_of_entries >= 3) {
		assert((params->config).s == 2);
		assert((params->config).there_is_a_collision);
        put_an_entry_in_hashtable(ht, entries, 2, 1);
    }
}

static void initialize_key_value_pairs_to_be_added(key_value_pairs_params_t *params) {
    key_value_pair *key_value_pairs_to_be_added = NULL;
    alloc_and_save_address_to_be_freed((void*)&key_value_pairs_to_be_added, sizeof(key_value_pair));
    size_t index = params->key_is_already_in_use ? INDEX_OF_A_KEY_IN_USE : INDEX_OF_A_KEY_NOT_IN_USE;
    alloc_and_save_address_to_be_freed((void*)&(key_value_pairs_to_be_added->key), strlen(KEY_VALUES[index])+1);
    memcpy((void*)key_value_pairs_to_be_added->key, KEY_VALUES[index], strlen(KEY_VALUES[index])+1);
    alloc_and_save_address_to_be_freed((void*)&(key_value_pairs_to_be_added->value), sizeof(char));
    *((char *) key_value_pairs_to_be_added->value) = STATIC_CHARS[DUMMY_INDEX_OF_A_DUMMY_KEY];
    params->key_value_pairs_to_be_added = key_value_pairs_to_be_added;
}



//-----------------------------------------------------------------------------
// GENERAL FIXTURES
//-----------------------------------------------------------------------------


static int hashtable_setup(void **state) { // *state is a hashtable_params_t *
	const hashtable_params_t *model = *state;
    hashtable_params_t *params = NULL;
    alloc_and_save_address_to_be_freed((void*)&params, sizeof(hashtable_params_t));
	*params = *model;
	if (model->ht) {
		initialize_hashtable(params);
		initialize_entries(params);
		put_entries_in_hashtable(params);
	}
    *state = params;
    return 0;
}

static int key_value_pairs_to_be_added_setup(void **state) { // *state is a key_value_pairs_params_t *
    const key_value_pairs_params_t *model = *state;
    key_value_pairs_params_t *params = NULL;
    alloc_and_save_address_to_be_freed((void*)&params, sizeof(key_value_pairs_params_t));
	assert_non_null(params);
	*params = *model;
    if ((params->key_value_pairs_to_be_added != DUMMY_KEY_VALUE_PAIRS) && (model != &key_value_pairs_params_key_null))
        initialize_key_value_pairs_to_be_added(params);
    *state = params;
    return 0;
}

static int general_teardown(void **state) { // *state is a hashtable_params_t *
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
// hashtable_create TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
    const char *label;
    size_t s;
    hashtable_destroy_value_fn_t f;
} create_test_params_t;



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------


static size_t param_s(void **state) {
    return ((create_test_params_t *) *state)->s;
}

static hashtable_destroy_value_fn_t param_f(void **state) {
    return ((create_test_params_t *) *state)->f;
}



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------


static const create_test_params_t create_s_0_f_null = {
    .label = "s == 0, f == NULL",
    .s = 0,
    .f = NULL,
};

static const create_test_params_t create_s_0_f_dummy_not_null = {
    .label = "s == 0, f == dummy not null",
    .s = 0,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};

static const create_test_params_t create_s_1_f_null = {
    .label = "s == 1, f == NULL",
    .s = 1,
    .f = NULL,
};

static const create_test_params_t create_s_1_f_dummy_not_null = {
    .label = "s == 1, f == dummy not null",
    .s = 1,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};

static const create_test_params_t create_s_2_f_null = {
    .label = "s == 2, f == NULL",
    .s = 2,
    .f = NULL,
};

static const create_test_params_t create_s_2_f_dummy_not_null = {
    .label = "s == 2, f == dummy not null",
    .s = 2,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_teardown(void **state) {
    set_allocators(NULL, NULL);
    list_free_list(collected_ptr_to_be_freed, destroy_fn_with_current_free, NULL);
    collected_ptr_to_be_freed = NULL;
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: s == 0
// Expected: returns NULL
// param:
//	- create_s_0_f_null
//	- create_s_0_f_dummy_not_null
static void create_returns_null_when_s_0(void **state) {
    assert_null(hashtable_create(param_s(state), STUB_KEY_TYPE, param_f(state)));
}

// Given: s == 0
// Expected: calls neither DATA_STRUCTURE_MALLOC nor DATA_STRUCTURE_FREE
// param:
//	- create_s_0_f_null
//	- create_s_0_f_dummy_not_null
static void create_calls_neither_malloc_nor_free_when_s_0(void **state) {
    hashtable_create(param_s(state), STUB_KEY_TYPE, param_f(state));
}

// Given: s > 0
// Expected: calls DATA_STRUCTURE_MALLOC(sizeof(hashtable)) for hashtable
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_calls_malloc_for_hashtable_with_right_params_when_s_not_0(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid an other mock call
    hashtable_create(param_s(state), STUB_KEY_TYPE, param_f(state));
}

// Given: malloc call for hashtable fail
// Expected: returns NULL
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_returns_null_when_malloc_for_hashtable_fail(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(hashtable_create(param_s(state), STUB_KEY_TYPE, param_f(state)));
}

// Given: malloc call for hashtable success
// Expected: calls DATA_STRUCTURE_MALLOC(size * sizeof(list)) for buckets field
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success(void **state) {
    fake_malloc_returned_value_for_hashtable = malloc(sizeof(hashtable));
    assert_non_null(fake_malloc_returned_value_for_hashtable);
    fake_malloc_returned_value_for_buckets = malloc(param_s(state) * sizeof(list));
    assert_non_null(fake_malloc_returned_value_for_buckets);
    set_allocators(NULL, NULL);
    collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, fake_malloc_returned_value_for_hashtable);
    collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, fake_malloc_returned_value_for_buckets);
    set_allocators(mock_malloc, mock_free);
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable_create(param_s(state), STUB_KEY_TYPE, param_f(state));
}

// Given: malloc call for buckets field fail
// Expected: frees the pointer return by malloc call for hashtable
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
 static void create_frees_hashtable_when_malloc_for_buckets_fail(void **state) {
    alloc_and_save_address_to_be_freed(&fake_malloc_returned_value_for_hashtable, sizeof(hashtable));
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_hashtable);
    hashtable_create(param_s(state), STUB_KEY_TYPE, param_f(state));
}

// Given: malloc call for buckets success
// Expected: returns a hashtable * with correctly initialized buckets field
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success(void **state) {
    alloc_and_save_address_to_be_freed(&fake_malloc_returned_value_for_hashtable, sizeof(hashtable));
    alloc_and_save_address_to_be_freed(&fake_malloc_returned_value_for_buckets, param_s(state) * sizeof(list));
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable *ret = hashtable_create(param_s(state), STUB_KEY_TYPE, param_f(state));
    assert_non_null(ret);
    assert_ptr_equal(ret->buckets, fake_malloc_returned_value_for_buckets);
    size_t memory_area_size = param_s(state) * sizeof(list);
    unsigned char zeros[memory_area_size];
    memset(zeros, 0, memory_area_size);
    assert_memory_equal(ret->buckets, zeros, memory_area_size);
}

// Given: malloc call for buckets success
// Expected: returns a hashtable * with correctly initialized size field
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success(void **state) {
    alloc_and_save_address_to_be_freed(&fake_malloc_returned_value_for_hashtable, sizeof(hashtable));
    alloc_and_save_address_to_be_freed(&fake_malloc_returned_value_for_buckets, param_s(state) * sizeof(list));
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable *ret = hashtable_create(param_s(state), STUB_KEY_TYPE, param_f(state));
    assert_non_null(ret);
    assert_int_equal(ret->size, param_s(state));
}

// Given: malloc call for buckets success
// Expected: returns a hashtable * with correctly initialized destroy_value_fn field
// param:
//	- create_s_1_f_null
//	- create_s_1_f_dummy_not_null
//	- create_s_2_f_null
//	- create_s_2_f_dummy_not_null
static void create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success(void **state) {
    alloc_and_save_address_to_be_freed(&fake_malloc_returned_value_for_hashtable, sizeof(hashtable));
    alloc_and_save_address_to_be_freed(&fake_malloc_returned_value_for_buckets, param_s(state) * sizeof(list));
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(list));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable *ret = hashtable_create(param_s(state), STUB_KEY_TYPE, param_f(state));
    assert_non_null(ret);
    assert_ptr_equal(ret->destroy_value_fn, param_f(state));
}



//-----------------------------------------------------------------------------
// destroy TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_setup(void **state) { // *state is a hashtable_params_t *
    int ret = hashtable_setup(state);
    set_allocators(mock_malloc, mock_free);
    return ret;
}

// note: general_teardown will be used



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: ht == NULL
// Expected: does not call free
// param:
//	- hashtable_params_ht_null
static void destroy_does_not_call_free_when_s_0(void **state) {
    hashtable_destroy(((hashtable_params_t*)*state)->ht);
}

// Given: ht size == 1 or 2, no entry (empty buckets)
// Expected: frees
//	- ht->buckets
//	- ht
// param:
//	- hashtable_params_template_s_1_n_0_f_null
//	- hashtable_params_template_s_1_n_0_f_free
//	- hashtable_params_template_s_2_n_0_f_null
//	- hashtable_params_template_s_2_n_0_f_free
static void destroy_frees_hashtable_when_no_entries(void **state) {
    hashtable *ht = ((hashtable_params_t*)*state)->ht;
	expect_value(mock_free, ptr, ht->buckets);
	expect_value(mock_free, ptr, ht);
    hashtable_destroy(ht);
}

// Given: ht size == 1 or 2, one entry at first bucket for a char statically allocated
// Expected: frees
//	- ((entry *) ((ht->buckets)[0])->car)->key
//  - ((ht->buckets)[0])->car
//	- (ht->buckets)[0]
//	- ht->buckets
//	- ht
// param:
//	- hashtable_params_template_s_1_n_1_static
//	- hashtable_params_template_s_2_n_1_static
static void destroy_frees_hashtable_when_one_entry_at_first_bucket_with_a_static_char(void **state) {
    hashtable *ht = ((hashtable_params_t*)*state)->ht;
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->key); // frees key of entry
    expect_value(mock_free, ptr, ((ht->buckets)[0])->car); // frees entry
    expect_value(mock_free, ptr, (ht->buckets)[0]); // frees list which contained entry
	expect_value(mock_free, ptr, ht->buckets); // frees buckets
	expect_value(mock_free, ptr, ht); // frees hashtable
    hashtable_destroy(ht);
}

// Given: ht size == 1 or 2, one entry at first bucket for a char dynamically allocated
// Expected: frees
//	- ((entry *) ((ht->buckets)[0])->car)->key
//	- ((entry *) ((ht->buckets)[0])->car)->value
//	- (ht->buckets)[0]
//	- ht->buckets
//	- ht
// param:
//	- hashtable_params_template_s_1_n_1_dynamic
//	- hashtable_params_template_s_2_n_1_dynamic
static void destroy_frees_hashtable_when_one_entry_at_first_bucket_with_a_dynamic_char(void **state) {
    hashtable *ht = ((hashtable_params_t*)*state)->ht;
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->key); // frees key of entry
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->value); // frees value of entry
    expect_value(mock_free, ptr, ((ht->buckets)[0])->car); // frees entry
    expect_value(mock_free, ptr, (ht->buckets)[0]); // frees list which contained entry
	expect_value(mock_free, ptr, ht->buckets); // frees buckets
	expect_value(mock_free, ptr, ht); // frees hashtable
    hashtable_destroy(ht);
}

// Given: ht size == 2, 2 entries for chars statically allocated, no collision
// Expected: frees
//	- ((entry *) ((ht->buckets)[0])->car)->key
//	- (ht->buckets)[0]
//	- ((entry *) ((ht->buckets)[1])->car)->key
//	- (ht->buckets)[1]
//	- ht->buckets
//	- ht
// param:
//	- hashtable_params_template_s_2_n_2_no_collision_static
static void destroy_frees_hashtable_when_s_2_two_entries_with_a_static_char_no_collision(void **state) {
    hashtable *ht = ((hashtable_params_t*)*state)->ht;
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->key); // frees key of entry at first bucket
    expect_value(mock_free, ptr, ((ht->buckets)[0])->car); // frees entry at first bucket
    expect_value(mock_free, ptr, (ht->buckets)[0]); // frees list at first bucket
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[1])->car)->key); // frees key of entry at second bucket
    expect_value(mock_free, ptr, ((ht->buckets)[1])->car); // frees entry at second bucket
    expect_value(mock_free, ptr, (ht->buckets)[1]); // frees list at second bucket
	expect_value(mock_free, ptr, ht->buckets); // frees buckets
	expect_value(mock_free, ptr, ht); // frees hashtable
    hashtable_destroy(ht);
}

// Given: ht size == 2, 2 entries for chars dynamically allocated, no collision
// Expected: frees
//	- ((entry *) ((ht->buckets)[0])->car)->key
//	- ((entry *) ((ht->buckets)[0])->car)->value
//	- (ht->buckets)[0]
//	- ((entry *) ((ht->buckets)[1])->car)->key
//	- ((entry *) ((ht->buckets)[1])->car)->value
//	- (ht->buckets)[1]
//	- ht->buckets
//	- ht
// param:
//	- hashtable_params_template_s_2_n_2_no_collision_dynamic
static void destroy_frees_hashtable_when_s_2_two_entries_with_a_dynamic_char_no_collision(void **state) {
    hashtable *ht = ((hashtable_params_t*)*state)->ht;
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->key); // frees key of entry at first bucket
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->value); // frees value of entry at first bucket
    expect_value(mock_free, ptr, ((ht->buckets)[0])->car); // frees entry at first bucket
    expect_value(mock_free, ptr, (ht->buckets)[0]); // frees list at first bucket
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[1])->car)->key); // frees key of entry at second bucket
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[1])->car)->value); // frees value of entry at second bucket
    expect_value(mock_free, ptr, ((ht->buckets)[1])->car); // frees entry at second bucket
    expect_value(mock_free, ptr, (ht->buckets)[1]); // frees list at second bucket
	expect_value(mock_free, ptr, ht->buckets); // frees buckets
	expect_value(mock_free, ptr, ht); // frees hashtable
    hashtable_destroy(ht);
}

// Given: ht size == 1, 2 entries (hence collision)
// Expected: frees
//	- ((entry *) ((ht->buckets)[0])->car)->key
//  - if chars are dynamically allocated: ((entry *) ((ht->buckets)[0])->car)->value
//	- ((ht->buckets)[0])->car
//  - (ht->buckets)[0]
//	- ((entry *) (((ht->buckets)[0])->cdr)->car)->key
//  - if chars are dynamically allocated: ((entry *) (((ht->buckets)[0])->cdr)->car)->value
//  - (((ht->buckets)[0])->cdr)->car
//	- ((ht->buckets)[0])->cdr
//	- ht->buckets
//	- ht
// param:
//	- hashtable_params_template_s_1_n_2_collision_static
//  - hashtable_params_template_s_1_n_2_collision_dynamic
static void destroy_frees_hashtable_when_s_1_two_entries_hence_collision(void **state) {
    hashtable *ht = ((hashtable_params_t*)*state)->ht;
    boolean chars_are_dynamically_allocated = (((hashtable_params_t*)*state)->config).chars_are_dynamically_allocated;
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->key); // frees key of first entry at first bucket
    if (chars_are_dynamically_allocated)
        expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->value); // frees value of first entry at first bucket
    expect_value(mock_free, ptr, ((ht->buckets)[0])->car); // frees first entry at first bucket
    expect_value(mock_free, ptr, (ht->buckets)[0]); // frees first element of the list at first bucket
    expect_value(mock_free, ptr, ((entry *) (((ht->buckets)[0])->cdr)->car)->key); // frees key of second entry at first bucket
    if (chars_are_dynamically_allocated)
        expect_value(mock_free, ptr, ((entry *) (((ht->buckets)[0])->cdr)->car)->value); // frees value of second entry at first bucket
    expect_value(mock_free, ptr, (((ht->buckets)[0])->cdr)->car); // frees second entry at first bucket
    expect_value(mock_free, ptr, ((ht->buckets)[0])->cdr); // frees second element of the list at first bucket
	expect_value(mock_free, ptr, ht->buckets); // frees buckets
	expect_value(mock_free, ptr, ht); // frees hashtable
    hashtable_destroy(ht);
}

// Given: ht size == 2, 2 entries, collision at first bucket
// Expected: frees
//	- ((entry *) ((ht->buckets)[0])->car)->key
//  - if chars are dynamically allocated: ((entry *) ((ht->buckets)[0])->car)->value
//	- ((ht->buckets)[0])->car
//  - (ht->buckets)[0]
//	- ((entry *) (((ht->buckets)[0])->cdr)->car)->key
//  - if chars are dynamically allocated: ((entry *) (((ht->buckets)[0])->cdr)->car)->value
//  - (((ht->buckets)[0])->cdr)->car
//	- ((ht->buckets)[0])->cdr
//	- ht->buckets
//	- ht
// param:
//	- hashtable_params_template_s_2_n_2_collision_static
//  - hashtable_params_template_s_2_n_2_collision_dynamic
static void destroy_frees_hashtable_when_s_2_two_entries_collision_at_first_bucket(void **state) {
    hashtable *ht = ((hashtable_params_t*)*state)->ht;
    boolean chars_are_dynamically_allocated = (((hashtable_params_t*)*state)->config).chars_are_dynamically_allocated;
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->key); // frees key of first entry at first bucket
    if (chars_are_dynamically_allocated)
        expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->value); // frees value of first entry at first bucket
    expect_value(mock_free, ptr, ((ht->buckets)[0])->car); // frees first entry at first bucket
    expect_value(mock_free, ptr, (ht->buckets)[0]); // frees first element of the list at first bucket
    expect_value(mock_free, ptr, ((entry *) (((ht->buckets)[0])->cdr)->car)->key); // frees key of second entry at first bucket
    if (chars_are_dynamically_allocated)
        expect_value(mock_free, ptr, ((entry *) (((ht->buckets)[0])->cdr)->car)->value); // frees value of second entry at first bucket
    expect_value(mock_free, ptr, (((ht->buckets)[0])->cdr)->car); // frees second entry at first bucket
    expect_value(mock_free, ptr, ((ht->buckets)[0])->cdr); // frees second element of the list at first bucket
	expect_value(mock_free, ptr, ht->buckets); // frees buckets
	expect_value(mock_free, ptr, ht); // frees hashtable
    hashtable_destroy(ht);
}

// Given: ht size == 2, 3 entries, collision at first bucket
// Expected: frees
//	- ((entry *) ((ht->buckets)[0])->car)->key
//  - if chars are dynamically allocated: ((entry *) ((ht->buckets)[0])->car)->value
//	- ((ht->buckets)[0])->car
//  - (ht->buckets)[0]
//	- ((entry *) (((ht->buckets)[0])->cdr)->car)->key
//  - if chars are dynamically allocated: ((entry *) (((ht->buckets)[0])->cdr)->car)->value
//  - (((ht->buckets)[0])->cdr)->car
//	- ((ht->buckets)[0])->cdr
//	- ((entry *) ((ht->buckets)[1])->car)->key
//	- if chars are dynamically allocated: ((entry *) ((ht->buckets)[1])->car)->value
//	- (ht->buckets)[1])->car
//	- (ht->buckets)[1]
//	- ht->buckets
//	- ht
// param:
//	- hashtable_params_template_s_2_n_3_collision_static
//	- hashtable_params_template_s_2_n_3_collision_dynamic
static void destroy_frees_hashtable_when_s_2_three_entries_collision_at_first_bucket(void **state) {
    hashtable *ht = ((hashtable_params_t*)*state)->ht;
    boolean chars_are_dynamically_allocated = (((hashtable_params_t*)*state)->config).chars_are_dynamically_allocated;
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->key); // frees key of first entry at first bucket
    if (chars_are_dynamically_allocated)
        expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[0])->car)->value); // frees value of first entry at first bucket
    expect_value(mock_free, ptr, ((ht->buckets)[0])->car); // frees first entry at first bucket
    expect_value(mock_free, ptr, (ht->buckets)[0]); // frees first element of the list at first bucket
    expect_value(mock_free, ptr, ((entry *) (((ht->buckets)[0])->cdr)->car)->key); // frees key of second entry at first bucket
    if (chars_are_dynamically_allocated)
        expect_value(mock_free, ptr, ((entry *) (((ht->buckets)[0])->cdr)->car)->value); // frees value of second entry at first bucket
    expect_value(mock_free, ptr, (((ht->buckets)[0])->cdr)->car); // frees second entry at first bucket
    expect_value(mock_free, ptr, ((ht->buckets)[0])->cdr); // frees second element of the list at first bucket expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[1])->car)->key); // frees key of first entry at second bucket
    expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[1])->car)->key); // frees key of first entry at second bucket
    if (chars_are_dynamically_allocated)
        expect_value(mock_free, ptr, ((entry *) ((ht->buckets)[1])->car)->value); // frees value of first entry at second bucket
    expect_value(mock_free, ptr, ((ht->buckets)[1])->car); // frees first entry at second bucket
    expect_value(mock_free, ptr, (ht->buckets)[1]); // frees first element of the list at second bucket
	expect_value(mock_free, ptr, ht->buckets); // frees buckets
	expect_value(mock_free, ptr, ht); // frees hashtable
    hashtable_destroy(ht);
}



//-----------------------------------------------------------------------------
// hashtable_key_is_in_use TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int key_is_in_use_setup(void **state) { // *state is a params_t
	const params_t *model = *state;
    params_instance_t *params = NULL;
    alloc_and_save_address_to_be_freed((void*)&params, sizeof(params_instance_t));
    memcpy(params, model, sizeof(params_t));
    *state = params;
    int ret = 0;
    ret += hashtable_setup((void**)&(((params_instance_t *)*state)->hashtable_params));
    ret += key_value_pairs_to_be_added_setup((void**)&(((params_instance_t *)*state)->key_value_pairs_params));
    set_allocators(mock_malloc, mock_free);
    return ret;
}

// note: general_teardown will be used


//-----------------------------------------------------------------------------
// HELPER
//-----------------------------------------------------------------------------


static void expect_return(int ret, void **state) {
    params_t *params = (params_t *) *state;
    assert_int_equal(
        ret,
        hashtable_key_is_in_use(
            params->hashtable_params->ht,
            params->key_value_pairs_params->key_value_pairs_to_be_added->key ) );
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: ht == NULL
// Expected: returns 0
// param:
//	- params_ht_null
static void key_is_in_use_return_0_when_ht_null(void **state) {
    expect_return(0, state);
}

// Given: ht != NULL, key = NULL
// Expected: returns 0
static void key_is_in_use_return_0_when_ht_not_null_key_null(void **state) {
    assert_int_equal(hashtable_key_is_in_use((hashtable *) DUMMY_HASHTABLE_P, NULL), 0);
}

// Given: argument key already in use (==  A_KEY_IN_USE (A_KEY_IN_USE pointing to "key_for_A"), ht = 1 or 2, nb of entries = 1,2 or 3
// Expected: returns 1
// param:
//	- params_template_s_1_n_1_static_key_in_use
//	- params_template_s_1_n_1_dynamic_key_in_use
//	- params_template_s_1_n_2_collision_static_key_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_in_use
//	- params_template_s_2_n_1_static_key_in_use
//	- params_template_s_2_n_1_dynamic_key_in_use
//	- params_template_s_2_n_2_no_collision_static_key_in_use
//	- params_template_s_2_n_2_collision_static_key_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_in_use
//	- params_template_s_2_n_3_collision_static_key_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_in_use
static void key_is_in_use_returns_1_when_key_already_in_use(void **state) {
    expect_return(1, state);
}

// Given: argument key not already in use (==  A_KEY_NOT_IN_USE (A_KEY_NOT_IN_USE pointing to "a key not in use"), ht = 1 or 2, nb of entries = 0,1,2 or 3
// Expected:
// param:
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
static void key_is_in_use_returns_0_when_key_not_already_in_use(void **state) {
    expect_return(0, state);
}



//-----------------------------------------------------------------------------
// hashtable_get TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int get_setup(void **state) { // *state is a hashtable_params_t *
    int ret = 0;
    ret += hashtable_setup(state);
    entry **entries = ((hashtable_params_t*) *state)->entries;
    if (entries && entries[0])
        VALUE_CORRESPONDING_TO_A_KEY_IN_USE = entries[0]->value;
    set_allocators(mock_malloc, mock_free);
    return ret;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: ht == NULL
// Expected: returns NULL
// param:
//	- hashtable_params_ht_null
static void get_returns_null_when_ht_null(void **state) {
    hashtable_get(((hashtable_params_t*)*state)->ht, DUMMY_KEY);
}

// Given: ht != NULL, key = NULL
// Expected: returns NULL
static void get_returns_null_when_ht_not_null_key_null(void **state) {
	assert_null(hashtable_get((hashtable *) DUMMY_HASHTABLE_P, NULL));
}

// Given: ht == 1 or 2, argument key not in use
// Expected: returns NULL
// param:
//	- hashtable_params_template_s_1_n_0_f_null
//	- hashtable_params_template_s_1_n_0_f_free
//	- hashtable_params_template_s_1_n_1_static
//	- hashtable_params_template_s_1_n_1_dynamic
//	- hashtable_params_template_s_1_n_2_collision_static
//	- hashtable_params_template_s_1_n_2_collision_dynamic
//	- hashtable_params_template_s_2_n_0_f_null
//	- hashtable_params_template_s_2_n_0_f_free
//	- hashtable_params_template_s_2_n_1_static
//	- hashtable_params_template_s_2_n_1_dynamic
//	- hashtable_params_template_s_2_n_2_no_collision_static
//	- hashtable_params_template_s_2_n_2_collision_static
//	- hashtable_params_template_s_2_n_2_no_collision_dynamic
//	- hashtable_params_template_s_2_n_2_collision_dynamic
//	- hashtable_params_template_s_2_n_3_collision_static
//	- hashtable_params_template_s_2_n_3_collision_dynamic
static void get_returns_null_when_ht_does_not_contain_key(void **state) {
    assert_null(hashtable_get(((hashtable_params_t*)*state)->ht, A_KEY_NOT_IN_USE));
}

// Given: ht == 1 or 2, nb of entries > 0, argument key ==  A_KEY_IN_USE (A_KEY_IN_USE pointing to "key_for_A")
// Expected: returns entries[0]->value
// param:
//	- hashtable_params_template_s_1_n_1_static
//	- hashtable_params_template_s_1_n_1_dynamic
//	- hashtable_params_template_s_1_n_2_collision_static
//	- hashtable_params_template_s_1_n_2_collision_dynamic
//	- hashtable_params_template_s_2_n_1_static
//	- hashtable_params_template_s_2_n_1_dynamic
//	- hashtable_params_template_s_2_n_2_no_collision_static
//	- hashtable_params_template_s_2_n_2_collision_static
//	- hashtable_params_template_s_2_n_2_no_collision_dynamic
//	- hashtable_params_template_s_2_n_2_collision_dynamic
//	- hashtable_params_template_s_2_n_3_collision_static
//	- hashtable_params_template_s_2_n_3_collision_dynamic
static void get_returns_right_value_when_ht_contains_key(void **state) {
    assert_ptr_equal(
        hashtable_get(
            ((hashtable_params_t*) *state)->ht,
            A_KEY_IN_USE),
        VALUE_CORRESPONDING_TO_A_KEY_IN_USE
    );
}



//-----------------------------------------------------------------------------
// hashtable_add TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int add_setup(void **state) { // *state is a params_t
	const params_t *model = *state;
    params_instance_t *params = NULL;
    alloc_and_save_address_to_be_freed((void*)&params, sizeof(params_instance_t));
    memcpy(params, model, sizeof(params_t));
    *state = params;
    int ret = 0;
    ret += hashtable_setup((void**)&(((params_instance_t *)*state)->hashtable_params));
    ret += key_value_pairs_to_be_added_setup((void**)&(((params_instance_t *)*state)->key_value_pairs_params));
    set_allocators(mock_malloc, mock_free);
    set_string_duplicate(mock_strdup);
    return ret;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: ht == NULL
// Expected: returns 1
// param:
//	- params_ht_null
static void add_returns_1_when_ht_null(void **state) {
    params_t *params = (params_t *) *state;
    const key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;
    int ret = 0;
    ret = hashtable_add(
        params->hashtable_params->ht,
        key_value_pairs_to_be_added->key,
        key_value_pairs_to_be_added->value
    );
    assert_int_equal(ret, 1);
}

// Given: key is NULL
// Expected: returns 1
// param:
//	- params_key_null
static void add_returns_1_when_key_null(void **state) {
    params_t *params = (params_t *) *state;
    const key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;
    int ret = 0;
    ret = hashtable_add(
        params->hashtable_params->ht,
        key_value_pairs_to_be_added->key,
        key_value_pairs_to_be_added->value
    );
    assert_int_equal(ret, 1);
}

// Given: argument key already in use (==  A_KEY_IN_USE (A_KEY_IN_USE pointing to "key_for_A"), ht = 1 or 2, nb of entries = 1,2 or 3
// Expected: returns 1
// param:
//	- params_t params_template_s_1_n_1_static_key_in_use
//	- params_template_s_1_n_1_dynamic_key_in_use
//	- params_template_s_1_n_2_collision_static_key_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_in_use
//	- params_template_s_2_n_1_static_key_in_use
//	- params_template_s_2_n_1_dynamic_key_in_use
//	- params_template_s_2_n_2_no_collision_static_key_in_use
//	- params_template_s_2_n_2_collision_static_key_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_in_use
//	- params_template_s_2_n_3_collision_static_key_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_in_use
static void add_returns_1_when_key_already_in_use(void **state) {
    params_t *params = (params_t *) *state;
    int ret = 0;
    key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;
    ret = hashtable_add(
        params->hashtable_params->ht,
        key_value_pairs_to_be_added->key,
        key_value_pairs_to_be_added->value
    );
    assert_int_equal(ret, 1);
}

// Given: argument key not already in use (==  A_KEY_NOT_IN_USE (A_KEY_NOT_IN_USE pointing to "a key not in use"), ht = 1 or 2, nb of entries = 0,1,2 or 3
// Expected: call malloc with sizeof(entry)
// param:
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
static void add_calls_malloc_for_a_new_entry_when_key_not_already_in_use(void **state) {
    params_t *params = (params_t *) *state;
    expect_value(mock_malloc, size, sizeof(entry));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid an other mock call
    key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;
    hashtable_add(
        params->hashtable_params->ht,
        key_value_pairs_to_be_added->key,
        key_value_pairs_to_be_added->value
    );
}

// Given: malloc call for a new entry fail
// Expected: returns 1
// param:
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
static void add_returns_1_when_malloc_for_a_new_entry_fails(void **state) {
    params_t *params = (params_t *) *state;
    expect_value(mock_malloc, size, sizeof(entry));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;
    assert_int_equal(
        1,
        hashtable_add(
            params->hashtable_params->ht,
            key_value_pairs_to_be_added->key,
            key_value_pairs_to_be_added->value ) );
}

// Given: malloc for a new entry succeeds
// Expected: call strdup with params->key_value_pairs_params->key_value_pairs_to_be_added->key
// param:
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
static void add_call_strdup_with_key_arg_when_malloc_for_a_new_entry_succeeds(void **state) {
    params_t *params = (params_t *) *state;
    key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;

    expect_value(mock_malloc, size, sizeof(entry));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);

    expect_value(mock_strdup, s, key_value_pairs_to_be_added->key);
    will_return(mock_strdup, STRDUP_ERROR_CODE); // to avoid some mock calls

    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE);

    hashtable_add(
        params->hashtable_params->ht,
        key_value_pairs_to_be_added->key,
        key_value_pairs_to_be_added->value );
}

// Given: strdup fail
// Expected: returns 1
// param:
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
static void add_returns_1_when_strdup_fails(void **state) {
    params_t *params = (params_t *) *state;
    key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;

    expect_value(mock_malloc, size, sizeof(entry));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);

    expect_value(mock_strdup, s, key_value_pairs_to_be_added->key);
    will_return(mock_strdup, STRDUP_ERROR_CODE);

    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE);

    assert_int_equal(
        1,
        hashtable_add(
            params->hashtable_params->ht,
            key_value_pairs_to_be_added->key,
            key_value_pairs_to_be_added->value ) );
}

// Given: strdup succeeds
// Expected: calls malloc with sizeof(cons)
// param:
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
static void add_calls_malloc_for_a_new_cons_when_strdup_succeeds(void **state) {
    params_t *params = (params_t *) *state;
    key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;

    expect_value(mock_malloc, size, sizeof(entry));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);

    expect_value(mock_strdup, s, key_value_pairs_to_be_added->key);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);

    expect_value(mock_malloc, size, sizeof(cons));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);

    hashtable_add(
        params->hashtable_params->ht,
        key_value_pairs_to_be_added->key,
        key_value_pairs_to_be_added->value );
}

// Given: malloc for new cons fails
// Expected: returns 1
// param:
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
static void add_returns_1_when_malloc_for_new_cons_fails(void **state) {
    params_t *params = (params_t *) *state;
    key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;

    expect_value(mock_malloc, size, sizeof(entry));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);

    expect_value(mock_strdup, s, key_value_pairs_to_be_added->key);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);

    expect_value(mock_malloc, size, sizeof(cons));
    will_return(mock_malloc, MALLOC_ERROR_CODE);

    expect_value(mock_free, ptr, DUMMY_STRDUP_RETURNED_VALUE);
    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE);

    assert_int_equal(
        1,
        hashtable_add(
            params->hashtable_params->ht,
            key_value_pairs_to_be_added->key,
            key_value_pairs_to_be_added->value ) );
}

// Given: malloc for new cons succeeds
// Expected: add a proper new entry in ht
// param:
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
static void add_initializes_new_entry_in_hashtable_when_malloc_for_new_cons_succeeds(void **state) {
    params_t *params = (params_t *) *state;
    key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;

    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_new_entry_in_hashtable, sizeof(entry));
    assert_non_null(fake_malloc_returned_value_for_a_new_entry_in_hashtable);
    memset(fake_malloc_returned_value_for_a_new_entry_in_hashtable, 0, sizeof(entry));

    alloc_and_save_address_to_be_freed((void **)&fake_strdup_returned_value_for_key_of_hashtable, strlen(key_value_pairs_to_be_added->key) + 1);
    assert_non_null(fake_strdup_returned_value_for_key_of_hashtable);
    memset(fake_strdup_returned_value_for_key_of_hashtable, 0, strlen(key_value_pairs_to_be_added->key) + 1);

    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_new_cons_for_a_bucket, sizeof(cons));
    assert_non_null(fake_malloc_returned_value_for_a_new_cons_for_a_bucket);
    memset(fake_malloc_returned_value_for_a_new_cons_for_a_bucket, 0, sizeof(cons));

    expect_value(mock_malloc, size, sizeof(entry));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_new_entry_in_hashtable);

    expect_value(mock_strdup, s, key_value_pairs_to_be_added->key);
    will_return(mock_strdup, fake_strdup_returned_value_for_key_of_hashtable);

    expect_value(mock_malloc, size, sizeof(cons));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_new_cons_for_a_bucket);

    entry expected_entry = {
        .key = fake_strdup_returned_value_for_key_of_hashtable,
        .value = key_value_pairs_to_be_added->value
    };

    hashtable_add(
        params->hashtable_params->ht,
        key_value_pairs_to_be_added->key,
        key_value_pairs_to_be_added->value );

    assert_memory_equal(
        &expected_entry,
        fake_malloc_returned_value_for_a_new_entry_in_hashtable,
        sizeof(entry) );

    assert_memory_equal(
        expected_entry.key,
        ((entry *) fake_malloc_returned_value_for_a_new_entry_in_hashtable)->key,
        strlen(expected_entry.key) + 1 );

    hashtable *ht = params->hashtable_params->ht;

    size_t index = ht->size;
    for (size_t i = 0; i < ht->size; i++) {
        if (ht->buckets[i] == fake_malloc_returned_value_for_a_new_cons_for_a_bucket) {
            index = i;
            break;
        }
    }
    assert_int_not_equal(index, ht->size);
}

// Given: malloc for new cons succeeds
// Expected: returns 0
// param:
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
static void add_return_0_when_malloc_for_new_cons_succeeds(void **state) {
    params_t *params = (params_t *) *state;
    key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;

    expect_value(mock_malloc, size, sizeof(entry));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);

    expect_value(mock_strdup, s, key_value_pairs_to_be_added->key);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);

    expect_value(mock_malloc, size, sizeof(cons));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);

    assert_int_equal(
        0,
        hashtable_add(
            params->hashtable_params->ht,
            key_value_pairs_to_be_added->key,
            key_value_pairs_to_be_added->value ) );
}




//-----------------------------------------------------------------------------
// hashtable_reset_value TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int reset_value_setup(void **state) { // *state is a params_t
	const params_t *model = *state;
    params_instance_t *params = NULL;
    alloc_and_save_address_to_be_freed((void*)&params, sizeof(params_instance_t));
    memcpy(params, model, sizeof(params_t));
    *state = params;
    int ret = 0;
    ret += hashtable_setup((void**)&(((params_instance_t *)*state)->hashtable_params));
    ret += key_value_pairs_to_be_added_setup((void**)&(((params_instance_t *)*state)->key_value_pairs_params));
    set_allocators(mock_malloc, mock_free);
    set_string_duplicate(mock_strdup);
	set_hash_djb2(mock_hash_djb2); // mock hash function to always return 0, so all keys map to bucket 0 (where INDEX_OF_A_KEY_IN_USE is stored)
    return ret;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: ht == NULL
// Expected: returns 1
// param:
//	- params_ht_null
static void reset_value_returns_1_when_ht_null(void **state) {
    params_t *params = (params_t *) *state;
    const key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;
    assert_int_equal(
        1,
        hashtable_reset_value(
            params->hashtable_params->ht,
            key_value_pairs_to_be_added->key,
            key_value_pairs_to_be_added->value ) );
}

// Given: ht != NULL, key = NULL
// Expected: returns 1
static void reset_value_return_null_when_ht_not_null_key_null(void **state) {
    assert_int_equal(hashtable_reset_value((hashtable *) DUMMY_HASHTABLE_P, NULL, DUMMY_VALUE), 1);
}

// Given: ht not null, argument key not already in use (==  A_KEY_NOT_IN_USE (A_KEY_NOT_IN_USE pointing to "a key not in use"), ht = 1 or 2, nb of entries = 0,1,2 or 3
// Expected: call malloc with sizeof(entry)
// param:
//	- params_template_s_1_n_1_static_key_not_in_use
//	- params_template_s_1_n_1_dynamic_key_not_in_use
//	- params_template_s_1_n_2_collision_static_key_not_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_1_static_key_not_in_use
//	- params_template_s_2_n_1_dynamic_key_not_in_use
//	- params_template_s_2_n_2_no_collision_static_key_not_in_use
//	- params_template_s_2_n_2_collision_static_key_not_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_not_in_use
//	- params_template_s_2_n_3_collision_static_key_not_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_not_in_use
//	- params_template_s_1_n_0_f_null_key_not_in_use
//	- params_template_s_1_n_0_f_free_key_not_in_use
//	- params_template_s_2_n_0_f_null_key_not_in_use
//	- params_template_s_2_n_0_f_free_key_not_in_use
static void reset_value_returns_1_when_key_not_in_use(void **state) {
    params_t *params = (params_t *) *state;
    const key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;
    assert_int_equal(
        1,
        hashtable_reset_value(
            params->hashtable_params->ht,
            key_value_pairs_to_be_added->key,
            key_value_pairs_to_be_added->value ) );
}

// Given: ht not null, argument key already in use (==  A_KEY_IN_USE (A_KEY_IN_USE pointing to "key_for_A"), ht = 1 or 2, nb of entries = 1,2 or 3
// Expected:
//  - if ht->destroy_value_fn not NULL calls ht->destroy_value_fn with the value related to key in ht
//  - initialize the value related to key with argument value
//  - returns 0
// param:
//	- params_t params_template_s_1_n_1_static_key_in_use
//	- params_template_s_1_n_1_dynamic_key_in_use
//	- params_template_s_1_n_2_collision_static_key_in_use
//	- params_template_s_1_n_2_collision_dynamic_key_in_use
//	- params_template_s_2_n_1_static_key_in_use
//	- params_template_s_2_n_1_dynamic_key_in_use
//	- params_template_s_2_n_2_no_collision_static_key_in_use
//	- params_template_s_2_n_2_collision_static_key_in_use
//	- params_template_s_2_n_2_no_collision_dynamic_key_in_use
//	- params_template_s_2_n_2_collision_dynamic_key_in_use
//	- params_template_s_2_n_3_collision_static_key_in_use
//	- params_template_s_2_n_3_collision_dynamic_key_in_use
static void reset_value_free_reset_value_and_returns_0_when_key_in_use(void **state) {
    params_t *params = (params_t *) *state;
    hashtable *ht = params->hashtable_params->ht;
    const key_value_pair *key_value_pairs_to_be_added = params->key_value_pairs_params->key_value_pairs_to_be_added;
    const char *key = key_value_pairs_to_be_added->key;
    void *value = key_value_pairs_to_be_added->value;
    void *old_value = hashtable_get((const hashtable*) ht, key); // the mock of hash_djb2 returns index 0
    if (ht->destroy_value_fn)
        expect_value(mock_free, ptr, old_value);
    assert_int_equal(
        0,
        hashtable_reset_value(ht, key, value) );
    assert_ptr_equal(
        value,
        hashtable_get(ht, key) );
}



//-----------------------------------------------------------------------------
// hashtable_remove TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int remove_setup(void **state) { // *state is a hashtable_params_t *
	int ret = hashtable_setup(state);
    set_allocators(mock_malloc, mock_free);
    set_hash_djb2(mock_hash_djb2); // make hash always 0, the index of the bucket where is INDEX_OF_A_KEY_IN_USE
    return ret;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: ht == NULL
// Expected: returns 1
// param:
//	- hashtable_params_ht_null
static void remove_returns_1_when_ht_null(void **state) {
   	assert_int_equal(
        1,
        hashtable_remove(
            ((hashtable_params_t *) *state)->ht,
            DUMMY_KEY ) );
}

// Given: ht != NULL, key = NULL
// Expected: returns 1
static void remove_returns_1_when_when_ht_not_null_key_null(void **state) {
	assert_int_equal(hashtable_remove((hashtable *) DUMMY_HASHTABLE_P, NULL), 1);
}

// Given: ht not null, argument key not already in use (==  A_KEY_NOT_IN_USE (A_KEY_NOT_IN_USE pointing to "a key not in use"), ht = 1 or 2, nb of entries = 0,1,2 or 3
// Expected: returns 1
// param:
//  - hashtable_params_template_s_1_n_1_static,
//  - hashtable_params_template_s_1_n_1_dynamic,
//  - hashtable_params_template_s_1_n_2_collision_static,
//  - hashtable_params_template_s_1_n_2_collision_dynamic,
//  - hashtable_params_template_s_2_n_1_static,
//  - hashtable_params_template_s_2_n_1_dynamic,
//  - hashtable_params_template_s_2_n_2_no_collision_static,
//  - hashtable_params_template_s_2_n_2_collision_static,
//  - hashtable_params_template_s_2_n_2_no_collision_dynamic,
//  - hashtable_params_template_s_2_n_2_collision_dynamic,
//  - hashtable_params_template_s_2_n_3_collision_static,
//  - hashtable_params_template_s_2_n_3_collision_dynamic
static void remove_returns_1_when_key_not_in_use(void **state) {
    assert_int_equal(
        1,
        hashtable_remove(
            ((hashtable_params_t *) *state)->ht,
            A_KEY_NOT_IN_USE ) );
}

// Given: The hashtable is not empty, and the provided key ("key_for_A") matches the key of the entry at the head of bucket 0.
// Expected: (remove first entry of bucket 0)
//  - If ht->destroy_value_fn is not NULL, calls ht->destroy_value_fn with the value associated with the key.
//  - Frees the key of the entry to be removed.
//  - Frees the entry structure itself (the entry pointed to by the car of the first cons cell in bucket 0).
//  - Frees the cons cell at the head of bucket 0 (which contained the entry).
//  - Updates bucket 0 to point to the next cons cell in the list (i.e., removes the head).
//  - Returns 0.
// param:
//  - hashtable_params_template_s_1_n_1_static,
//  - hashtable_params_template_s_1_n_1_dynamic,
//  - hashtable_params_template_s_2_n_1_static,
//  - hashtable_params_template_s_2_n_1_dynamic,
//  - hashtable_params_template_s_2_n_2_no_collision_static,
//  - hashtable_params_template_s_2_n_2_no_collision_dynamic,
static void remove_destroy_first_entry_of_first_bucket_and_returns_0_when_no_collision(void **state) {
    hashtable_params_t *params = (hashtable_params_t *) *state;
    hashtable *ht = params->ht;

	// what to be cleanup
	cons *cons_to_be_cleanup = ht->buckets[0];
	entry *entry_to_be_cleanup = (entry *) (cons_to_be_cleanup->car);
	const char *key_to_be_cleanup = entry_to_be_cleanup->key;
	char *value_that_might_be_cleanup = entry_to_be_cleanup->value;

	// a key which match the key to be removed
	char *matching_key = NULL;
	alloc_and_save_address_to_be_freed((void*)&matching_key, strlen(key_to_be_cleanup) + 1);
    memcpy(matching_key, key_to_be_cleanup, strlen(key_to_be_cleanup) + 1);

	// Expect cleanup of all dynamically allocated memory associated with the entry to be removed.
	if (ht->destroy_value_fn)
        expect_value(mock_free, ptr, value_that_might_be_cleanup);
    expect_value(mock_free, ptr, key_to_be_cleanup);
    expect_value(mock_free, ptr, entry_to_be_cleanup);
    expect_value(mock_free, ptr, cons_to_be_cleanup);

	list updated_first_bucket = cons_to_be_cleanup->cdr;

	// make it search at first bucket
	set_hash_djb2(mock_hash_djb2_dummy_return_0);

    assert_int_equal(
        0,
        hashtable_remove(ht, matching_key) );

    // check update
    assert_ptr_equal((ht->buckets)[0], updated_first_bucket);
}

// Given: The hashtable is not empty, and the provided key ("key_for_B") matches the key of the second entry in bucket 0.
// Expected: (remove second entry of bucket 0)
//  - If ht->destroy_value_fn is not NULL, calls ht->destroy_value_fn with the value associated with the key.
//  - Frees the key of the entry to be removed.
//  - Frees the entry structure itself (the entry pointed to by the car of the second cons cell in bucket 0).
//  - Frees the second cons cell in bucket 0 (the cell containing the entry to be removed).
//  - Updates the bucket list so that the first cons cell's cdr points to the third cons cell (or NULL if there is no third cell), effectively skipping the removed cell.
//  - Returns 0.
// params:
//  - hashtable_params_template_s_1_n_2_collision_static
//  - hashtable_params_template_s_1_n_2_collision_dynamic
//  - hashtable_params_template_s_2_n_2_collision_static
//  - hashtable_params_template_s_2_n_2_collision_dynamic
//  - hashtable_params_template_s_2_n_3_collision_static
//  - hashtable_params_template_s_2_n_3_collision_dynamic
static void remove_destroy_second_entry_of_first_bucket_and_returns_0_when_collision_at_first_bucket(void **state) {
    hashtable_params_t *params = (hashtable_params_t *) *state;
    hashtable *ht = params->ht;

	// what to be cleanup
	cons *cons_to_be_cleanup = (ht->buckets[0])->cdr;
	entry *entry_to_be_cleanup = (entry *) (cons_to_be_cleanup->car);
	const char *key_to_be_cleanup = entry_to_be_cleanup->key;
	char *value_that_might_be_cleanup = entry_to_be_cleanup->value;

	// a key which match the key to be removed
	char *matching_key = NULL;
	alloc_and_save_address_to_be_freed((void*)&matching_key, strlen(key_to_be_cleanup) + 1);
    memcpy(matching_key, key_to_be_cleanup, strlen(key_to_be_cleanup) + 1);

	// Expect cleanup of all dynamically allocated memory associated with the entry to be removed.
	if (ht->destroy_value_fn)
        expect_value(mock_free, ptr, value_that_might_be_cleanup);
    expect_value(mock_free, ptr, key_to_be_cleanup);
    expect_value(mock_free, ptr, entry_to_be_cleanup);
    expect_value(mock_free, ptr, cons_to_be_cleanup);

	list updated_second_element_of_first_bucket = cons_to_be_cleanup->cdr;

	// make it search at first bucket
	set_hash_djb2(mock_hash_djb2_dummy_return_0);

    assert_int_equal(
        0,
        hashtable_remove(ht, matching_key) );

    // check update
    assert_ptr_equal((ht->buckets)[0]->cdr, updated_second_element_of_first_bucket);
}

// Given: The hashtable is not empty, and the provided key matches the key of the entry at the head of bucket 1.
// Expected: (removed first entry of bucket 0)
//  - If ht->destroy_value_fn is not NULL, calls ht->destroy_value_fn with the value associated with the key.
//  - Frees the key of the entry to be removed.
//  - Frees the entry structure itself (the entry pointed to by the car of the first cons cell in bucket 1).
//  - Frees the first cons cell in bucket 1 (the cell containing the entry to be removed).
//  - Updates the bucket list so that the head of bucket 1 now points to the next cons cell (cdr), or NULL if there are no more entries.
//  - Returns 0.
// param:
//  - hashtable_params_template_s_2_n_2_no_collision_static
//  - hashtable_params_template_s_2_n_2_no_collision_dynamic
//  - hashtable_params_template_s_2_n_3_collision_static
//  - hashtable_params_template_s_2_n_3_collision_dynamic
static void remove_destroy_first_entry_of_second_bucket_and_returns_0_when_no_collision_at_second_bucket(void **state) {
    hashtable_params_t *params = (hashtable_params_t *) *state;
    hashtable *ht = params->ht;

	// what to be cleanup
	cons *cons_to_be_cleanup = ht->buckets[1];
	entry *entry_to_be_cleanup = (entry *) (cons_to_be_cleanup->car);
	const char *key_to_be_cleanup = entry_to_be_cleanup->key;
	char *value_that_might_be_cleanup = entry_to_be_cleanup->value;

	// a key which match the key to be removed
	char *matching_key = NULL;
	alloc_and_save_address_to_be_freed((void*)&matching_key, strlen(key_to_be_cleanup) + 1);
    memcpy(matching_key, key_to_be_cleanup, strlen(key_to_be_cleanup) + 1);

	// Expect cleanup of all dynamically allocated memory associated with the entry to be removed.
	if (ht->destroy_value_fn)
        expect_value(mock_free, ptr, value_that_might_be_cleanup);
    expect_value(mock_free, ptr, key_to_be_cleanup);
    expect_value(mock_free, ptr, entry_to_be_cleanup);
    expect_value(mock_free, ptr, cons_to_be_cleanup);

	list updated_second_bucket = cons_to_be_cleanup->cdr;

	// make it search at second bucket
	set_hash_djb2(mock_hash_djb2_dummy_return_1);

    assert_int_equal(
        0,
        hashtable_remove(ht, matching_key) );

    // check update
    assert_ptr_equal((ht->buckets)[1], updated_second_bucket);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const hashtable_params_t *hashtable_params_ht_not_empty[12] = {
        &hashtable_params_template_s_1_n_1_static,
        &hashtable_params_template_s_1_n_1_dynamic,
        &hashtable_params_template_s_1_n_2_collision_static,
        &hashtable_params_template_s_1_n_2_collision_dynamic,
        &hashtable_params_template_s_2_n_1_static,
        &hashtable_params_template_s_2_n_1_dynamic,
        &hashtable_params_template_s_2_n_2_no_collision_static,
        &hashtable_params_template_s_2_n_2_collision_static,
        &hashtable_params_template_s_2_n_2_no_collision_dynamic,
        &hashtable_params_template_s_2_n_2_collision_dynamic,
        &hashtable_params_template_s_2_n_3_collision_static,
        &hashtable_params_template_s_2_n_3_collision_dynamic
    };

    const params_t *params_template_key_in_use[12] = {
        &params_template_s_1_n_1_static_key_in_use,
        &params_template_s_1_n_1_dynamic_key_in_use,
        &params_template_s_1_n_2_collision_static_key_in_use,
        &params_template_s_1_n_2_collision_dynamic_key_in_use,
        &params_template_s_2_n_1_static_key_in_use,
        &params_template_s_2_n_1_dynamic_key_in_use,
        &params_template_s_2_n_2_no_collision_static_key_in_use,
        &params_template_s_2_n_2_collision_static_key_in_use,
        &params_template_s_2_n_2_no_collision_dynamic_key_in_use,
        &params_template_s_2_n_2_collision_dynamic_key_in_use,
        &params_template_s_2_n_3_collision_static_key_in_use,
        &params_template_s_2_n_3_collision_dynamic_key_in_use
    };
    const params_t *params_template_key_not_in_use[16] = {
        &params_template_s_1_n_0_f_null_key_not_in_use,
        &params_template_s_1_n_0_f_free_key_not_in_use,
        &params_template_s_2_n_0_f_null_key_not_in_use,
        &params_template_s_2_n_0_f_free_key_not_in_use,
        &params_template_s_1_n_1_static_key_not_in_use,
        &params_template_s_1_n_1_dynamic_key_not_in_use,
        &params_template_s_1_n_2_collision_static_key_not_in_use,
        &params_template_s_1_n_2_collision_dynamic_key_not_in_use,
        &params_template_s_2_n_1_static_key_not_in_use,
        &params_template_s_2_n_1_dynamic_key_not_in_use,
        &params_template_s_2_n_2_no_collision_static_key_not_in_use,
        &params_template_s_2_n_2_collision_static_key_not_in_use,
        &params_template_s_2_n_2_no_collision_dynamic_key_not_in_use,
        &params_template_s_2_n_2_collision_dynamic_key_not_in_use,
        &params_template_s_2_n_3_collision_static_key_not_in_use,
        &params_template_s_2_n_3_collision_dynamic_key_not_in_use
    };

    const struct CMUnitTest create_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_s_0,
            create_setup, create_teardown, (void *)&create_s_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_s_0,
            create_setup, create_teardown, (void *)&create_s_0_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_neither_malloc_nor_free_when_s_0,
            create_setup, create_teardown, (void *)&create_s_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_neither_malloc_nor_free_when_s_0,
            create_setup, create_teardown, (void *)&create_s_0_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_null_when_malloc_for_hashtable_fail,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_frees_hashtable_when_malloc_for_buckets_fail,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            create_setup, create_teardown, (void *)&create_s_2_f_dummy_not_null),
    };

    const struct CMUnitTest destroy_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            destroy_does_not_call_free_when_s_0,
            destroy_setup, general_teardown, (void *)&hashtable_params_ht_null),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_no_entries,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_no_entries,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_0_f_free),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_no_entries,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_no_entries,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_0_f_free),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_one_entry_at_first_bucket_with_a_static_char,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_1_static),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_one_entry_at_first_bucket_with_a_static_char,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_1_static),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_one_entry_at_first_bucket_with_a_dynamic_char,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_1_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_one_entry_at_first_bucket_with_a_dynamic_char,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_1_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_s_2_two_entries_with_a_static_char_no_collision,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_s_2_two_entries_with_a_dynamic_char_no_collision,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_s_1_two_entries_hence_collision,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_2_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_s_1_two_entries_hence_collision,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_2_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_s_2_two_entries_collision_at_first_bucket,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_s_2_two_entries_collision_at_first_bucket,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_s_2_three_entries_collision_at_first_bucket,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            destroy_frees_hashtable_when_s_2_three_entries_collision_at_first_bucket,
            destroy_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_dynamic),
    };


    const struct CMUnitTest key_is_in_use_return_0_when_ht_null_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            key_is_in_use_return_0_when_ht_null,
            key_is_in_use_setup, general_teardown, (void *)&params_ht_null),
    };
    const struct CMUnitTest key_is_in_use_no_param_no_fixtures_tests[] = {
        cmocka_unit_test(
            key_is_in_use_return_0_when_ht_not_null_key_null),
    };
    struct CMUnitTest key_is_in_use_returns_1_when_key_already_in_use_tests[13] = {0};
    for (size_t i = 0; i < 12; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            key_is_in_use_returns_1_when_key_already_in_use,
            key_is_in_use_setup,
            general_teardown,
            (void *)params_template_key_in_use[i]);
        key_is_in_use_returns_1_when_key_already_in_use_tests[i] = tmp;
    }
    struct CMUnitTest key_is_in_use_returns_0_when_key_not_already_in_use_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            key_is_in_use_returns_0_when_key_not_already_in_use,
            key_is_in_use_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        key_is_in_use_returns_0_when_key_not_already_in_use_tests[i] = tmp;
    }


    const struct CMUnitTest get_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_null,
            get_setup, general_teardown, (void *)&hashtable_params_ht_null),
        cmocka_unit_test(get_returns_null_when_ht_not_null_key_null),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_0_f_free),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_1_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_1_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_2_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_2_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_0_f_free),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_1_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_1_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_null_when_ht_does_not_contain_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_1_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_1_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_2_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_2_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_1_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_1_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            get_returns_right_value_when_ht_contains_key,
            get_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_dynamic),
    };

    const struct CMUnitTest add_one_param_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            add_returns_1_when_ht_null,
            add_setup, general_teardown, (void *)&params_ht_null),
        cmocka_unit_test_prestate_setup_teardown(
            add_returns_1_when_key_null,
            add_setup, general_teardown, (void *)&params_key_null),
    };

    struct CMUnitTest add_returns_1_when_key_already_in_use_tests[13] = {0};
    for (size_t i = 0; i < 12; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            add_returns_1_when_key_already_in_use,
            add_setup,
            general_teardown,
            (void *)params_template_key_in_use[i]);
        add_returns_1_when_key_already_in_use_tests[i] = tmp;
    }
    struct CMUnitTest add_calls_malloc_for_a_new_entry_when_key_not_already_in_use_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            add_calls_malloc_for_a_new_entry_when_key_not_already_in_use,
            add_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        add_calls_malloc_for_a_new_entry_when_key_not_already_in_use_tests[i] = tmp;
    }
    struct CMUnitTest add_returns_1_when_malloc_for_a_new_entry_fails_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            add_returns_1_when_malloc_for_a_new_entry_fails,
            add_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        add_returns_1_when_malloc_for_a_new_entry_fails_tests[i] = tmp;
    }
    struct CMUnitTest add_call_strdup_with_key_arg_when_malloc_for_a_new_entry_succeeds_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            add_call_strdup_with_key_arg_when_malloc_for_a_new_entry_succeeds,
            add_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        add_call_strdup_with_key_arg_when_malloc_for_a_new_entry_succeeds_tests[i] = tmp;
    }
    struct CMUnitTest add_returns_1_when_strdup_fails_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            add_returns_1_when_strdup_fails,
            add_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        add_returns_1_when_strdup_fails_tests[i] = tmp;
    }
    struct CMUnitTest add_calls_malloc_for_a_new_cons_when_strdup_succeeds_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            add_calls_malloc_for_a_new_cons_when_strdup_succeeds,
            add_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        add_calls_malloc_for_a_new_cons_when_strdup_succeeds_tests[i] = tmp;
    }
    struct CMUnitTest add_returns_1_when_malloc_for_new_cons_fails_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            add_returns_1_when_malloc_for_new_cons_fails,
            add_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        add_returns_1_when_malloc_for_new_cons_fails_tests[i] = tmp;
    }
    struct CMUnitTest add_initializes_new_entry_in_hashtable_when_malloc_for_new_cons_succeeds_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            add_initializes_new_entry_in_hashtable_when_malloc_for_new_cons_succeeds,
            add_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        add_initializes_new_entry_in_hashtable_when_malloc_for_new_cons_succeeds_tests[i] = tmp;
    }
    struct CMUnitTest add_return_0_when_malloc_for_new_cons_succeeds_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            add_return_0_when_malloc_for_new_cons_succeeds,
            add_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        add_return_0_when_malloc_for_new_cons_succeeds_tests[i] = tmp;
    }

    const struct CMUnitTest reset_value_one_param_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            reset_value_returns_1_when_ht_null,
            reset_value_setup, general_teardown, (void *)&params_ht_null),
    };
    const struct CMUnitTest reset_value_no_param_no_fixtures_tests[] = {
        cmocka_unit_test(reset_value_return_null_when_ht_not_null_key_null),
    };
    struct CMUnitTest reset_value_returns_1_when_key_not_in_use_tests[17] = {0};
    for (size_t i = 0; i < 16; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            reset_value_returns_1_when_key_not_in_use,
            reset_value_setup,
            general_teardown,
            (void *)params_template_key_not_in_use[i]);
        reset_value_returns_1_when_key_not_in_use_tests[i] = tmp;
    }

    struct CMUnitTest reset_value_free_reset_value_and_returns_0_when_key_in_use_tests[13] = {0};
    for (size_t i = 0; i < 12; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            reset_value_free_reset_value_and_returns_0_when_key_in_use,
            reset_value_setup,
            general_teardown,
            (void *)params_template_key_in_use[i]);
        reset_value_free_reset_value_and_returns_0_when_key_in_use_tests[i] = tmp;
    }

    const struct CMUnitTest remove_returns_1_when_ht_null_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            remove_returns_1_when_ht_null,
            remove_setup, general_teardown, (void *)&hashtable_params_ht_null),
    };

    const struct CMUnitTest remove_no_param_no_fixtures_tests[] = {
        cmocka_unit_test(
            remove_returns_1_when_when_ht_not_null_key_null),
    };

    struct CMUnitTest remove_returns_1_when_key_not_in_use_tests[13] = {0};
    for (size_t i = 0; i < 12; i++) {
        struct CMUnitTest tmp = cmocka_unit_test_prestate_setup_teardown(
            remove_returns_1_when_key_not_in_use,
            remove_setup,
            general_teardown,
            (void *)hashtable_params_ht_not_empty[i]);
        remove_returns_1_when_key_not_in_use_tests[i] = tmp;
    }

    const struct CMUnitTest remove_destroy_first_entry_of_first_bucket_and_returns_0_when_no_collision_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_first_bucket_and_returns_0_when_no_collision,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_1_static),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_first_bucket_and_returns_0_when_no_collision,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_1_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_first_bucket_and_returns_0_when_no_collision,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_1_static),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_first_bucket_and_returns_0_when_no_collision,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_1_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_first_bucket_and_returns_0_when_no_collision,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_first_bucket_and_returns_0_when_no_collision,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_dynamic),
    };

    const struct CMUnitTest remove_destroy_second_entry_of_first_bucket_and_returns_0_when_collision_at_first_bucket_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_second_entry_of_first_bucket_and_returns_0_when_collision_at_first_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_2_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_second_entry_of_first_bucket_and_returns_0_when_collision_at_first_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_1_n_2_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_second_entry_of_first_bucket_and_returns_0_when_collision_at_first_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_second_entry_of_first_bucket_and_returns_0_when_collision_at_first_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_second_entry_of_first_bucket_and_returns_0_when_collision_at_first_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_second_entry_of_first_bucket_and_returns_0_when_collision_at_first_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_dynamic),
    };

    const struct CMUnitTest remove_destroy_first_entry_of_second_bucket_and_returns_0_when_no_collision_at_second_bucket_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_second_bucket_and_returns_0_when_no_collision_at_second_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_second_bucket_and_returns_0_when_no_collision_at_second_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_2_no_collision_dynamic),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_second_bucket_and_returns_0_when_no_collision_at_second_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_static),
        cmocka_unit_test_prestate_setup_teardown(
            remove_destroy_first_entry_of_second_bucket_and_returns_0_when_no_collision_at_second_bucket,
            remove_setup, general_teardown, (void *)&hashtable_params_template_s_2_n_3_collision_dynamic),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(create_tests, NULL, NULL);
    failed += cmocka_run_group_tests(destroy_tests, NULL, NULL);

    failed += cmocka_run_group_tests(key_is_in_use_return_0_when_ht_null_tests, NULL, NULL);
    failed += cmocka_run_group_tests(key_is_in_use_no_param_no_fixtures_tests, NULL, NULL);
    failed += cmocka_run_group_tests(key_is_in_use_returns_1_when_key_already_in_use_tests, NULL, NULL);
    failed += cmocka_run_group_tests(key_is_in_use_returns_0_when_key_not_already_in_use_tests, NULL, NULL);

    failed += cmocka_run_group_tests(get_tests, NULL, NULL);

    failed += cmocka_run_group_tests(add_one_param_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_returns_1_when_key_already_in_use_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_calls_malloc_for_a_new_entry_when_key_not_already_in_use_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_returns_1_when_malloc_for_a_new_entry_fails_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_call_strdup_with_key_arg_when_malloc_for_a_new_entry_succeeds_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_returns_1_when_strdup_fails_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_calls_malloc_for_a_new_cons_when_strdup_succeeds_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_returns_1_when_malloc_for_new_cons_fails_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_initializes_new_entry_in_hashtable_when_malloc_for_new_cons_succeeds_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_return_0_when_malloc_for_new_cons_succeeds_tests, NULL, NULL);

    failed += cmocka_run_group_tests(reset_value_one_param_tests, NULL, NULL);
    failed += cmocka_run_group_tests(reset_value_no_param_no_fixtures_tests, NULL, NULL);
    failed += cmocka_run_group_tests(reset_value_returns_1_when_key_not_in_use_tests, NULL, NULL);
    failed += cmocka_run_group_tests(reset_value_free_reset_value_and_returns_0_when_key_in_use_tests, NULL, NULL);

    failed += cmocka_run_group_tests(remove_returns_1_when_ht_null_tests, NULL, NULL);
    failed += cmocka_run_group_tests(remove_no_param_no_fixtures_tests, NULL, NULL);
    failed += cmocka_run_group_tests(remove_returns_1_when_key_not_in_use_tests, NULL, NULL);
    failed += cmocka_run_group_tests(remove_destroy_first_entry_of_first_bucket_and_returns_0_when_no_collision_tests, NULL, NULL);
    failed += cmocka_run_group_tests(remove_destroy_second_entry_of_first_bucket_and_returns_0_when_collision_at_first_bucket_tests, NULL, NULL);
    failed += cmocka_run_group_tests(remove_destroy_first_entry_of_second_bucket_and_returns_0_when_no_collision_at_second_bucket_tests, NULL, NULL);

    return failed;
}
