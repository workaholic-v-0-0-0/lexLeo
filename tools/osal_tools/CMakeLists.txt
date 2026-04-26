# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

set(
    OSAL_TOOLS_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set(
    OSAL_TOOLS_SOURCES
    "$<$<PLATFORM_ID:Windows>:${OSAL_TOOLS_SOURCE_DIR}/osal_tools_win32.c>"
    "$<$<PLATFORM_ID:Darwin>:${OSAL_TOOLS_SOURCE_DIR}/osal_tools_posix.c>"
    "$<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,SunOS>:${OSAL_TOOLS_SOURCE_DIR}/osal_tools_posix.c>"
)

add_library(
    osal_tools
    STATIC
        ${OSAL_TOOLS_SOURCES}
)

target_include_directories(
    osal_tools
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    osal_tools
    PRIVATE
        lexleo_policy_headers
)

set(
    OSAL_PROCESS_SOURCES
    "$<$<PLATFORM_ID:Windows>:${OSAL_TOOLS_SOURCE_DIR}/osal_process_win32.c>"
    "$<$<PLATFORM_ID:Darwin>:${OSAL_TOOLS_SOURCE_DIR}/osal_process_posix.c>"
    "$<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,SunOS>:${OSAL_TOOLS_SOURCE_DIR}/osal_process_posix.c>"
)

add_library(
    osal_process
    STATIC
        ${OSAL_PROCESS_SOURCES}
)

target_include_directories(
    osal_process
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    osal_process
    PRIVATE
        osal_str
        lexleo_policy_headers
)
