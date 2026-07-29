# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    logger_default_cr
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_default_cr.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/internal/logger_default_utc_timestamp.c"
)

target_include_directories(
    logger_default_cr
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cr"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    logger_default_cr
    PUBLIC
        lexleo_policy_headers
    PRIVATE
        logger_new_adapter
        stream_new_borrower
        osal_time
        osal_str
        lexleo_assert
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
