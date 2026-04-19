# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    stream
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stream.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/stream_factory.c"
)

target_include_directories(
    stream
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    stream
    PRIVATE
        osal_mem
        lexleo_assert
)

add_library(stream_cr_headers INTERFACE)
target_link_libraries(
    stream_cr_headers
    INTERFACE
        stream
        osal_mem_cr_headers
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
