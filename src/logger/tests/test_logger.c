// logger/tests/test_logger.c

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <time.h>
#include "logger.h"



//-----------------------------------------------------------------------------
// init_logger TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// CONSTANTS
//-----------------------------------------------------------------------------

enum {SUCCESS = 0, FAILURE = -1};
#define APPEND_MODE "a"



//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES
//-----------------------------------------------------------------------------

static FILE fake_file;

FILE *fopen(const char *path, const char *mode) {
    check_expected_ptr(path);
    check_expected_ptr(mode);
    return mock_type(FILE *);
}



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------

typedef struct {
    const char *label;
    const char *log_path;
    FILE *log_file_init;
    int expected_result; // expected init_logger returned value
    FILE *expected_log_file_after_init_logger_call; // expected log_file value after init_logger call
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

static void expect_init_logger_result_and_effect_on_logfile(logger_test_params_t *params) {
    FILE *log_file_before_init_logger_call = get_log_file();
    int result = init_logger(params->log_path);
    assert_int_equal(result, params->expected_result);
    assert_ptr_equal(get_log_file(), params->expected_log_file_after_init_logger_call);
}



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------

static logger_test_params_t logPathNull_logFileNull = {
    .label = "log_path == NULL, log_file == NULL",
    .log_path = NULL,
    .log_file_init = NULL,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = NULL
};
static logger_test_params_t logPathNull_logFileNotNull = {
    .label = "log_path == NULL, log_file != NULL",
    .log_path = NULL,
    .log_file_init = &fake_file,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = &fake_file
};
static logger_test_params_t logPathNotNull_logFileNotNull = {
    .label = "log_path != NULL, log_file != NULL",
    .log_path = "example/path",
    .log_file_init = &fake_file,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = &fake_file
};
static logger_test_params_t logPathNotNull_logFileNull_FopenFail = {
    .label = "log_path != NULL, log_file == NULL, fopen fail",
    .log_path = "example/path",
    .log_file_init = NULL,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = NULL
};
static logger_test_params_t logPathNotNull_logFileNull_FopenSuccess = {
    .label = "log_path != NULL, log_file == NULL, fopen success",
    .log_path = "example/path",
    .log_file_init = NULL,
    .expected_result = SUCCESS,
    .expected_log_file_after_init_logger_call = &fake_file
};



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------

// Given: log_path == NULL and log_file == NULL
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNull_logFileNull
static void init_logger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNull(void **state) {
    expect_init_logger_result_and_effect_on_logfile((logger_test_params_t *)(*state));
}

// Given: log_path == NULL and log_file != NULL
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNull_logFileNotNull
static void initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNotNull(void **state) {
    expect_init_logger_result_and_effect_on_logfile((logger_test_params_t *)(*state));
}

// Given: log_file is not NULL and log_file is not NULL
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNotNull_logFileNotNull
static void initLogger_ErrorAndNoLogfileChange_WhenLoggerIsAlreadyInitialized(void **state) {
    expect_init_logger_result_and_effect_on_logfile((logger_test_params_t *)(*state));
}

// Given: log_file is not NULL and log_file is not NULL
// Expected : init_logger do not call fopen
// Param : logPathNotNull_logFileNotNull
static void initLogger_DoNotCallFopen_WhenLogPathIsNotNull_AndLogfileIsNotNull(void **state) {
    expect_init_logger_result_and_effect_on_logfile((logger_test_params_t *)(*state));
}

// Given: log_file is not NULL and log_file is NULL
// Expected : init_logger call fopen with right arguments (log_path and APPEND_MODE)
// Param : logPathNotNull_logFileNull_FopenFail
static void initLogger_CallFopenWithRightParams_WhenLogPathIsNotNull_AndLogfileIsNull(void **state) {
    logger_test_params_t *params = (logger_test_params_t *)(*state);
    expect_string(fopen, path, params->log_path);
    expect_string(fopen, mode, APPEND_MODE);
    will_return(fopen, NULL);
    init_logger(params->log_path);
}

// Given: log_file is not NULL, log_file is NULL, fopen call fail
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNotNull_logFileNull_FopenFail
static void initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenFail(void **state) {
    logger_test_params_t *params = (logger_test_params_t *)(*state);
    expect_string(fopen, path, params->log_path);
    expect_string(fopen, mode, APPEND_MODE);
    will_return(fopen, NULL);
    int result = init_logger(params->log_path);
    assert_int_equal(result, FAILURE);
    assert_ptr_equal(get_log_file(), NULL);
}

// Given: log_file is not NULL, log_file is NULL, fopen call success
// Expected : init_logger returns 0 and log_file value is the one returned by fopen
// Param : logPathNotNull_logFileNull_FopenSuccess
static void initLogger_SucessAndLogfileInitialized_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenSuccess(void **state) {
    logger_test_params_t *params = (logger_test_params_t *)(*state);
    expect_string(fopen, path, params->log_path);
    expect_string(fopen, mode, APPEND_MODE);
    will_return(fopen, &fake_file);
    int result = init_logger(params->log_path);
    assert_int_equal(result, SUCCESS);
    assert_ptr_equal(get_log_file(), &fake_file);
}



//-----------------------------------------------------------------------------
// log_message TESTS
//-----------------------------------------------------------------------------

// TO DO



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest init_logger_tests[] = {
		cmocka_unit_test_prestate_setup_teardown(
            init_logger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNull,
            setup, teardown, &logPathNull_logFileNull),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNotNull,
            setup, teardown, &logPathNull_logFileNotNull),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_ErrorAndNoLogfileChange_WhenLoggerIsAlreadyInitialized,
            setup, teardown, &logPathNotNull_logFileNotNull),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_DoNotCallFopen_WhenLogPathIsNotNull_AndLogfileIsNotNull,
            setup, teardown, &logPathNotNull_logFileNotNull),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_CallFopenWithRightParams_WhenLogPathIsNotNull_AndLogfileIsNull,
            setup, teardown, &logPathNotNull_logFileNull_FopenFail),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenFail,
            setup, teardown, &logPathNotNull_logFileNull_FopenFail),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_SucessAndLogfileInitialized_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenSuccess,
            setup, teardown, &logPathNotNull_logFileNull_FopenSuccess),
    };
    return cmocka_run_group_tests(init_logger_tests, NULL, NULL);
}
