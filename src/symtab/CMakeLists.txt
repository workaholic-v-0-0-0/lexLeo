# src/symtab/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(symtab LANGUAGES C)

add_library(
    symtab
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/symtab.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/internal/symtab_test_utils.c
)

target_include_directories(
    symtab
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/ast/include
)
target_link_libraries(symtab PUBLIC data_structures)
target_compile_definitions(symtab PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(symtab PRIVATE USE_LOGGER)
    target_include_directories(symtab PUBLIC ${LOGGER_HEADER_DIR})
    target_link_libraries(symtab PUBLIC ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(symtab PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(symtab PUBLIC ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(symtab PUBLIC ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(symtab PRIVATE USE_STRING_UTILS)
    target_include_directories(symtab PUBLIC ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(symtab PUBLIC ${STRING_UTILS_TARGET})
endif ()

if (BUILD_TESTING)
    target_compile_definitions(symtab PRIVATE UNIT_TEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()
