// logger/tests/test_logger.c

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "fake_time.h"
#include "logger.h"


//-----------------------------------------------------------------------------
// CONSTANTS
//-----------------------------------------------------------------------------

enum {
    SUCCESS = 0,
    FAILURE = -1,
    FIRST_EPOCH_TIME = 0,
    EPOCH_TIME_2025_06_10_MIDDAY = 1749549600,
    EPOCH_TIME_2025_05_07_MIDDAY = 1746612000,
};

#define APPEND_MODE "a"


//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES
//-----------------------------------------------------------------------------

static FILE dummy_log_file;

FILE *fopen(const char *path, const char *mode) {
    check_expected_ptr(path);
    check_expected_ptr(mode);
    return mock_type(FILE *);
}

int mock_fclose(FILE *file) {
    check_expected_ptr(file);
    return mock_type(int);
}

#define LOGGER_LINE_MAX_LENGTH 1024
#define LOGGER_FILE_MAX_LENGTH (10 * LOGGER_LINE_MAX_LENGTH)
static char fake_log_file_address[LOGGER_FILE_MAX_LENGTH];
FILE *fake_log_file; // defined in setup via fmemopen(fake_log_file_address, LOGGER_FILE_MAX_LENGTH, APPEND_MODE);

#define TIME_STR_LENGTH 20

// time is faked via fake_time.h


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
    init_logger_test_params_t *params = (init_logger_test_params_t *) (*state);
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
    .log_file_init = &dummy_log_file,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = &dummy_log_file
};
static init_logger_test_params_t logPathNotNull_logFileNotNull = {
    .label = "log_path != NULL, log_file != NULL",
    .log_path = "example/path",
    .log_file_init = &dummy_log_file,
    .expected_result = FAILURE,
    .expected_log_file_after_init_logger_call = &dummy_log_file
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
    .expected_log_file_after_init_logger_call = &dummy_log_file
};


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------

// Given: log_path == NULL and log_file == NULL
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNull_logFileNull
static void init_logger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNull(void **state) {
    expect_init_logger_result_and_effect_on_logfile((init_logger_test_params_t *) (*state));
}

// Given: log_path == NULL and log_file != NULL
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNull_logFileNotNull
static void initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNull_AndLogfileIsNotNull(void **state) {
    expect_init_logger_result_and_effect_on_logfile((init_logger_test_params_t *) (*state));
}

// Given: log_file is not NULL and log_file is not NULL
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNotNull_logFileNotNull
static void initLogger_ErrorAndNoLogfileChange_WhenLoggerIsAlreadyInitialized(void **state) {
    expect_init_logger_result_and_effect_on_logfile((init_logger_test_params_t *) (*state));
}

// Given: log_file is not NULL and log_file is not NULL
// Expected : init_logger do not call fopen
// Param : logPathNotNull_logFileNotNull
static void initLogger_DoNotCallFopen_WhenLogPathIsNotNull_AndLogfileIsNotNull(void **state) {
    expect_init_logger_result_and_effect_on_logfile((init_logger_test_params_t *) (*state));
}

// Given: log_file is not NULL and log_file is NULL
// Expected : init_logger call fopen with right arguments (log_path and APPEND_MODE)
// Param : logPathNotNull_logFileNull_FopenFail
static void initLogger_CallFopenWithRightParams_WhenLogPathIsNotNull_AndLogfileIsNull(void **state) {
    init_logger_test_params_t *params = (init_logger_test_params_t *) (*state);
    expect_string(fopen, path, params->log_path);
    expect_string(fopen, mode, APPEND_MODE);
    will_return(fopen, NULL);
    init_logger(params->log_path);
}

// Given: log_file is not NULL, log_file is NULL, fopen call fail
// Expected : init_logger returns -1 and log_file is not changed.
// Param : logPathNotNull_logFileNull_FopenFail
static void initLogger_ErrorAndNoLogfileChange_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenFail(void **state) {
    init_logger_test_params_t *params = (init_logger_test_params_t *) (*state);
    expect_string(fopen, path, params->log_path);
    expect_string(fopen, mode, APPEND_MODE);
    will_return(fopen, NULL);
    expect_init_logger_result_and_effect_on_logfile(params);
}

