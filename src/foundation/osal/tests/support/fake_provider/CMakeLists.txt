# src/foundation/osal/tests/support/fake_provider/CMakeLists.txt

add_library(
    osal_mem_fake_provider
    STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/osal_mem_fake_provider.c"
)

target_link_libraries(
    osal_mem_fake_provider
    PUBLIC
    fake_memory
)

target_include_directories(
    osal_mem_fake_provider
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src/foundation/osal/include"
)

