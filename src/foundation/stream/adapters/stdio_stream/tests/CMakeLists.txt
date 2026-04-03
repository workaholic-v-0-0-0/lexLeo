# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_subdirectory(support)

add_executable(
    unit_test_stdio_stream
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_stdio_stream.c"
)

target_link_libraries(
    unit_test_stdio_stream
    PRIVATE
        stream
        stream_white_box_tests_access
        stdio_stream
        stdio_stream_white_box_tests_access
        osal_stdio_fake_provider
        osal_mem_fake_provider
        lexleo_cmocka_support
        cmocka
)

add_test(
    NAME unit_test_stdio_stream
    COMMAND unit_test_stdio_stream
)

find_program(VALGRIND_EXECUTABLE valgrind)

if(VALGRIND_EXECUTABLE)
    add_test(
        NAME unit_test_stdio_stream_memory
        COMMAND
        ${VALGRIND_EXECUTABLE}
        --leak-check=full
        --error-exitcode=1
        $<TARGET_FILE:unit_test_stdio_stream>
    )
    set_tests_properties(
        unit_test_stdio_stream_memory
        PROPERTIES LABELS "memory"
    )
endif()
