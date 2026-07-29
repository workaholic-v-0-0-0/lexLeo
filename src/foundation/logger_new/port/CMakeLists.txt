# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    logger_new_borrower
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_borrower.c"
)
target_include_directories(
    logger_new_borrower
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/borrowers"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
)
target_link_libraries(
    logger_new_borrower
    PUBLIC
        lexleo_policy_headers
    PRIVATE
        osal_mem
        lexleo_assert
)

add_library(
    logger_new_cr
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_cr.c"
)
target_include_directories(
    logger_new_cr
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cr"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    logger_new_cr
    PUBLIC
        osal_mem
        lexleo_policy_headers
    PRIVATE
        lexleo_assert
)

add_library(
    logger_new_adapter
    INTERFACE
)
target_include_directories(
    logger_new_adapter
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common"
)
target_link_libraries(
    logger_new_adapter
    INTERFACE
        osal_mem
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
