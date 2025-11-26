# CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(lexLeo LANGUAGES C)


#------------------------------------------------------------------------------
# BUILD TYPE MANAGEMENT
#------------------------------------------------------------------------------

message(STATUS "CMake generator: ${CMAKE_GENERATOR}")
# Detect if we are using a multi-config generator (Visual Studio, Xcode, etc.)
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (isMultiConfig)
    message(STATUS [=[
Multi-config generator detected (e.g., Visual Studio, Xcode).
The build type is not selected during CMake configuration.
By default, most IDEs will build the "Debug" configuration.
You can select which configuration to build
at the time you invoke the build, either in your IDE
or on the command line using the --config option
(e.g., cmake --build . --config Release).
Then CMAKE_BUILD_TYPE is not set yet.
        ]=]
    )
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
    message(STATUS "CMAKE_CONFIGURATION_TYPES: ${CMAKE_CONFIGURATION_TYPES}") #debug
else ()
    message(STATUS "Single-config generator detected (e.g., Makefiles, Ninja).")
    # Default to Debug build if nothing is specified
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    endif ()
    # Set allowed build types for cache editor GUIs or tools
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
    message(STATUS "CMAKE_BUILD_TYPE is set to ${CMAKE_BUILD_TYPE}") #debug
endif ()


#------------------------------------------------------------------------------
# TESTS MANAGEMENT
#------------------------------------------------------------------------------

if (BUILD_TESTING)

    # The include(CTest) command loads the CTest module from CMake's standard
    # module directory. This module provides functions to manage and run tests
    # as part of the build process. CTest manages tests case registration and
    # execution, integrating seamlessly with CMake to enable commands like
    # add_test(). It searches for the CTest.cmake script in the default CMake
    # Modules directory, pre-packaged with the CMake installation. This setup
    # facilitates creating, managing, and running tests across different
    # platforms and configurations, ensuring consistent tests behavior.
    include(CTest)

    # Enable the testing functionality so CMake can prepare a tests driver
    # This command must be called in the root CMakeLists.txt to enable integration
    # of tests cases defined in any subdirectory of the project.
    enable_testing()

    # Add the 'tests' subdirectory which contains only a CMakeLists.txt.
    # It configures the Check and CMocka testing libraries for unit and mock
    # testing.
    # Then it defines targets to check all tests with verbose output and
    # to run specific groups of tests using labels.
    add_subdirectory(tests)

endif ()


#------------------------------------------------------------------------------
# PORTABILITY MANAGEMENT
#------------------------------------------------------------------------------

add_subdirectory(src/osal)


#------------------------------------------------------------------------------
# LOGGER MANAGEMENT
#------------------------------------------------------------------------------

add_subdirectory(src/logger)

# configure CACHE VARIABLES for logger dependency injection into sub-projects:
set(
    USE_LOGGER ON
    CACHE BOOL
    "Enable logger support in brother subprojects"
)
set(
    LOGGER_TARGET logger
    CACHE STRING
    "CMake target to use for logger"
)
set(
    LOGGER_HEADER_DIR ${CMAKE_SOURCE_DIR}/src/logger/include
    CACHE PATH "Path to logger header directory"
)
# logger library dependency injection ("at cmake scope") will be automatically
# done by convention in other subprojects that uses the mechanism of the
# following kind:
#
# if (USE_LOGGER)
#     target_compile_definitions(list PRIVATE USE_LOGGER)
#     target_include_directories(list PUBLIC ${LOGGER_HEADER_DIR})
#     target_link_libraries(list PUBLIC ${LOGGER_TARGET})
# endif()
#
# and in C sources:
# #ifdef USE_LOGGER
# init_logger("log_file_name")
# log_info("blabla")
# log_error("bad blabla")
# close_logger()
# #endif


#------------------------------------------------------------------------------
# MEMORY ALLOCATOR MANAGEMENT
#------------------------------------------------------------------------------

add_subdirectory(src/memory_allocator)

set(
    USE_MEMORY_ALLOCATOR ON
    CACHE BOOL
    "Enable custom memory allocator support in subprojects"
)
set(
    MEMORY_ALLOCATOR_TARGET memory_allocator
    CACHE STRING
    "CMake target to use for memory allocator"
)
set(
    MEMORY_ALLOCATOR_HEADER_DIR ${CMAKE_SOURCE_DIR}/src/memory_allocator/include
    CACHE PATH "Path to memory allocator header directory"
)

