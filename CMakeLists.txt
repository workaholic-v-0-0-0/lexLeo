cmake_minimum_required(VERSION 3.22)
project(lexLeo)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)
set(OBJ_DIR ${CMAKE_SOURCE_DIR}/obj)

include_directories(${INC_DIR})

set(LEX_SRC ${SRC_DIR}/parser/lexer.l)
set(YACC_SRC ${SRC_DIR}/parser/parser.y)

find_package(BISON)
find_package(FLEX)

BISON_TARGET(Parser ${YACC_SRC} ${SRC_DIR}/parser/parser.tab.c
        DEFINES_FILE ${INC_DIR}/parser/parser.tab.h)

# adjust the include path in generated Bison C file
add_custom_target(PostProcessParser DEPENDS ${BISON_Parser_OUTPUTS})
add_custom_command(
        TARGET PostProcessParser POST_BUILD
        COMMAND sed -i 's|\#include \"parser.tab.h\"|\#include \"parser/parser.tab.h\"|' ${SRC_DIR}/parser/parser.tab.c
        COMMENT "Adjusting include path in parser.tab.c"
)

FLEX_TARGET(Lexer ${CMAKE_SOURCE_DIR}/src/parser/lexer.l ${CMAKE_SOURCE_DIR}/src/parser/lexer.yy.c)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.c)
add_executable(${PROJECT_NAME} ${SOURCES} ${BISON_Parser_OUTPUTS} ${FLEX_Lexer_OUTPUTS})
add_dependencies(${PROJECT_NAME} PostProcessParser)

add_custom_target(clean-custom
        COMMAND ${CMAKE_COMMAND} -E echo "Removing additional generated files..."
        COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/include/parser/parser.tab.h"
        COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/src/parser/parser.tab.c"
        COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/src/parser/lexer.yy.c"
        COMMENT "Running custom clean commands..."
)
