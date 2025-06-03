# src/data_structures/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(data_structures LANGUAGES C)

add_library(
    data_structures
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hashtable.c
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

if (BUILD_TESTING)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cmake)
endif ()
