# src/core/runtime_env/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(runtime_env LANGUAGES C)

if (BUILD_TESTING)
    add_library(
        runtime_env
        STATIC
        src/runtime_env.c
        src/internal/runtime_env_internal.c
    )
    target_compile_definitions(runtime_env PRIVATE UNIT_TEST)
    include(tests/tests.cmake)
else ()
    add_library(
        runtime_env
        STATIC
        src/runtime_env.c
        src/internal/runtime_env_internal.c
    )
endif ()

target_include_directories(
    runtime_env
    PUBLIC include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/foundation/data_structures/include
)
target_link_libraries(runtime_env PUBLIC data_structures)
target_compile_definitions(runtime_env PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(runtime_env PRIVATE USE_LOGGER)
    target_include_directories(runtime_env PUBLIC ${LOGGER_HEADER_DIR})
    target_link_libraries(runtime_env PUBLIC ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(runtime_env PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(runtime_env PUBLIC ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(runtime_env PUBLIC ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(runtime_env PRIVATE USE_STRING_UTILS)
    target_include_directories(runtime_env PUBLIC ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(runtime_env PUBLIC ${STRING_UTILS_TARGET})
endif ()
