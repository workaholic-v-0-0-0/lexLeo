# src/parser/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(parser LANGUAGES C)

find_package(BISON REQUIRED)
set(YACC_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y")
set(YACC_OUT "${CMAKE_CURRENT_SOURCE_DIR}/src/parser.tab.c")
set(PARSER_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/parser.tab.h")
BISON_TARGET(parser_src ${YACC_SRC} ${YACC_OUT}
    DEFINES_FILE ${PARSER_HEADER})
add_custom_target(parser_files DEPENDS ${YACC_OUT} ${PARSER_HEADER})

add_library(parser STATIC ${YACC_OUT} ${PARSER_HEADER})

