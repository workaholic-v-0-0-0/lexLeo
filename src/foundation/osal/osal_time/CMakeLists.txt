add_library(
    osal_time
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/osal_time.c"
)

target_include_directories(
    osal_time
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/internal"
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    osal_time
    PUBLIC
        lexleo_policy_headers
)

add_subdirectory(tests)
