# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    dynamic_buffer_stream_new_cr
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/dynamic_buffer_stream_cr.c"
)

target_include_directories(
    dynamic_buffer_stream_new_cr
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cr"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    dynamic_buffer_stream_new_cr
    PUBLIC
        stream_new_adapter
        osal_mem
        lexleo_policy_headers
    PRIVATE
        lexleo_assert
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
