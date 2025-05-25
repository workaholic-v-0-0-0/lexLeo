// logger/tests/test_logger.c

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <time.h>
#include "logger.h"



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

int fclose(FILE *file) {
    check_expected_ptr(file);
    return mock_type(int);
}


//-----------------------------------------------------------------------------
// init_logger TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------

typedef struct {
    const char *label;
    const char *log_path;
    FILE *log_file_init;
    int expected_result; // expected init_logger returned value
    FILE *expected_log_file_after_init_logger_call; // expected log_file value after init_logger call
} init_logger_test_params_t;



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int init_logger_setup(void **state) {
    init_logger_test_params_t *params = (init_logger_test_params_t *)(*state);
    set_log_file(params->log_file_init);
    return 0;
}

static int init_logger_teardown(void **state) {
    set_log_file(NULL);
    return 0;
}



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------

static void expect_init_logger_result_and_effect_on_logfile(init_logger_test_params_t *params) {
    FILE *log_file_before_init_logger_call = get_log_file();
    int result = init_logger(params->log_path);
    assert_int_equal(result, params->expected_result);
    assert_ptr_equal(get_log_file(), params->expected_log_file_after_init_logger_call);
}



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------

static init_logger_test_params_t logPathNull_logFileNull = {
    .label = "log_path == NULL, log_file == NULL",
    .log_path = NULL,
    .log_file_init = NULL,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = NULL
};
static init_logger_test_params_t logPathNull_logFileNotNull = {
    .label = "log_path == NULL, log_file != NULL",
    .log_path = NULL,
    .log_file_init = &fake_file,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = &fake_file
};
static init_logger_test_params_t logPathNotNull_logFileNotNull = {
    .label = "log_path != NULL, log_file != NULL",
    .log_path = "example/path",
    .log_file_init = &fake_file,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = &fake_file
};
static init_logger_test_params_t logPathNotNull_logFileNull_FopenFail = {
    .label = "log_path != NULL, log_file == NULL, fopen fail",
    .log_path = "example/path",
    .log_file_init = NULL,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = NULL
};
static init_logger_test_params_t logPathNotNull_logFileNull_FopenSuccess = {
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
    expect_init_logger_result_and_effect_on_logfile((init_logger_test_params_t *)(*state));
}

// Given: log_path == NULL and log_file != NULL
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNull_logFileNotNull
static void initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNotNull(void **state) {
    expect_init_logger_result_and_effect_on_logfile((init_logger_test_params_t *)(*state));
}

// Given: log_file is not NULL and log_file is not NULL
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNotNull_logFileNotNull
static void initLogger_ErrorAndNoLogfileChange_WhenLoggerIsAlreadyInitialized(void **state) {
    expect_init_logger_result_and_effect_on_logfile((init_logger_test_params_t *)(*state));
}

// Given: log_file is not NULL and log_file is not NULL
// Expected : init_logger do not call fopen
// Param : logPathNotNull_logFileNotNull
static void initLogger_DoNotCallFopen_WhenLogPathIsNotNull_AndLogfileIsNotNull(void **state) {
    expect_init_logger_result_and_effect_on_logfile((init_logger_test_params_t *)(*state));
}

// Given: log_file is not NULL and log_file is NULL
// Expected : init_logger call fopen with right arguments (log_path and APPEND_MODE)
// Param : logPathNotNull_logFileNull_FopenFail
static void initLogger_CallFopenWithRightParams_WhenLogPathIsNotNull_AndLogfileIsNull(void **state) {
    init_logger_test_params_t *params = (init_logger_test_params_t *)(*state);
    expect_string(fopen, path, params->log_path);
    expect_string(fopen, mode, APPEND_MODE);
    will_return(fopen, NULL);
    init_logger(params->log_path);
}

// Given: log_file is not NULL, log_file is NULL, fopen call fail
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNotNull_logFileNull_FopenFail
static void initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenFail(void **state) {
    init_logger_test_params_t *params = (init_logger_test_params_t *)(*state);
    expect_string(fopen, path, params->log_path);
    expect_string(fopen, mode, APPEND_MODE);
    will_return(fopen, NULL);
    expect_init_logger_result_and_effect_on_logfile(params);
}

// Given: log_file is not NULL, log_file is NULL, fopen call success
// Expected : init_logger returns 0 and log_file value is the one returned by fopen
// Param : logPathNotNull_logFileNull_FopenSuccess
static void initLogger_SucessAndLogfileInitialized_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenSuccess(void **state) {
    init_logger_test_params_t *params = (init_logger_test_params_t *)(*state);
    expect_string(fopen, path, params->log_path);
    expect_string(fopen, mode, APPEND_MODE);
    will_return(fopen, &fake_file);
    expect_init_logger_result_and_effect_on_logfile(params);
}



//-----------------------------------------------------------------------------
// close_logger TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------

typedef struct {
    const char *label;
    FILE *log_file_init;
    int expected_result; // expected close_logger returned value
    FILE *expected_log_file_after_close_logger_call; // expected log_file value after close_logger call
} close_logger_test_params_t;



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int close_logger_setup(void **state) {
    close_logger_test_params_t *params = (close_logger_test_params_t *)(*state);
    set_log_file(params->log_file_init);
    return 0;
}

