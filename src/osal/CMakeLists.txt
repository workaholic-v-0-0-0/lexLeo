# bison/osal/CMakeLists.txt

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(osal STATIC
    $<$<PLATFORM_ID:Windows>:${SRC}/osal_windows.c>
    $<$<PLATFORM_ID:Darwin>:${SRC}/osal_macos.c>
    $<$<PLATFORM_ID:Linux,FreeBSD,OpenBSD,SunOS>:${SRC}/osal_unix.c>
)
target_include_directories(osal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# fake time zone
if(WIN32)
    set(FAKE_TIME_ZONE "TZ=GMT-2" CACHE STRING "fake time zone to test logger")
else()
    set(FAKE_TIME_ZONE "TZ=Europe/Paris" CACHE STRING "fake time zone to test logger")
endif()
