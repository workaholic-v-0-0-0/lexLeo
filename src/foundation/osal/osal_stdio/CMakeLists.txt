add_library(
    osal_stdio
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/osal_stdio.c"
)

target_include_directories(
    osal_stdio
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    osal_stdio
    PUBLIC
        lexleo_policy_headers
)

add_library(
    osal_stdio_api
    INTERFACE
)

target_include_directories(
    osal_stdio_api
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()
