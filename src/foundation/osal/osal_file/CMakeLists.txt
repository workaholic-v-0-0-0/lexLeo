set(
    SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set(
    OSAL_SRC
    "$<$<PLATFORM_ID:Windows>:${SRC_DIR}/osal_file_win32.c>"
    "$<$<PLATFORM_ID:Darwin>:${SRC_DIR}/osal_file_posix.c>"
    "$<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,SunOS>:${SRC_DIR}/osal_file_posix.c>"
)

add_library(
    osal_file
    STATIC
        ${OSAL_SRC}
)

target_include_directories(
    osal_file
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    osal_file
    PUBLIC
        osal_mem
        lexleo_policy_headers
    PRIVATE
        lexleo_assert
)

add_subdirectory(tests)