# memory_allocator library dependency injection ("at cmake scope") will be automatically
# done by convention in other subprojects that uses the mechanism of the
# following kind:
#
# if (USE_MEMORY_ALLOCATOR)
#     target_compile_definitions(mymodule PRIVATE USE_MEMORY_ALLOCATOR)
#     target_include_directories(mymodule PUBLIC ${MEMORY_ALLOCATOR_HEADER_DIR})
#     target_link_libraries(mymodule PUBLIC ${MEMORY_ALLOCATOR_TARGET})
# endif()
#
# and in C sources:
# #ifdef USE_MEMORY_ALLOCATOR
# set_allocators(mock_malloc, mock_free); // example for tests
# #endif


#------------------------------------------------------------------------------
# STRING UTILS MANAGEMENT
#------------------------------------------------------------------------------

add_subdirectory(src/string_utils)

set(
    USE_STRING_UTILS ON
    CACHE BOOL
    "Enable custom string utils support in subprojects"
)
set(
    STRING_UTILS_TARGET string_utils
    CACHE STRING
    "CMake target to use for string  utils"
)
set(
    STRING_UTILS_HEADER_DIR ${CMAKE_SOURCE_DIR}/src/string_utils/include
    CACHE PATH "Path to string utils header directory"
)

# string_utils library dependency injection ("at cmake scope") will be automatically
# done by convention in other subprojects that uses the mechanism of the
# following kind:
#
# if (USE_STRING_UTILS)
#     target_compile_definitions(data_structures PRIVATE USE_STRING_UTILS)
#     target_include_directories(data_structures PUBLIC ${USE_STRING_UTILS_HEADER_DIR})
#     target_link_libraries(data_structures PUBLIC ${USE_STRING_UTILS_TARGET})
# endif ()
#
# and in C sources:
# #ifdef USE_STRING_UTILS
# set_string_compare(strcmp_mock); // example for tests
# #endif


#------------------------------------------------------------------------------
# OTHER LIBRARIES
#------------------------------------------------------------------------------

add_subdirectory(src/data_structures)
add_subdirectory(src/ast) # depends on data_structures
add_subdirectory(src/symtab) # depends on data_structures and ast
add_subdirectory(src/parser) # depends on lexer and ast
add_subdirectory(src/lexer) # depends on parser interface—opaque types
add_subdirectory(src/resolver) # depends on ast and symtab
add_subdirectory(src/runtime_env) # depends on hashtable
add_subdirectory(src/frontend/input_provider) # depends on lexer
add_subdirectory(src/frontend/ports/stream)
add_subdirectory(src/adapters/stream/stdio_stream) # depends on stream
add_subdirectory(src/adapters/stream/fs_stream) # depends on stream
add_subdirectory(src/adapters/stream/dynamic_buffer_stream) # depends on stream
add_subdirectory(src/runtime_session) # depends on ...?
add_subdirectory(src/interpreter) # depends on runtime_env, ...?

# TO BE IMPLEMENTED
# add_subdirectory(src/compiler)


#------------------------------------------------------------------------------
# CLI
#------------------------------------------------------------------------------

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    add_executable(lexLeo_cli src/cli/src/main.c)
endif ()
target_link_libraries(lexLeo_cli
    PRIVATE osal
    # PRIVATE logger
)


#------------------------------------------------------------------------------
# COMPILE OPTIONS & DEFINITIONS
#------------------------------------------------------------------------------

function(enable_DEBUG_and_RELEASE_definitions target)
    target_compile_definitions(${target}
        PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:RELEASE>
    )
endfunction()

enable_DEBUG_and_RELEASE_definitions(lexLeo_cli)
enable_DEBUG_and_RELEASE_definitions(data_structures)

# Central build options for all project targets
add_library(project_build_options INTERFACE)
target_compile_options(project_build_options INTERFACE
    $<$<AND:$<CONFIG:Debug>,$<C_COMPILER_ID:GNU,Clang>>:-g>
    $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:GNU,Clang>>:-O3>
    $<$<AND:$<CONFIG:Debug>,$<C_COMPILER_ID:MSVC>>:/Zi>
    $<$<AND:$<CONFIG:Release>,$<C_COMPILER_ID:MSVC>>:/O2>
)

target_link_libraries(lexLeo_cli PRIVATE project_build_options)
target_link_libraries(data_structures PRIVATE project_build_options)


#------------------------------------------------------------------------------
# TOOLS MANAGEMENT
#------------------------------------------------------------------------------

add_subdirectory(tools)


#------------------------------------------------------------------------------
# DOCUMENTATION MANAGEMENT
#------------------------------------------------------------------------------

add_subdirectory(doxygen_management)
