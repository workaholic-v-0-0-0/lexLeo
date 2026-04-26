# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    unit_test_stream
    "${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_stream.c"
)

target_link_libraries(
    unit_test_stream
    PRIVATE
        stream
        osal_mem_fake_provider
        osal_str
        lexleo_policy_headers
        lexleo_cmocka_support
)

add_test(
    NAME unit_test_stream
    COMMAND unit_test_stream
)
#[[
add_executable(
    integration_test_stream_factory_fs_stream
    "${CMAKE_CURRENT_SOURCE_DIR}/integration/integration_test_stream_factory_fs_stream.c"
)

target_link_libraries(
    integration_test_stream_factory_fs_stream
    PRIVATE
        stream
        fs_stream
        osal_mem
        osal_file_fake_provider
        lexleo_policy_headers
        lexleo_cmocka_support
)

add_test(
    NAME integration_test_stream_factory_fs_stream
    COMMAND integration_test_stream_factory_fs_stream
)

find_program(VALGRIND_EXECUTABLE valgrind)

if(VALGRIND_EXECUTABLE)
    add_test(
        NAME integration_test_stream_factory_fs_stream_memory
        COMMAND
        ${VALGRIND_EXECUTABLE}
        --leak-check=full
        --error-exitcode=1
        $<TARGET_FILE:integration_test_stream_factory_fs_stream>
    )
    set_tests_properties(
        integration_test_stream_factory_fs_stream_memory
        PROPERTIES LABELS "memory"
    )
endif()

add_executable(
    integration_test_stream_factory
    "${CMAKE_CURRENT_SOURCE_DIR}/integration/integration_test_stream_factory.c"
)

target_link_libraries(
    integration_test_stream_factory
    PRIVATE
        stream
        fs_stream
        osal_mem
        osal_file_fake_provider
        lexleo_policy_headers
        lexleo_cmocka_support
)

add_test(
    NAME integration_test_stream_factory
    COMMAND integration_test_stream_factory
)

find_program(VALGRIND_EXECUTABLE valgrind)

if(VALGRIND_EXECUTABLE)
    add_test(
        NAME integration_test_stream_factory_memory
        COMMAND
        ${VALGRIND_EXECUTABLE}
        --leak-check=full
        --error-exitcode=1
        $<TARGET_FILE:integration_test_stream_factory>
    )
    set_tests_properties(
        integration_test_stream_factory_memory
        PROPERTIES LABELS "memory"
    )
endif()
#]]
add_subdirectory(support)