// Given: log_file is not NULL, log_file is NULL, fopen call success
// Expected : init_logger returns 0 and log_file value is the one returned by fopen
// Param : logPathNotNull_logFileNull_FopenSuccess
static void initLogger_SucessAndLogfileInitialized_WhenLogPathIsNotNull_AndLogfileIsNull_AndFopenSuccess(void **state) {
    init_logger_test_params_t *params = (init_logger_test_params_t *) (*state);
    expect_string(fopen, path, params->log_path);
    expect_string(fopen, mode, APPEND_MODE);
    will_return(fopen, &dummy_log_file);
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
    set_logger_fclose(mock_fclose);
    close_logger_test_params_t *params = (close_logger_test_params_t *) (*state);
    set_log_file(params->log_file_init);
    return 0;
}

static int close_logger_teardown(void **state) {
    set_log_file(NULL);
    set_logger_fclose(fclose);
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
    .log_file_init = &dummy_log_file,
    .expected_result = FAILURE,
    .expected_log_file_after_close_logger_call = &dummy_log_file
};

static close_logger_test_params_t logFileNotNull_FcloseSuccess = {
    .label = "log_file != NULL, fclose success",
    .log_file_init = &dummy_log_file,
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
    expect_close_logger_result_and_effect_on_logfile((close_logger_test_params_t *) (*state));
}

// Given: log_file != NULL
// Expected: close_logger calls fclose with parameter log_file
// Param : logFileNotNull_FcloseFail
static void close_logger_CallsFcloseWithRightParam_WhenLogfileIsNotNull(void **state) {
    close_logger_test_params_t *params = (close_logger_test_params_t *) (*state);
    expect_value(mock_fclose, file, params->log_file_init);
    will_return(mock_fclose, EOF);
    close_logger();
}

// Given: log_file != NULL and fclose returns EOF
// Expected: close_logger return -1 and log_file still points to the open file
// Param : logFileNotNull_FcloseFail
static void close_logger_ErrorAndNoLogfileChange_WhenLogfileIsNotNull_AndFcloseReturnsEOF(void **state) {
    close_logger_test_params_t *params = (close_logger_test_params_t *) (*state);
    expect_value(mock_fclose, file, params->log_file_init);
    will_return(mock_fclose, EOF);
    expect_close_logger_result_and_effect_on_logfile(params);
}

// Given: log_file != NULL and fclose returns 0
// Expected: close_logger returns 0 and log_file is NULL
// Param : logFileNotNull_FcloseSuccess
static void close_logger_SucessAndLogfileIsNull_WhenLogfileIsNotNull_AndFcloseReturns0(void **state) {
    close_logger_test_params_t *params = (close_logger_test_params_t *) (*state);
    expect_value(mock_fclose, file, params->log_file_init);
    will_return(mock_fclose, 0);
    expect_close_logger_result_and_effect_on_logfile(params);
}


//-----------------------------------------------------------------------------
// log_info TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int log_info_setup(void **state) {
    // environnement has been configured in the following cmake files :
    // src/osal/CMakeLists.txt
    // src/logger/tests/CMakeLists.txt
    // Ie "TZ=GMT-2" for Windows and "TZ=Europe/Paris" otherwise
    set_fake_time(EPOCH_TIME_2025_06_10_MIDDAY);
    if ((fake_log_file = fmemopen(fake_log_file_address, LOGGER_FILE_MAX_LENGTH, APPEND_MODE)) == NULL)
        return errno;
    memset(fake_log_file_address, 0, LOGGER_FILE_MAX_LENGTH * sizeof(char));
    return 0;
}

