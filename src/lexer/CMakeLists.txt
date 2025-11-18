# src/lexer/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(lexer LANGUAGES C)

find_package(FLEX REQUIRED)

set(
    FLEX_L
    "${CMAKE_CURRENT_SOURCE_DIR}/flex/lexer.l"
)

set(
    GENERATED_LEXER_SOURCE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/src"
)

file(MAKE_DIRECTORY ${GENERATED_LEXER_SOURCE_DIR})

set(
    LEXER_INCLUDE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

file(MAKE_DIRECTORY ${LEXER_INCLUDE_DIR})
add_custom_target(move_generated_lexer_header ALL
    COMMAND ${CMAKE_COMMAND} -E rename
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.h
    ${LEXER_INCLUDE_DIR}/lexer.yy.h
    DEPENDS lexer
    COMMENT "Move \"generated lexer header \
${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.h to \
${LEXER_INCLUDE_DIR}/lexer.yy.h\""
)

set(
    FLEX_OUTPUT
    "${GENERATED_LEXER_SOURCE_DIR}/lexer.yy.c"
)

set(
    PARSER_INCLUDE_DIR
    "${CMAKE_BINARY_DIR}/src/parser/include"
)

FLEX_TARGET(lexer ${FLEX_L} ${FLEX_OUTPUT})

add_library(lexer STATIC ${FLEX_lexer_OUTPUTS})
target_link_libraries(lexer PRIVATE osal input_provider)

target_include_directories(lexer
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/include"
    "${PARSER_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/frontend/input_provider/include"
)

add_dependencies(lexer parser) # lexer depends on the Bison-generated header

#[[
if (BUILD_TESTING)
    target_compile_definitions(lexer PRIVATE UNIT_TEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
endif ()
]]
