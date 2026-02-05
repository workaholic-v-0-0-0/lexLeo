# src/foundation/osal/osal_file/CMakeLists.txt

add_library(
    osal_file
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/common/osal_file.c"
        "$<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_SOURCE_DIR}/src/win32/osal_file_win32.c>"
        "$<$<PLATFORM_ID:Linux,Darwin,FreeBSD,OpenBSD,SunOS>:${CMAKE_CURRENT_SOURCE_DIR}/src/posix/osal_file_posix.c>"
)

target_include_directories(
    osal_file
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/common/internal/osal_file_internal.h"
)

target_link_libraries(
    osal_file
    PRIVATE
        osal_mem
        lexleo_panic
)

add_subdirectory(tests)
