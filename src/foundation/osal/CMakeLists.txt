# src/foundation/osal/CMakeLists.txt

include(CheckSymbolExists)

if (NOT WIN32 AND NOT APPLE)
    set(CMAKE_REQUIRED_DEFINITIONS_SAVE "${CMAKE_REQUIRED_DEFINITIONS}")
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
    check_symbol_exists(fmemopen "stdio.h" OSAL_HAVE_FMEMOPEN)
    check_symbol_exists(open_memstream "stdio.h" OSAL_HAVE_OPEN_MEMSTREAM)
    check_symbol_exists(fopencookie "stdio.h" OSAL_HAVE_FOPENCOOKIE)
    set(CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS_SAVE}")
else ()
    set(OSAL_HAVE_FMEMOPEN 0)
    set(OSAL_HAVE_OPEN_MEMSTREAM 0)
    set(OSAL_HAVE_FOPENCOOKIE 0)
endif ()

configure_file(
    include/osal_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/osal_config.h
)

set(SRC src)

add_library(
    osal
    STATIC
    ${SRC}/common/osal_mem.c
    $<$<PLATFORM_ID:Windows>:${SRC}/win32/osal_windows.c>
    $<$<PLATFORM_ID:Darwin>:${SRC}/macos/osal_macos.c>
    $<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,SunOS>:${SRC}/posix/osal_unix.c>
)

target_include_directories(osal
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mem
    ${CMAKE_CURRENT_BINARY_DIR}/include
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src/include
)
target_compile_definitions(
    osal
    PRIVATE
    $<$<PLATFORM_ID:Linux>:_GNU_SOURCE>
)
add_library(
    osal_mem
    STATIC
    ${SRC}/common/osal_mem.c
)

target_include_directories(
    osal_mem
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mem"
)

add_library(osal_mem_wiring_headers INTERFACE)

target_link_libraries(
    osal_mem_wiring_headers
    INTERFACE
)

add_library(
    osal_file
    STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/osal_file.c"
    "$<$<PLATFORM_ID:Windows>:${SRC}/win32/osal_file_win32.c>"
    "$<$<PLATFORM_ID:Linux,Darwin,FreeBSD,OpenBSD,SunOS>:${SRC}/posix/osal_file_posix.c>"
)
target_include_directories(
    osal_file
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(
    osal_file
    PRIVATE
        lexleo_panic
)

# fake time zone
if (WIN32)
    set(FAKE_TIME_ZONE "TZ=GMT-2" CACHE STRING "fake time zone to test logger")
else ()
    set(FAKE_TIME_ZONE "TZ=Europe/Paris" CACHE STRING "fake time zone to test logger")
endif ()

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