static int log_info_teardown(void **state) {
    fflush(fake_log_file);
    memset(fake_log_file_address, 0, LOGGER_FILE_MAX_LENGTH * sizeof(char));
    fclose(fake_log_file);
    set_log_file(NULL);
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------

// Given: set_fake_time is call with EPOCH_TIME_2025_06_10_MIDDAY
// Expected: time call returns EPOCH_TIME_2025_06_10_MIDDAY
static void fakeTime_TimeReturnsEPOCHTIME20250610MIDDAY_WhenSetFakeTimeWasCalledWithIt(void **state) {
    (void) *state; // unused
    assert_int_equal(time(NULL), EPOCH_TIME_2025_06_10_MIDDAY);
}

// Given: set_fake_time is called with 0
// Expected: time call does not return EPOCH_TIME_2025_06_10_MIDDAY
static void fakeTime_TimeDoesNotReturnEPOCHTIME20250610MIDDAY_WhenSetFakeTimeWasNotCalledWithIt(void **state) {
    (void) *state; // unused
    set_fake_time(FIRST_EPOCH_TIME);
    assert_int_not_equal(time(NULL), EPOCH_TIME_2025_06_10_MIDDAY);
}

// Given: log_file == NULL
// Expected: log_info returns -1, log_file remains NULL
static void log_info_ErrorAndNoLogfileChange_WhenLogfileIsNull(void **state) {
    (void) *state; // unused
    set_log_file(NULL);
    int result = log_info("dummy format", "dummy optionnal argument");
    assert_int_equal(result, FAILURE);
    assert_ptr_equal(get_log_file(), NULL);
}

// Given: log_file != NULL, format == NULL
// Expected: log_info returns -1 and log_file is not changed
static void log_info_ErrorAndNoLogfileChange_WhenFormatIsNull(void **state) {
    (void) *state; // unused
    set_log_file(fake_log_file);
    memset(fake_log_file_address, 0xaa, LOGGER_FILE_MAX_LENGTH);
    unsigned char *fake_log_file_save = malloc(LOGGER_FILE_MAX_LENGTH * sizeof(char));
    assert_non_null(fake_log_file_save);
    memcpy(fake_log_file_save, fake_log_file_address, LOGGER_FILE_MAX_LENGTH * sizeof(char));
    const char *format = NULL;
    int result = log_info(format, "dummy optionnal argument");
    assert_int_equal(result, FAILURE);
    assert_ptr_equal(get_log_file(), fake_log_file);
    assert_memory_equal(fake_log_file_address, fake_log_file_save, LOGGER_FILE_MAX_LENGTH);
    free(fake_log_file_save);
}

// Given: log_file != NULL, format == "no format operation", fake_log_file is empty
// Expected:
//     - fake_log_file contains "[2025-06-10 12:00:00] INFO: no format operation\n"
//     - log_info returns 0
// note : it is an isolated integration tests
static void log_info_SuccessAndLogfileContainsCorrectlyFilled_WhenValidParameters_AndNoFormatOperation(void **state) {
    (void) *state; // unused
    set_log_file(fake_log_file);
    memset(fake_log_file_address, 0xaa, LOGGER_FILE_MAX_LENGTH);
    unsigned char *fake_log_file_with_expected_content = malloc(LOGGER_FILE_MAX_LENGTH * sizeof(char));
    assert_non_null(fake_log_file_with_expected_content);
    memcpy(fake_log_file_with_expected_content, fake_log_file_address, LOGGER_FILE_MAX_LENGTH * sizeof(char));
    const char *expected_line = "[2025-06-10 12:00:00] INFO: no format operation\n";
    memcpy(fake_log_file_with_expected_content, expected_line, strlen(expected_line) + 1);
    int result = log_info("no format operation");
    assert_int_equal(result, SUCCESS);
    assert_ptr_equal(get_log_file(), fake_log_file);
    fflush(fake_log_file);
    assert_memory_equal(
        fake_log_file_address,
        fake_log_file_with_expected_content,
        LOGGER_FILE_MAX_LENGTH
    );
    free(fake_log_file_with_expected_content);
}

// Given: log_file != NULL, format == "no format operation", fake_log_file is empty
// Expected:
//     - fake_log_file contains :
//         [2025-06-10 12:00:00] INFO: It's Adeline's birthday!
//         [2025-05-07 12:00:00] INFO: It's Sylvain's birthday!
//     - log_info returns 0
// note : it is an isolated integration tests
static void
log_info_SuccessAndLogfileContainsCorrectlyFilled_WhenValidParameters_AndNoFormatOperation_AndTwoLogInfoCalls(
    void **state) {
    (void) *state; // unused
    set_log_file(fake_log_file);
    memset(fake_log_file_address, 0xaa, LOGGER_FILE_MAX_LENGTH);
    unsigned char *fake_log_file_with_expected_content = malloc(LOGGER_FILE_MAX_LENGTH * sizeof(char));
    assert_non_null(fake_log_file_with_expected_content);
    memcpy(fake_log_file_with_expected_content, fake_log_file_address, LOGGER_FILE_MAX_LENGTH * sizeof(char));
    const char *expected_line = "[2025-06-10 12:00:00] INFO: It's Adeline's birthday!\n\
[2025-05-07 12:00:00] INFO: It's Sylvain's birthday!\n";
    memcpy(fake_log_file_with_expected_content, expected_line, strlen(expected_line) + 1);
    int result = log_info("It's Adeline's birthday!");
    set_fake_time(EPOCH_TIME_2025_05_07_MIDDAY);
    result += log_info("It's Sylvain's birthday!");
    assert_int_equal(result, SUCCESS);
    assert_ptr_equal(get_log_file(), fake_log_file);
    fflush(fake_log_file);
    assert_memory_equal(
        fake_log_file_address,
        fake_log_file_with_expected_content,
        LOGGER_FILE_MAX_LENGTH
    );
    free(fake_log_file_with_expected_content);
}

// Given:
//     - fake_log_file != NULL
//     - fake_log_file is empty
//     - format == "an int and a string: %i, %s"
//     - a first optionnal argument == 20250610
//     - a second optionnal argument == "It's Adeline's birthday!"
// Expected:
//     - fake_log_file contains "[2025-06-10 12:00:00] INFO: an int and a string: 20250610, It's Adeline's birthday!\n"
//     - log_info returns 0
// note : it is an isolated integration tests
static void log_info_SuccessAndLogfileContainsCorrectlyFilled_WhenValidParameters_AndFormatOperation(void **state) {
    (void) *state; // unused
    set_log_file(fake_log_file);
    memset(fake_log_file_address, 0xaa, LOGGER_FILE_MAX_LENGTH);
    unsigned char *fake_log_file_with_expected_content = malloc(LOGGER_FILE_MAX_LENGTH * sizeof(char));
    assert_non_null(fake_log_file_with_expected_content);
    memcpy(fake_log_file_with_expected_content, fake_log_file_address, LOGGER_FILE_MAX_LENGTH * sizeof(char));
    const char *expected_line = "[2025-06-10 12:00:00] INFO: an int and a string: 20250610, It's Adeline's birthday!\n";
    memcpy(fake_log_file_with_expected_content, expected_line, strlen(expected_line) + 1);
    int result = log_info("an int and a string: %i, %s", 20250610, "It's Adeline's birthday!");
    assert_int_equal(result, SUCCESS);
    assert_ptr_equal(get_log_file(), fake_log_file);
    fflush(fake_log_file);
    assert_memory_equal(
        fake_log_file_address,
        fake_log_file_with_expected_content,
        LOGGER_FILE_MAX_LENGTH
    );
    free(fake_log_file_with_expected_content);
}


//-----------------------------------------------------------------------------
// log_error TESTS
//-----------------------------------------------------------------------------
//
// Note:
//
// We do not add a dedicated suite of tests for log_error, since this function
// is a simple wrapper around log_message() with the level set to "ERROR".
// All relevant cases (success, error, argument handling) are already
// covered by the tests for log_info, which uses the same logic.
//
// A minimal tests could be added to check that "ERROR" appears in the log line,
// but as it stands, this would mainly duplicate existing coverage.
//
// If log_error evolves independently in the future, it would then make sense
// to add specific tests.
//
// -----------------------------------------------------------------------------


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
    const struct CMUnitTest log_info_tests[] = {
        cmocka_unit_test_setup_teardown(
            fakeTime_TimeReturnsEPOCHTIME20250610MIDDAY_WhenSetFakeTimeWasCalledWithIt,
            log_info_setup, log_info_teardown),
        cmocka_unit_test_setup_teardown(
            fakeTime_TimeDoesNotReturnEPOCHTIME20250610MIDDAY_WhenSetFakeTimeWasNotCalledWithIt,
            log_info_setup, log_info_teardown),
        cmocka_unit_test_setup_teardown(
            log_info_ErrorAndNoLogfileChange_WhenLogfileIsNull,
            log_info_setup, log_info_teardown),
        cmocka_unit_test_setup_teardown(
            log_info_ErrorAndNoLogfileChange_WhenFormatIsNull,
            log_info_setup, log_info_teardown),
        cmocka_unit_test_setup_teardown(
            log_info_SuccessAndLogfileContainsCorrectlyFilled_WhenValidParameters_AndNoFormatOperation,
            log_info_setup, log_info_teardown),
        cmocka_unit_test_setup_teardown(
            log_info_SuccessAndLogfileContainsCorrectlyFilled_WhenValidParameters_AndNoFormatOperation_AndTwoLogInfoCalls,
            log_info_setup, log_info_teardown),
        cmocka_unit_test_setup_teardown(
            log_info_SuccessAndLogfileContainsCorrectlyFilled_WhenValidParameters_AndFormatOperation,
            log_info_setup, log_info_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(init_logger_tests, NULL, NULL);
    failed += cmocka_run_group_tests(close_logger_tests, NULL, NULL);
    failed += cmocka_run_group_tests(log_info_tests, NULL, NULL);
}