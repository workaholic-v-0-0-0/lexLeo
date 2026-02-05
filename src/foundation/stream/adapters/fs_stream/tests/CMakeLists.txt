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

add_library(fs_stream_test_headers INTERFACE)
target_include_directories(
    fs_stream_test_headers
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(fs_stream_test_usage_requirements INTERFACE)
target_link_libraries(
    fs_stream_test_usage_requirements
    INTERFACE
        fs_stream_cr_headers
        fs_stream_test_headers
        stream
        osal_mem_fake_provider
        osal_file_fake_provider
        osam_mem
)
#<here> struggle to compile first unit test with beautiful injection
# start with doing a best cmake config with ideas "_test_usage_requirements,
# _test_headers,..."
target_link_libraries(
    unit_test_fs_stream
    PRIVATE
        fs_stream_test_usage_requirements
        lexleo_cmocka_support
        cmocka
)

add_test(
    NAME unit_test_fs_stream
    COMMAND unit_test_fs_stream
)

