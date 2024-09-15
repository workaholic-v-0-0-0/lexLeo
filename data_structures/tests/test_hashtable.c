#include "data_structures/hashtable.h"
#include <check.h>
#include <stdlib.h>
#include <string.h>

static hashtable test_hashtable;

void setup(void) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        test_hashtable[i] = NULL;  // Ensure each list starts as NULL
    }
}

void teardown(void) {
    cleanup_hashtable(test_hashtable);
    cleanup_hashtable(test_hashtable);
}

START_TEST(test_hash_function) {
    char *key = "testkey";
    unsigned long int result = hash(key);
    ck_assert_uint_ne(result, 0);
}
END_TEST

START_TEST(test_create_entry) {
    char *key = "testkey";
    char *value = strdup("value");
    hash_location loc = create_entry(test_hashtable, key, value);
    ck_assert_str_eq(((hash_entry *)test_hashtable[loc.index]->car)->key, key);
    ck_assert_str_eq(((hash_entry *)test_hashtable[loc.index]->car)->value, value);
}
END_TEST

START_TEST(test_get_value) {
    char *key = "testkey";
    char *value = strdup("value");
    create_entry(test_hashtable, key, value);
    void *retrieved_value = get_value(test_hashtable, (hash_location){key, get_hashtable_index(key)});
    ck_assert_ptr_eq(retrieved_value, value);
}
END_TEST

Suite* hash_table_suite(void) {
    Suite *s = suite_create("Hash Table");
    TCase *tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_hash_function);
    tcase_add_test(tc_core, test_create_entry);
    tcase_add_test(tc_core, test_get_value);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {
    int number_failed;
    Suite *s = hash_table_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
