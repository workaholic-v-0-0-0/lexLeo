# bison/lexer/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(lexer LANGUAGES C)

find_package(FLEX REQUIRED)

set(
    FLEX_L
    "${CMAKE_CURRENT_SOURCE_DIR}/flex/lexer.l"
)

set(
    GENERATED_LEXER_SRC_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/src"
)

file(MAKE_DIRECTORY ${GENERATED_LEXER_SRC_DIR})

set(
    FLEX_OUTPUT
    "${GENERATED_LEXER_SRC_DIR}/lexer.yy.c"
)

set(
    PARSER_INCLUDE_DIR
    "${CMAKE_BINARY_DIR}/src/parser/include"
)

FLEX_TARGET(lexer ${FLEX_L} ${FLEX_OUTPUT})

add_library(lexer STATIC ${FLEX_lexer_OUTPUTS})

target_include_directories(lexer
    PRIVATE
    ${PARSER_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src/ast/include
)

add_dependencies(lexer parser)
