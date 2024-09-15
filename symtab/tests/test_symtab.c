#include "symtab/symtab.h"

#include "data_structures/hashtable.h"

#include "logger/logger.h"

#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void setup(void) {

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        SYMBOL_TABLE[i] = NULL;
    }
}

void teardown(void) {
    cleanup_hashtable(SYMBOL_TABLE);  // Cleanup symbol table after each test
}

START_TEST(test_declare_symbol) {
    char *key = "test_symbol";
    typed_data *value = malloc(sizeof(typed_data));
    value->type = TYPE_INT;
    value->data.int_value = 42;

    declare(key, value);

    ck_assert(is_declared(key) == 1);
}
END_TEST

START_TEST(test_redeclare_symbol) {
    char *key = "test_symbol";
    typed_data *value = malloc(sizeof(typed_data));
    value->type = TYPE_INT;
    value->data.int_value = 42;

    declare(key, value);
    // Fork a new process to test the exit behavior
    pid_t pid = fork();
    if (pid == 0) {
        declare(key, value);
        // If we reach here, the test has failed because declare() should have
        // exited
        exit(EXIT_SUCCESS);
    } else {
        int status;
        waitpid(pid, &status, 0);

        // Check if the child process exited with the expected error code
        ck_assert_msg(WIFEXITED(status), "Child process did not exit normally");
        ck_assert_int_eq(WEXITSTATUS(status), EXIT_FAILURE);
    }
}
END_TEST

START_TEST(test_eval_symbol) {
    char *key = "test_symbol";
    typed_data *value = malloc(sizeof(typed_data));
    value->type = TYPE_INT;
    value->data.int_value = 42;

    symbol sym = declare(key, value);
    typed_data result = eval(sym);

    ck_assert_int_eq(result.type, TYPE_INT);
    ck_assert_int_eq(result.data.int_value, 42);
}
END_TEST

// Test for checking if a non-declared symbol exists
START_TEST(test_is_declared_non_existent) {
    char *key = "non_existent_symbol";

    // Check if the symbol is declared (it should not be)
    ck_assert(is_declared(key) == 0);
}
END_TEST

Suite* symtab_suite(void) {
    Suite *s = suite_create("Symtab");
    TCase *tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_declare_symbol);
    tcase_add_test(tc_core, test_redeclare_symbol);
    tcase_add_test(tc_core, test_eval_symbol);
    tcase_add_test(tc_core, test_is_declared_non_existent);
    suite_add_tcase(s, tc_core);
    return s;
}

// Main function to run the tests
int main(void) {
    int number_failed;
    Suite *s = symtab_suite();
    SRunner *sr = srunner_create(s);

    // Run all tests
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
