# CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(lexLeo LANGUAGES C)


#------------------------------------------------------------------------------
# BUILD TYPE MANAGEMENT
#------------------------------------------------------------------------------

message(STATUS "CMake generator: ${CMAKE_GENERATOR}")
# Detect if we are using a multi-config generator (Visual Studio, Xcode, etc.)
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(isMultiConfig)
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
else()
    message(STATUS "Single-config generator detected (e.g., Makefiles, Ninja).")
    # Default to Debug build if nothing is specified
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    endif()
    # Set allowed build types for cache editor GUIs or tools
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
    message(STATUS "CMAKE_BUILD_TYPE is set to ${CMAKE_BUILD_TYPE}") #debug
endif()



#------------------------------------------------------------------------------
# TESTS MANAGEMENT
#------------------------------------------------------------------------------

# The include(CTest) command loads the CTest module from CMake's standard
# module directory. This module provides functions to manage and run tests
# as part of the build process. CTest manages test case registration and
# execution, integrating seamlessly with CMake to enable commands like
# add_test(). It searches for the CTest.cmake script in the default CMake
# Modules directory, pre-packaged with the CMake installation. This setup
# facilitates creating, managing, and running tests across different
# platforms and configurations, ensuring consistent test behavior.
include(CTest)

# Enable the testing functionality so CMake can prepare a test driver
# This command must be called in the root tests.cmake to enable integration
# of test cases defined in any subdirectory of the project.
enable_testing()

# Add the 'tests' subdirectory which contains only a tests.cmake.
# It configures the Check and CMocka testing libraries for unit and mock
# testing.
# Then it defines targets to check all tests with verbose output and
# to run specific groups of tests using labels.
include(tests/tests.cmake)


#------------------------------------------------------------------------------
# PORTABILITY MANAGEMENT
#------------------------------------------------------------------------------

add_subdirectory(src/osal)


#------------------------------------------------------------------------------
# LOGGER MANAGEMENT
#------------------------------------------------------------------------------

add_subdirectory(src/logger)


#------------------------------------------------------------------------------
# OTHER LIBRARIES
#------------------------------------------------------------------------------

# TO BE IMPLEMENTED
# add_subdirectory(src/data_structures)
# add_subdirectory(src/symtab)
# add_subdirectory(src/ast)
# add_subdirectory(src/parser)
# add_subdirectory(src/lexer)
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
# CLI COMPILE OPTIONS & DEFINITIONS
#------------------------------------------------------------------------------

target_compile_definitions(lexLeo_cli
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
)
target_compile_options(lexLeo_cli
    PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang>>:-g>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-O3>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Zi>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2>
)
