# src/foundation/input_provider_legacy/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)

add_library(
    input_provider_legacy
    STATIC
    src/input_provider_legacy.c
)

if (BUILD_TESTING)
    include(tests/tests.cmake)
endif ()

target_include_directories(
    input_provider_legacy
    PUBLIC include
    PUBLIC ${CMAKE_SOURCE_DIR}/src/ports/stream_legacy/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/adapters/stream_legacy/dynamic_buffer_stream_legacy/include
    PRIVATE ${CMAKE_BINARY_DIR}/src/ports/lexer/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/include
)

target_link_libraries(input_provider_legacy
    PRIVATE stream_legacy dynamic_buffer_stream_legacy
)

target_compile_definitions(input_provider_legacy PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(input_provider_legacy PRIVATE USE_LOGGER)
    target_include_directories(input_provider_legacy PRIVATE ${LOGGER_HEADER_DIR})
    target_link_libraries(input_provider_legacy PRIVATE ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(input_provider_legacy PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(input_provider_legacy PRIVATE ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(input_provider_legacy PRIVATE ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(input_provider_legacy PRIVATE USE_STRING_UTILS)
    target_include_directories(input_provider_legacy PRIVATE ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(input_provider_legacy PRIVATE ${STRING_UTILS_TARGET})
endif ()
