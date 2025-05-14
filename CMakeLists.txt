# CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(lexLeo LANGUAGES C)

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    add_executable(lexLeo_cli src/cli/src/main.c)
endif ()

# portability
add_subdirectory(src/osal)
target_link_libraries(lexLeo_cli PRIVATE osal)
