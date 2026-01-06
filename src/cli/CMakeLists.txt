# src/cli/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(cli LANGUAGES C)

if (BUILD_TESTING)
    add_library(
        cli
        STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cli.c
    )
    target_compile_definitions(cli PRIVATE UNIT_TEST)
    target_compile_definitions(cli PRIVATE DEBUG)
    #include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
else ()
    add_library(
        cli
        STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cli.c
    )
endif ()

target_include_directories(
    cli
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/ports/stream_legacy/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/adapters/stream_legacy/stdio_stream_legacy/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/core/runtime_env/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/core/runtime_session/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/core/interpreter/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/foundation/input_provider_legacy/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/ports/lexer_legacy/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/ports/parser/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/core/resolver/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/core/interpreter/include
)
target_link_libraries(
    cli
    PRIVATE stream_legacy
    PRIVATE stdio_stream_legacy
    PRIVATE runtime_env
    PRIVATE runtime_session
    PRIVATE interpreter
    PRIVATE input_provider_legacy
    PRIVATE lexer_legacy
    PRIVATE parser
    PRIVATE resolver
    PRIVATE interpreter
)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(cli PRIVATE USE_LOGGER)
    target_include_directories(cli PUBLIC ${LOGGER_HEADER_DIR})
    target_link_libraries(cli PUBLIC ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(cli PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(cli PUBLIC ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(cli PUBLIC ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(cli PRIVATE USE_STRING_UTILS)
    target_include_directories(cli PUBLIC ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(cli PUBLIC ${STRING_UTILS_TARGET})
endif ()
