# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    stream_borrower
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/borrowers/stream_borrower.c"
)
target_include_directories(
    stream_borrower
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/borrowers"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
)
target_link_libraries(
    stream_borrower
    PUBLIC
        lexleo_policy_headers
    PRIVATE
        osal_mem
        lexleo_policy_headers
        lexleo_assert
)

file(
    GLOB STREAM_CREATOR_OWNERS_SOURCES
    CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/owners/creators/*.c"
)
add_library(
    stream_owner
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/owners/stream_owner.c"
        ${STREAM_CREATOR_OWNERS_SOURCES}
)
target_include_directories(
    stream_owner
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/owners"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
)
target_link_libraries(
    stream_owner
    PUBLIC
        stream_borrower
    PRIVATE
        osal_mem
        osal_str
        lexleo_policy_headers
        lexleo_assert
)

add_library(
    stream_adapter
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/adapters/stream_adapter.c"
)
target_include_directories(
    stream_adapter
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    stream_adapter
    PUBLIC
        stream_owner
    PRIVATE
        osal_mem
        lexleo_policy_headers
)

file(
    GLOB STREAM_CREATOR_CR_SOURCES
    CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/cr/creators/*.c"
)
add_library(
    stream_cr
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cr/stream_cr.c"
        ${STREAM_CREATOR_CR_SOURCES}
)
target_include_directories(
    stream_cr
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cr"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    stream_cr
    PUBLIC
        stream_adapter
    PRIVATE
        osal_mem
        osal_str
        lexleo_policy_headers
        lexleo_assert
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
