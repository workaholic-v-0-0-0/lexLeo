# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    logger
    STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/logger.c"
)

target_include_directories(
    logger
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    logger
    PRIVATE
    osal_mem
    lexleo_assert
)

add_library(logger_cr_headers INTERFACE)

target_link_libraries(
    logger_cr_headers
    INTERFACE
    logger
    osal_mem_cr_headers
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
