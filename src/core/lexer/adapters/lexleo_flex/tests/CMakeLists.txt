# src/core/lexer/adapters/lexleo_flex/tests/CMakeLists.txt

add_executable(
    unit_test_lexleo_flex
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/unit_test_lexleo_flex.c
)

target_link_libraries(
    your_test_target
    PRIVATE
    lexleo_cmocka_support_test
)

target_include_directories(
    unit_test_lexleo_flex
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_SOURCE_DIR}/tests/test_support/lexleo_cmocka_support/include
)
