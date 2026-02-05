# src/core/lexer/port/CMakeLists.txt

add_library(lexer STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.c"
)

target_include_directories(
    lexer
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    lexer
    PRIVATE
        osal_mem_legacy
)
