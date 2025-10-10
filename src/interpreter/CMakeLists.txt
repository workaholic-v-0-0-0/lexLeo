# src/interpreter/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(runtime_env LANGUAGES C)

if (BUILD_TESTING)
    add_library(
        interpreter
        STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/interpreter.c
    )
    target_compile_definitions(interpreter PRIVATE UNIT_TEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
else ()
    add_library(
        interpreter
        STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/interpreter.c
    )
endif ()

target_include_directories(
    interpreter
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/runtime_env/include
    #PRIVATE ${CMAKE_SOURCE_DIR}/src/ast/include
)
target_link_libraries(interpreter PUBLIC ast runtime_env)
target_compile_definitions(interpreter PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(interpreter PRIVATE USE_LOGGER)
    target_include_directories(interpreter PUBLIC ${LOGGER_HEADER_DIR})
    target_link_libraries(interpreter PUBLIC ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(interpreter PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(interpreter PUBLIC ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(interpreter PUBLIC ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(interpreter PRIVATE USE_STRING_UTILS)
    target_include_directories(interpreter PUBLIC ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(interpreter PUBLIC ${STRING_UTILS_TARGET})
endif ()
