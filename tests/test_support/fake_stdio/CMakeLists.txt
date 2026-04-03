add_library(
    fake_stdio
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_stdio.c"
)

target_include_directories(
    fake_stdio
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_stdio
    PUBLIC
        osal_stdio_api
        lexleo_policy_headers
    PRIVATE
        cmocka
)
