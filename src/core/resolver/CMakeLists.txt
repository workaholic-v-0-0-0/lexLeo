# src/core/resolver/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(resolver LANGUAGES C)

add_library(
    resolver
    STATIC
    src/resolver.c
)

target_include_directories(
    resolver
    PUBLIC include
)

target_link_libraries(resolver PUBLIC ast symtab)
target_compile_definitions(resolver PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(resolver PRIVATE USE_LOGGER)
    target_include_directories(resolver PUBLIC ${LOGGER_HEADER_DIR})
    target_link_libraries(resolver PUBLIC ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(resolver PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(resolver PUBLIC ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(resolver PUBLIC ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(resolver PRIVATE USE_STRING_UTILS)
    target_include_directories(resolver PUBLIC ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(resolver PUBLIC ${STRING_UTILS_TARGET})
endif ()

if (BUILD_TESTING)
    target_compile_definitions(resolver PRIVATE UNIT_TEST)
    include(tests/tests.cmake)
endif ()
