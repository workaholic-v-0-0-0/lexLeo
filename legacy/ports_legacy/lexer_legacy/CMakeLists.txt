# src/ports/lexer_legacy/CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(lexer_legacy LANGUAGES C)

find_package(FLEX REQUIRED)

set(
    FLEX_L
    "flex/lexer_legacy.l"
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
add_custom_target(move_generated_lexer_legacy_header ALL
    COMMAND ${CMAKE_COMMAND} -E rename
    ${CMAKE_CURRENT_BINARY_DIR}/lexer_legacy.yy.h
    ${LEXER_INCLUDE_DIR}/lexer_legacy.yy.h
    DEPENDS lexer_legacy
    COMMENT "Move \"generated lexer header \
${CMAKE_CURRENT_BINARY_DIR}/lexer_legacy.yy.h to \
${LEXER_INCLUDE_DIR}/lexer_legacy.yy.h\""
)

set(
    FLEX_OUTPUT
    "${GENERATED_LEXER_SOURCE_DIR}/lexer.yy.c"
)

set(
    PARSER_INCLUDE_DIR
    "${CMAKE_BINARY_DIR}/src/ports/parser/include"
)

FLEX_TARGET(lexer_legacy ${FLEX_L} ${FLEX_OUTPUT})

add_library(lexer_legacy STATIC ${FLEX_lexer_legacy_OUTPUTS})
target_link_libraries(lexer_legacy PRIVATE osal input_provider_legacy)

target_include_directories(lexer_legacy
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/policy/include"
    "${CMAKE_SOURCE_DIR}/src/langage/include"
    "${PARSER_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/foundation/input_provider_legacy/include"
)

add_dependencies(lexer_legacy parser) # lexer depends on the Bison-generated header

#[[
if (BUILD_TESTING)
    target_compile_definitions(lexer PRIVATE UNIT_TEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
endif ()
]]
