

add_library(
    fake_vm
    STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/fake_vm.c"
)

target_include_directories(
    fake_vm
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    fake_vm
    PUBLIC
        lexleo_vm_cr
        lexleo_policy_headers
    PRIVATE
        lexleo_cmocka_support
)
