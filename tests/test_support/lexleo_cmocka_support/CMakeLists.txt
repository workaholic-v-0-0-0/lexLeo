# tests/test_support/cmocka_support/CMakeLists.txt

add_library(lexleo_cmocka_support INTERFACE)

target_include_directories(
    lexleo_cmocka_support
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
