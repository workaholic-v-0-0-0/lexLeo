# src/adapters/dynamic_buffer_stream/dynamic_buffer_stream/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)

add_library(
    dynamic_buffer_stream
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dynamic_buffer_stream.c
)

if (BUILD_TESTING)
    target_compile_definitions(dynamic_buffer_stream PRIVATE UNIT_TEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()

target_include_directories(
    dynamic_buffer_stream
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PUBLIC "${CMAKE_SOURCE_DIR}/src/frontend/ports/stream/include/"
)

target_link_libraries(dynamic_buffer_stream
    PRIVATE stream
    #PRIVATE $<$<BOOL:${BUILD_TESTING}>:extra_for_tests>
)

target_compile_definitions(dynamic_buffer_stream PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(dynamic_buffer_stream PRIVATE USE_LOGGER)
    target_include_directories(dynamic_buffer_stream PRIVATE ${LOGGER_HEADER_DIR})
    target_link_libraries(dynamic_buffer_stream PRIVATE ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(dynamic_buffer_stream PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(dynamic_buffer_stream PRIVATE ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(dynamic_buffer_stream PRIVATE ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(dynamic_buffer_stream PRIVATE USE_STRING_UTILS)
    target_include_directories(dynamic_buffer_stream PRIVATE ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(dynamic_buffer_stream PRIVATE ${STRING_UTILS_TARGET})
endif ()
