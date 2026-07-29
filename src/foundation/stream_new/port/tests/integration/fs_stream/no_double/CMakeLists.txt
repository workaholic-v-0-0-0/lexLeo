# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    integration_test_stream_fs_stream_no_double
    "${CMAKE_CURRENT_SOURCE_DIR}/integration_test_stream_fs_stream_no_double.c"
)
target_link_libraries(
    integration_test_stream_fs_stream_no_double
    PRIVATE
        stream_new_cr
        stream_new_adapter
        fs_stream_new_cr
        osal_mem
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)
add_test(
    NAME integration_test_stream_fs_stream_no_double
    COMMAND integration_test_stream_fs_stream_no_double
)

find_program(VALGRIND_EXECUTABLE valgrind)

if(VALGRIND_EXECUTABLE)
    add_test(
        NAME integration_test_stream_fs_stream_no_double_memory
        COMMAND
        ${VALGRIND_EXECUTABLE}
        --leak-check=full
        --show-leak-kinds=all
        --error-exitcode=1
        $<TARGET_FILE:integration_test_stream_fs_stream_no_double>
    )
    set_tests_properties(
        integration_test_stream_fs_stream_no_double_memory
        PROPERTIES LABELS "memory"
    )
endif()
