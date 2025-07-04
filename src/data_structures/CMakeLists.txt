# src/data_structures/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(data_structures LANGUAGES C)

add_library(
    data_structures
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hashtable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/internal/hashtable_test_utils.c
)
target_include_directories(
    data_structures
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_definitions(
    data_structures
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(data_structures PRIVATE USE_LOGGER)
    target_include_directories(data_structures PUBLIC ${LOGGER_HEADER_DIR})
    target_link_libraries(data_structures PUBLIC ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(data_structures PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(data_structures PUBLIC ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(data_structures PUBLIC ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(data_structures PRIVATE USE_STRING_UTILS)
    target_include_directories(data_structures PUBLIC ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(data_structures PUBLIC ${STRING_UTILS_TARGET})
endif ()

if (BUILD_TESTING)
    target_compile_definitions(data_structures PRIVATE UNIT_TEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()
