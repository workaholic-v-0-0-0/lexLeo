# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    osal_time
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/osal_time.c"
)

target_include_directories(
    osal_time
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/internal"
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    osal_time
    PUBLIC
        lexleo_policy_headers
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
