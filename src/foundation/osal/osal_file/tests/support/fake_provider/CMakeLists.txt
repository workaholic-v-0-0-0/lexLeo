# src/foundation/osal/osal_file/tests/support/fake_provider/CMakeLists.txt

add_library(
    osal_file_fake_provider
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/osal_file_fake_provider.c"
)

target_include_directories(
    osal_file_fake_provider
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    osal_file_fake_provider
    PUBLIC
        fake_memory
        fake_file
        osal_mem
        osal_file
)