static int close_logger_teardown(void **state) {
    set_log_file(NULL);
    return 0;
}



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------

static void expect_close_logger_result_and_effect_on_logfile(close_logger_test_params_t *params) {
    FILE *log_file_before_init_logger_call = get_log_file();
    int result = close_logger();
    assert_int_equal(result, params->expected_result);
    assert_ptr_equal(get_log_file(), params->expected_log_file_after_close_logger_call);
}



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------

static close_logger_test_params_t logFileNull = {
    .label = "log_file == NULL",
    .log_file_init = NULL,
    .expected_result = FAILURE,
    .expected_log_file_after_close_logger_call = NULL
};

static close_logger_test_params_t logFileNotNull_FcloseFail = {
    .label = "log_file != NULL, fclose fail",
    .log_file_init = &fake_file,
    .expected_result = FAILURE,
    .expected_log_file_after_close_logger_call = &fake_file
};

static close_logger_test_params_t logFileNotNull_FcloseSuccess = {
    .label = "log_file != NULL, fclose success",
    .log_file_init = &fake_file,
    .expected_result = SUCCESS,
    .expected_log_file_after_close_logger_call = NULL
};



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------

// Given: log_file == NULL
// Expected: close_logger returns -1 and log_file remains NULL
// Param : logFileNull
static void close_logger_ErrorAndNoLogfileChange_WhenLogfileIsNull(void **state) {
    expect_close_logger_result_and_effect_on_logfile((close_logger_test_params_t *)(*state));
}

// Given: log_file != NULL
// Expected: close_logger calls fclose with parameter log_file
// Param : logFileNotNull_FcloseFail
static void close_logger_CallsFcloseWithRightParam_WhenLogfileIsNotNull(void **state) {
    close_logger_test_params_t *params = (close_logger_test_params_t *)(*state);
    expect_value(fclose, file, params->log_file_init);
    will_return(fclose, EOF);
    close_logger();
}

// Given: log_file != NULL and fclose returns EOF
// Expected: close_logger return -1 and log_file still points to the open file
// Param : logFileNotNull_FcloseFail
static void close_logger_ErrorAndNoLogfileChange_WhenLogfileIsNotNull_AndFcloseReturnsEOF(void **state) {
    close_logger_test_params_t *params = (close_logger_test_params_t *)(*state);
    expect_value(fclose, file, params->log_file_init);
    will_return(fclose, EOF);
    expect_close_logger_result_and_effect_on_logfile(params);
}

// Given: log_file != NULL and fclose returns 0
// Expected: close_logger returns 0 and log_file is NULL
// Param : logFileNotNull_FcloseSuccess
static void close_logger_SucessAndLogfileIsNull_WhenLogfileIsNotNull_AndFcloseReturns0(void **state) {
    close_logger_test_params_t *params = (close_logger_test_params_t *)(*state);
    expect_value(fclose, file, params->log_file_init);
    will_return(fclose, 0);
    expect_close_logger_result_and_effect_on_logfile(params);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest init_logger_tests[] = {
		cmocka_unit_test_prestate_setup_teardown(
            init_logger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNull,
            init_logger_setup, init_logger_teardown, &logPathNull_logFileNull),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNotNull,
            init_logger_setup, init_logger_teardown, &logPathNull_logFileNotNull),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_ErrorAndNoLogfileChange_WhenLoggerIsAlreadyInitialized,
            init_logger_setup, init_logger_teardown, &logPathNotNull_logFileNotNull),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_DoNotCallFopen_WhenLogPathIsNotNull_AndLogfileIsNotNull,
            init_logger_setup, init_logger_teardown, &logPathNotNull_logFileNotNull),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_CallFopenWithRightParams_WhenLogPathIsNotNull_AndLogfileIsNull,
            init_logger_setup, init_logger_teardown, &logPathNotNull_logFileNull_FopenFail),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenFail,
            init_logger_setup, init_logger_teardown, &logPathNotNull_logFileNull_FopenFail),
        cmocka_unit_test_prestate_setup_teardown(
            initLogger_SucessAndLogfileInitialized_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenSuccess,
            init_logger_setup, init_logger_teardown, &logPathNotNull_logFileNull_FopenSuccess),
    };

    const struct CMUnitTest close_logger_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            close_logger_ErrorAndNoLogfileChange_WhenLogfileIsNull,
            close_logger_setup, close_logger_teardown, &logFileNull),
        cmocka_unit_test_prestate_setup_teardown(
            close_logger_CallsFcloseWithRightParam_WhenLogfileIsNotNull,
            close_logger_setup, close_logger_teardown, &logFileNotNull_FcloseFail),
        cmocka_unit_test_prestate_setup_teardown(
            close_logger_ErrorAndNoLogfileChange_WhenLogfileIsNotNull_AndFcloseReturnsEOF,
            close_logger_setup, close_logger_teardown, &logFileNotNull_FcloseFail),
        cmocka_unit_test_prestate_setup_teardown(
            close_logger_SucessAndLogfileIsNull_WhenLogfileIsNotNull_AndFcloseReturns0,
            close_logger_setup, close_logger_teardown, &logFileNotNull_FcloseSuccess),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(init_logger_tests, NULL, NULL);
    failed += cmocka_run_group_tests(close_logger_tests, NULL, NULL);
    return failed;
}
