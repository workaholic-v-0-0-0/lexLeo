# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin
#
# File:
# src/foundation/stream/adapters/fs_stream/tests/
# CMakeLists.txt

add_executable(
    unit_test_fs_stream
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_fs_stream.c"
)

target_link_libraries(
    unit_test_fs_stream
    PRIVATE
        stream
        fs_stream
        osal_mem_fake_provider
        osal_file_fake_provider
        lexleo_policy_headers
        lexleo_cmocka_support
        cmocka
)

add_test(
    NAME unit_test_fs_stream
    COMMAND unit_test_fs_stream
)

find_program(VALGRIND_EXECUTABLE valgrind)

if(VALGRIND_EXECUTABLE)
    add_test(
        NAME unit_test_fs_stream_memory
        COMMAND
            ${VALGRIND_EXECUTABLE}
            --leak-check=full
            --error-exitcode=1
            $<TARGET_FILE:unit_test_fs_stream>
    )
    set_tests_properties(
        unit_test_fs_stream_memory
        PROPERTIES LABELS "memory"
    )
endif()
