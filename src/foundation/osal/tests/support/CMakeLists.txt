add_subdirectory(fake_osal_mem)

add_library(fake_osal_test_support INTERFACE)
target_link_libraries(fake_osal_test_support INTERFACE
    fake_osal_fake_provider
)
