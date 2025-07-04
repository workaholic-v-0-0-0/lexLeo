# src/parser/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(parser LANGUAGES C)

find_package(BISON REQUIRED)

set(
    PARSER_Y
    "${CMAKE_CURRENT_SOURCE_DIR}/bison/parser.y"
)

set(
    GENERATED_PARSER_SOURCE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/src"
)

set(
    GENERATED_PARSER_INCLUDE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/include/"
)

set(
    PARSER_SRC
    "${GENERATED_PARSER_SOURCE_DIR}/parser.tab.c"
)

set(
    PARSER_HEADER
    "${GENERATED_PARSER_INCLUDE_DIR}/parser.tab.h"
)

file(MAKE_DIRECTORY ${GENERATED_PARSER_SOURCE_DIR})
file(MAKE_DIRECTORY ${GENERATED_PARSER_INCLUDE_DIR})

BISON_TARGET(parser ${PARSER_Y} ${PARSER_SRC} DEFINES_FILE ${PARSER_HEADER})
add_library(parser STATIC ${BISON_parser_OUTPUTS})
target_include_directories(
    parser
    PUBLIC
    "${CMAKE_SOURCE_DIR}/src/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src/ast/include"
    "${CMAKE_SOURCE_DIR}/src/symtab/include"
)
target_link_libraries(parser PRIVATE lexer ast symtab)
