# src/foundation/osal/osal_mem/CMakeLists.txt

add_library(
    osal_mem
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/osal_mem.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/osal_mem_ops.c"
)

target_include_directories(
    osal_mem
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    osal_mem
    PRIVATE
        lexleo_policy_headers
)

add_subdirectory(tests)
