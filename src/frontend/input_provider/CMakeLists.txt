# src/frontend/input_provider/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)

add_library(
    input_provider
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/input_provider.c
)

if (BUILD_TESTING)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()

target_include_directories(
    input_provider
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_SOURCE_DIR}/src/frontend/ports/stream/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/adapters/stream/dynamic_buffer_stream/include
    PRIVATE ${CMAKE_BINARY_DIR}/src/lexer/include
)

target_link_libraries(input_provider
    PRIVATE stream dynamic_buffer_stream
)

target_compile_definitions(input_provider PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(input_provider PRIVATE USE_LOGGER)
    target_include_directories(input_provider PRIVATE ${LOGGER_HEADER_DIR})
    target_link_libraries(input_provider PRIVATE ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(input_provider PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(input_provider PRIVATE ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(input_provider PRIVATE ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(input_provider PRIVATE USE_STRING_UTILS)
    target_include_directories(input_provider PRIVATE ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(input_provider PRIVATE ${STRING_UTILS_TARGET})
endif ()
