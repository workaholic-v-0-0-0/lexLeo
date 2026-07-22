# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    logger_borrower
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_borrower.c"
)
target_include_directories(
    logger_borrower
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/borrowers"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
)
target_link_libraries(
    logger_borrower
    PRIVATE
        osal_mem
        lexleo_assert
)

add_library(
    logger_owner
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_owner.c"
)
target_include_directories(
    logger_owner
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/owners"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
)
target_link_libraries(
    logger_owner
    PUBLIC
        logger_borrower
    PRIVATE
        osal_mem
        lexleo_assert
)

add_library(
    logger_adapter
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_adapter.c"
)
target_include_directories(
    logger_adapter
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    logger_adapter
    PUBLIC
        logger_owner
    PRIVATE
        osal_mem
        lexleo_assert
)

# No logger_cr as no logger creators are implemented.

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
