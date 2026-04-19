# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    fs_stream
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fs_stream.c"
)

target_include_directories(
    fs_stream
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    fs_stream
    PRIVATE
        stream
        osal_mem
        osal_file
        lexleo_policy_headers
        lexleo_assert
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
