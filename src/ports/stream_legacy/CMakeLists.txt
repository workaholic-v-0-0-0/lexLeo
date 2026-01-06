# src/frontend/ports/stream_legacy/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)

add_library(
    stream_legacy
    STATIC
    src/stream_legacy.c
)

if (BUILD_TESTING)
    target_compile_definitions(
        stream_legacy
        PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        UNIT_TEST
    )
endif ()

# TODO
#[[
if (BUILD_TESTING)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()
#]]

target_include_directories(
    stream_legacy
    PUBLIC include
)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(stream_legacy PRIVATE USE_LOGGER)
    target_include_directories(stream_legacy PRIVATE ${LOGGER_HEADER_DIR})
    target_link_libraries(stream_legacy PRIVATE ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(stream_legacy PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(stream_legacy PRIVATE ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(stream_legacy PRIVATE ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(stream_legacy PRIVATE USE_STRING_UTILS)
    target_include_directories(stream_legacy PRIVATE ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(stream_legacy PRIVATE ${STRING_UTILS_TARGET})
endif ()
