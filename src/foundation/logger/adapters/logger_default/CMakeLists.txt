add_library(
    logger_default
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger_default.c"
)

target_include_directories(
    logger_default
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    logger_default
    PRIVATE
        logger
        stream
        osal_mem
        lexleo_assert
)

add_subdirectory(tests)
