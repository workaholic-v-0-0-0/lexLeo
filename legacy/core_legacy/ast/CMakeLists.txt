# src/core/ast/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(ast LANGUAGES C)

add_library(
    ast
    STATIC
    src/ast.c
    src/internal/ast_test_utils.c
)
target_include_directories(
    ast
    PUBLIC include
)
target_link_libraries(ast PUBLIC data_structures)
target_compile_definitions(ast PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(ast PRIVATE USE_LOGGER)
    target_include_directories(ast PUBLIC ${LOGGER_HEADER_DIR})
    target_link_libraries(ast PUBLIC ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(ast PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(ast PUBLIC ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(ast PUBLIC ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(ast PRIVATE USE_STRING_UTILS)
    target_include_directories(ast PUBLIC ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(ast PUBLIC ${STRING_UTILS_TARGET})
endif ()

if (BUILD_TESTING)
    target_compile_definitions(ast PRIVATE UNIT_TEST)
    include(tests/tests.cmake)
endif ()
