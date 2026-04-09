# src/cli/CMakeLists.txt

add_library(
    cli
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/cli.c"
)

target_include_directories(
    cli
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_executable(
    lexLeo_cli
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
)

target_link_libraries(
    lexLeo_cli
    PRIVATE
        cli
)
