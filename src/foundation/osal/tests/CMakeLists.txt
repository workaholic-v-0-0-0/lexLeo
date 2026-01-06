# src/foundation/osal/tests/CMakeLists.txt

add_library(
    osal_mem_test_api
    STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/support/src/osal_mem_test_api.c"
)

target_link_libraries(
    osal_mem_test_api
    PUBLIC
    fake_memory
)

target_include_directories(
    osal_mem_test_api
    PUBLIC
    "${CMAKE_SOURCE_DIR}/tests/test_support/fake_memory/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/include"
    "${CMAKE_SOURCE_DIR}/src/foundation/osal/include"
)

