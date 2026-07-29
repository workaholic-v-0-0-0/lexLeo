# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Sylvain Labopin

add_library(
    stream_new_borrower
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/borrowers/stream_borrower.c"
)
target_include_directories(
    stream_new_borrower
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/borrowers"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
)
target_link_libraries(
    stream_new_borrower
    PUBLIC
        lexleo_policy_headers
    PRIVATE
        osal_mem
        lexleo_assert
)

file(
    GLOB STREAM_CREATOR_OWNERS_SOURCES
    CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/owners/creators/*.c"
)
add_library(
    stream_new_owner
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/owners/stream_owner.c"
        ${STREAM_CREATOR_OWNERS_SOURCES}
)
target_include_directories(
    stream_new_owner
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/owners"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
)
target_link_libraries(
    stream_new_owner
    PUBLIC
        stream_new_borrower
    PRIVATE
        osal_mem
        osal_str
        lexleo_policy_headers
        lexleo_assert
)

file(
    GLOB STREAM_CREATOR_CR_SOURCES
    CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/cr/creators/*.c"
)
add_library(
    stream_new_cr
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cr/stream_cr.c"
        ${STREAM_CREATOR_CR_SOURCES}
)
target_include_directories(
    stream_new_cr
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cr"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    stream_new_cr
    PUBLIC
        stream_new_owner
        osal_mem
        lexleo_policy_headers
    PRIVATE
        osal_str
        lexleo_assert
)

add_library(
    stream_new_adapter
    INTERFACE
)
target_include_directories(
    stream_new_adapter
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/adapters"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/common"
)
target_link_libraries(
    stream_new_adapter
    INTERFACE
        osal_mem
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
