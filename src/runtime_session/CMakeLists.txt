# src/runtime_session/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)

add_library(
    runtime_session
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/runtime_session.c
)

if (BUILD_TESTING)
    target_compile_definitions(runtime_session PRIVATE UNIT_TEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()

target_include_directories(
    runtime_session
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    PUBLIC "${CMAKE_SOURCE_DIR}/src/data_structures/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/frontend/input_provider/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/symtab/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/ast/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/frontend/input_provider/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/runtime_env/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/src/lexer/include/"
    PRIVATE "${CMAKE_SOURCE_DIR}/include/"
)

target_link_libraries(
    runtime_session
    PRIVATE lexer
    PRIVATE symtab
    PRIVATE ast
    PRIVATE runtime_env
    PRIVATE input_provider
    #PRIVATE $<$<BOOL:${BUILD_TESTING}>:extra_for_tests>
)

target_compile_definitions(runtime_session PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# optional logger injection "at cmake scope"
if (USE_LOGGER)
    target_compile_definitions(runtime_session PRIVATE USE_LOGGER)
    target_include_directories(runtime_session PRIVATE ${LOGGER_HEADER_DIR})
    target_link_libraries(runtime_session PRIVATE ${LOGGER_TARGET})
endif ()

# optional memory_allocator injection "at cmake scope" ;
# if not set malloc and free for memory allocators
if (USE_MEMORY_ALLOCATOR)
    target_compile_definitions(runtime_session PRIVATE USE_MEMORY_ALLOCATOR)
    target_include_directories(runtime_session PRIVATE ${MEMORY_ALLOCATOR_HEADER_DIR})
    target_link_libraries(runtime_session PRIVATE ${MEMORY_ALLOCATOR_TARGET})
endif ()

# optional string_utils injection "at cmake scope" ;
# if no string comparison function is set, strcmp will be used for string comparison
if (USE_STRING_UTILS)
    target_compile_definitions(runtime_session PRIVATE USE_STRING_UTILS)
    target_include_directories(runtime_session PRIVATE ${STRING_UTILS_HEADER_DIR})
    target_link_libraries(runtime_session PRIVATE ${STRING_UTILS_TARGET})
endif ()
