# src/foundation/logger/CMakeLists.txt

add_library(logger_legacy src/logger.c)
target_include_directories(logger_legacy PUBLIC include)
target_compile_definitions(logger_legacy PRIVATE $<$<CONFIG:Debug>:DEBUG>)

#[[ legacy vestige
if(BUILD_TESTING)
    include(tests/tests.cmake)
endif()
#]]

add_subdirectory(port)
add_subdirectory(adapters)
