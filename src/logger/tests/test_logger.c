// logger/tests/test_logger.c

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "logger.h"


//-----------------------------------------------------------------------------
// MOCKS, STUB, FAKES
//-----------------------------------------------------------------------------

FILE *fopen(const char *path, const char *mode) {
    check_expected_ptr(path);
    check_expected_ptr(mode);
    return mock_type(FILE *);
}

static FILE fake_not_null_log_file;


//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------

typedef struct {
    const char *label;
    const char *log_path;
    FILE *log_file_init;
    int expected_result;       // expected return value
} logger_test_params_t;


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup(void **state) {
    logger_test_params_t *params = (logger_test_params_t *)(*state);
    set_log_file(params->log_file_init);
    return 0;
}

static int teardown(void **state) {
    set_log_file(NULL);
    return 0;
}


//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------

/**
 * check error and invariant on log_file.
 * Checks that init_logger returns -1 and log_file is unchanged.
 */
static void expect_init_logger_result_and_no_logfile_change(logger_test_params_t *params) {
    FILE *log_file_before = get_log_file();
    int result = init_logger(params->log_path);
    FILE *log_file_after = get_log_file();
    assert_int_equal(result, params->expected_result);
    assert_ptr_equal(log_file_before, log_file_after);
}


//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------

static logger_test_params_t case_null_null = {
    .label = "log_path == NULL, log_file == NULL",
    .log_path = NULL,
    .log_file_init = NULL,
    .expected_result = -1
};
static logger_test_params_t case_null_notnull = {
    .label = "log_path == NULL, log_file != NULL",
    .log_path = NULL,
    .log_file_init = &fake_not_null_log_file,
    .expected_result = -1
};
static logger_test_params_t case_valid_notnull = {
    .label = "log_path valid, log_file != NULL",
    .log_path = "example/path",
    .log_file_init = &fake_not_null_log_file,
    .expected_result = -1
};


//-----------------------------------------------------------------------------
// TESTS : init_logger
//-----------------------------------------------------------------------------

// Given: log_path == NULL and log_file == NULL
// Expected : the function returns -1 and log_file is not changed.
static void init_logger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNull(void **state) {
    expect_init_logger_result_and_no_logfile_change((logger_test_params_t *)(*state));
}

// Given: log_path == NULL and log_file != NULL
// Expected : the function returns -1 and log_file is not changed.
static void initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNotNull(void **state) {
    expect_init_logger_result_and_no_logfile_change((logger_test_params_t *)(*state));
}

// Given: log_file is not NULL
// Expected : the function returns -1 and log_file is not changed.
static void initLogger_ErrorAndNoLogfileChange_WhenLoggerIsAlreadyInitialized(void **state) {
    expect_init_logger_result_and_no_logfile_change((logger_test_params_t *)(*state));
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest tests[] = {
		cmocka_unit_test_prestate_setup_teardown(
            init_logger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNull,
            setup, teardown, &case_null_null),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNotNull,
            setup, teardown, &case_null_notnull),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_ErrorAndNoLogfileChange_WhenLoggerIsAlreadyInitialized,
            setup, teardown, &case_valid_notnull),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
