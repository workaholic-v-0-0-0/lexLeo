// logger/tests/test_init_logger.c

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "logger.h"

// Test: init_logger returns -1 if the log_path parameter is NULL.
// Precondition: log_path == NULL
// Expected postcondition: the function returns -1 and the logger remains uninitialized.
static void initLogger_ShouldReturnError_WhenLogPathIsNull(void **state) {
    (void)state; // Unused
    int result = init_logger(NULL);
    assert_int_equal(result, -1);
    // Optionnel : tester que log_file est toujours NULL, ou qu'un flag d'init est resté à 0.
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(initLogger_ShouldReturnError_WhenLogPathIsNull),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
