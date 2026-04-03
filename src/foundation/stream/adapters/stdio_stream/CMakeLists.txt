# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin
#
# File:
# src/foundation/stream/adapters/stdio_stream/
# CMakeLists.txt

add_library(
    stdio_stream
    STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stdio_stream.c"
)

target_include_directories(
    stdio_stream
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    stdio_stream
    PUBLIC
        stream
        osal_stdio
        osal_mem
    PRIVATE
        lexleo_assert
)

add_subdirectory(tests)
