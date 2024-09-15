#include "data_structures/list.h"
#include <check.h>
#include <stdlib.h>
#include <logger/logger.h>

START_TEST(test_push_list_single_element) {
    list l = NULL;
    int *value = malloc(sizeof(int));
    *value = 42;

    push_list(&l, value);  // Add an element to the list

    // Check that the element was successfully added
    ck_assert_ptr_nonnull(l);
    ck_assert_int_eq(*(int *)l->car, 42);
    ck_assert_ptr_null(l->cdr);

    cleanup_list(&l);  // Clean up the list after the test
}
END_TEST

START_TEST(test_push_list_multiple_elements) {
    list l = NULL;
    int *value = NULL;

    // Add multiple elements to the list
    for (int i = 0; i < 10; i++) {
        value = malloc(sizeof(int));
        *value = i;
        push_list(&l, value);
    }

    // Check that the elements were added correctly in reverse order
    list temp = l;
    for (int i = 9; i >= 0; i--) {
        ck_assert_ptr_nonnull(temp);
        ck_assert_int_eq(*(int *)temp->car, i);
        temp = temp->cdr;
    }

    // Clean up the list after the test
    cleanup_list(&l);
    ck_assert_ptr_null(l);  // Ensure the list is cleaned up and set to NULL
}
END_TEST

START_TEST(test_to_list) {
    int *element_1 = malloc(sizeof(int));
    *element_1 = 1;
    int *element_2 = malloc(sizeof(int));
    *element_2 = 2;
    int *element_3 = malloc(sizeof(int));
    *element_3 = 3;

    list l = to_list(3, element_1, element_2, element_3);

    ck_assert_int_eq(* (int *) l->car, 3);
    ck_assert_int_eq(* (int *) l->cdr->car, 2);
    ck_assert_int_eq(* (int *) l->cdr->cdr->car, 1);
    ck_assert_ptr_null(l->cdr->cdr->cdr);
    cleanup_list(&l);
}
END_TEST

START_TEST(test_reverse) {
    int *element_1 = malloc(sizeof(int));
    *element_1 = 1;
    int *element_2 = malloc(sizeof(int));
    *element_2 = 2;
    int *element_3 = malloc(sizeof(int));
    *element_3 = 3;

    list l = to_list(3, element_1, element_2, element_3);
    reverse(&l);

    ck_assert_int_eq(* (int *) l->car, 1);
    ck_assert_int_eq(* (int *) l->cdr->car, 2);
    ck_assert_int_eq(* (int *) l->cdr->cdr->car, 3);
    ck_assert_ptr_null(l->cdr->cdr->cdr);

    cleanup_list(&l);
}
END_TEST

START_TEST(test_cleanup_empty_list) {
    list l = NULL;
    cleanup_list(&l);  // Clean an empty list
    ck_assert_ptr_null(l);  // Ensure the list remains NULL
}
END_TEST

START_TEST(test_cleanup_non_empty_list) {
    list l = NULL;
    int *value1 = malloc(sizeof(int));
    int *value2 = malloc(sizeof(int));

    *value1 = 42;
    *value2 = 99;

    // Add two elements to the list
    push_list(&l, value1);
    push_list(&l, value2);

    // Clean up the list
    cleanup_list(&l);

    // Ensure the list has been properly cleaned up and set to NULL
    ck_assert_ptr_null(l);
}
END_TEST

Suite* list_suite(void) {
    Suite *s = suite_create("List");
    TCase *tc_core = tcase_create("Core");

    // Add tests to the test case
    tcase_add_test(tc_core, test_push_list_single_element);
    tcase_add_test(tc_core, test_push_list_multiple_elements);
    tcase_add_test(tc_core, test_to_list);
    tcase_add_test(tc_core, test_reverse);
    tcase_add_test(tc_core, test_cleanup_empty_list);
    tcase_add_test(tc_core, test_cleanup_non_empty_list);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void) {
    int number_failed;
    Suite *s = list_suite();
    SRunner *sr = srunner_create(s);

    // Run all tests
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
