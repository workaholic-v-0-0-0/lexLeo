# src/ports/stream/CMakeLists.txt

add_library(stream STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stream.c"
)

target_link_libraries(stream
    PRIVATE osal
)

target_include_directories(stream
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/policy/include"
)
