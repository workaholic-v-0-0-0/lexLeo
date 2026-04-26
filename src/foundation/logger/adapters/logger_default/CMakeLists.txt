# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    logger_default
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_default.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/internal/logger_default_utc_timestamp.c"
)

target_include_directories(
    logger_default
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    logger_default
    PUBLIC
        logger
        stream
        osal_time
        osal_mem
        osal_str
    PRIVATE
        lexleo_assert
)

if (BUILD_TESTING)
    #add_subdirectory(tests)
endif ()
