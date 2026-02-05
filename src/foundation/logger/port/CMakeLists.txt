# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin
#
# File:
# src/foundation/logger/port/
# CMakeLists.txt

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
        osal_mem_legacy
        lexleo_assert
)

add_library(logger_wiring_headers INTERFACE)
target_include_directories(
    logger_wiring_headers
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(
    logger_wiring_headers
    INTERFACE
        logger
        osal_mem_wiring_headers
)

add_subdirectory(tests)
