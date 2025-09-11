# tests/test_support/fake_memory/CMakeLists.txt

add_library(
    fake_memory
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fake_memory.c
)

target_include_directories(
    fake_memory
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(fake_memory PRIVATE osal)

target_compile_features(fake_memory PUBLIC c_std_11)
