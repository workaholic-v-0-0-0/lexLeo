# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    integration_test_logger_logger_default_stream_osal_time_doubled
    "${CMAKE_CURRENT_SOURCE_DIR}/integration_test_logger_logger_default_stream_osal_time_doubled.c"
)
target_link_libraries(
    integration_test_logger_logger_default_stream_osal_time_doubled
    PRIVATE
        logger_new_cr
        logger_new_borrower
        logger_new_adapter
        logger_default_cr
        fake_stream_new
        osal_time_fake_provider
        osal_mem
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)
add_test(
    NAME integration_test_logger_logger_default_stream_osal_time_doubled
    COMMAND integration_test_logger_logger_default_stream_osal_time_doubled
)

find_program(VALGRIND_EXECUTABLE valgrind)

if(VALGRIND_EXECUTABLE)
    add_test(
        NAME integration_test_logger_logger_default_stream_osal_time_doubled_memory
        COMMAND
        ${VALGRIND_EXECUTABLE}
        --leak-check=full
        --show-leak-kinds=all
        --error-exitcode=1
        $<TARGET_FILE:integration_test_logger_logger_default_stream_osal_time_doubled>
    )
    set_tests_properties(
        integration_test_logger_logger_default_stream_osal_time_doubled_memory
        PROPERTIES LABELS "memory"
    )
endif()
