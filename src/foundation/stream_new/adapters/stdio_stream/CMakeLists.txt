# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    stdio_stream_new_cr
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stdio_stream_cr.c"
)

target_include_directories(
    stdio_stream_new_cr
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    stdio_stream_new_cr
    PUBLIC
        stream_new_adapter
        osal_stdio
        osal_mem
    PRIVATE
        lexleo_assert
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
