# tests/test_support/fake_file/CMakeLists.txt

add_library(
    fake_file
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_file.c"
)

target_include_directories(
    fake_file
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_file
    PRIVATE
        osal_file
        osal_mem
        lexleo_assert
)
