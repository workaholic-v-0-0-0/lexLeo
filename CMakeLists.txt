cmake_minimum_required(VERSION 3.22)
project(lexLeo LANGUAGES C)

# compiler settings
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -g)
add_definitions(-D_GNU_SOURCE)

# usefull cause lexer's source includes the parser's header
set(PARSER_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/parser/include"
        CACHE PATH "Path to the parser include directory")

# tests management
include(CTest)
enable_testing()
add_subdirectory(tests)

# logger management
add_subdirectory(logger)

# library modules
add_subdirectory(data_structures)
add_subdirectory(symtab)
add_subdirectory(ast)
add_subdirectory(lexer)
add_subdirectory(parser)

# executable modules
add_subdirectory(interpreter)
add_subdirectory(compiler)

# documentation generation via Doxygen
find_package(Doxygen)
set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs)
add_custom_target(doc ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
)
