project(lexLeo_logger)

add_library(${PROJECT_NAME} src/logger.c)
add_library(lexLeo::logger ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
)

#target_link_libraries(${PROJECT_NAME} PUBLIC lexLeo::lexer)
