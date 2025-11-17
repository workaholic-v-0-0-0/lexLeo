# src/osal/CMakeLists.txt

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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/osal_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/osal_config.h
)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(osal STATIC
    $<$<PLATFORM_ID:Windows>:${SRC}/osal_windows.c>
    $<$<PLATFORM_ID:Darwin>:${SRC}/osal_macos.c>
    $<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,SunOS>:${SRC}/osal_unix.c>
)
target_include_directories(osal
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)
target_compile_definitions(osal PRIVATE _GNU_SOURCE)

# fake time zone
if (WIN32)
    set(FAKE_TIME_ZONE "TZ=GMT-2" CACHE STRING "fake time zone to test logger")
else ()
    set(FAKE_TIME_ZONE "TZ=Europe/Paris" CACHE STRING "fake time zone to test logger")
endif ()
