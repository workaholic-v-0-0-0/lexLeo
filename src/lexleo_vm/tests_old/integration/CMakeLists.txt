# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_executable(
    integration_test_lexleo_vm_cr_stream_creators
    "${CMAKE_CURRENT_SOURCE_DIR}/integration_test_lexleo_vm_cr_stream_creators.c"
)

target_link_libraries(
    integration_test_lexleo_vm_cr_stream_creators
    PRIVATE
        lexleo_vm_white_box_tests_access
        stream_owner
        osal_str
        lexleo_cmocka_support
)

add_test(
    NAME integration_test_lexleo_vm_cr_stream_creators
    COMMAND integration_test_lexleo_vm_cr_stream_creators
)

find_program(VALGRIND_EXECUTABLE valgrind)

if(VALGRIND_EXECUTABLE)
    add_test(
        NAME integration_test_lexleo_vm_cr_stream_creators_memory
        COMMAND
        ${VALGRIND_EXECUTABLE}
        --leak-check=full
        --error-exitcode=1
        $<TARGET_FILE:integration_test_lexleo_vm_cr_stream_creators>
    )
    set_tests_properties(
        integration_test_lexleo_vm_cr_stream_creators_memory
        PROPERTIES LABELS "memory"
    )
endif()
