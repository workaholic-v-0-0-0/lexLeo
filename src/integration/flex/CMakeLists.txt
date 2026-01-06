# src/integration/flex/CMakeLists.txt

find_package(FLEX REQUIRED)

set(
    FLEX_L
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l"
)

set(
    FLEX_SRC_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/src"
)

set(
    FLEX_INC_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

file(MAKE_DIRECTORY "${FLEX_SRC_DIR}")
file(MAKE_DIRECTORY "${FLEX_INC_DIR}")

set(FLEX_C "${FLEX_SRC_DIR}/lexer.yy.c")
set(FLEX_H "${FLEX_INC_DIR}/lexer.yy.h")

# Generate lexer.yy.c and lexer.yy.h
FLEX_TARGET(
    lexleo_flex_scanner
    "${FLEX_L}"
    "${FLEX_C}"
    COMPILE_FLAGS
    "--reentrant --header-file=lexer.yy.h"
)

add_library(
    lexleo_integration_flex
    STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/flex_backend.c"
    ${FLEX_lexleo_flex_scanner_OUTPUTS}
)

target_include_directories(lexleo_integration_flex
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${FLEX_INC_DIR}"
    "${FLEX_SRC_DIR}"
    "${CMAKE_SOURCE_DIR}/src/include"
    "${CMAKE_SOURCE_DIR}/src/frontend/input_provider/include"
    "${CMAKE_SOURCE_DIR}/src/osal/include"
    "${CMAKE_SOURCE_DIR}"

)
