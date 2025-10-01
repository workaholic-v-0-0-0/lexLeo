# tests/CMakeLists.txt

#------------------------------------------------------------------------------
# CHECK LIBRARY CONFIGURATION
#------------------------------------------------------------------------------

# Ensure PkgConfig is available to manage the Check library paths and
# compilation details.
find_package(PkgConfig REQUIRED)

# Find the Check testing library and configure it for use by creating an
# imported target.
# This setup enables the inclusion of Check's functions and macros necessary
# for writing unit tests.
pkg_check_modules(CHECK REQUIRED IMPORTED_TARGET check)

# Cache the include directories for Check to make them available across the
# project.
# This is necessary to ensure that all subdirectories can access the Check
# headers without needing to reconfigure these paths locally.
set(
    CHECK_INCLUDE_DIRS
    ${CHECK_INCLUDE_DIRS}
    CACHE STRING
    "Path to Check include directories"
)


#------------------------------------------------------------------------------
# CMOCKA LIBRARY CONFIGURATION
#------------------------------------------------------------------------------

# Locate the CMocka header file (cmocka.h) and library.
find_path(CMOCKA_INCLUDE_DIR
    NAMES cmocka.h
    DOC "Where the CMocka header can be found"
)
find_library(CMOCKA_LIBRARY
    NAMES cmocka
    DOC "Where the CMocka library can be found"
)

# Include the module to handle standard find_package arguments.
include(FindPackageHandleStandardArgs)

# Verify that all required components are found and set CMocka_FOUND.
find_package_handle_standard_args(
    CMocka REQUIRED_VARS CMOCKA_INCLUDE_DIR CMOCKA_LIBRARY
)

# Stop configuration if CMocka is not found.
if (NOT CMocka_FOUND)
    message(FATAL_ERROR "CMocka required but not found! Ensure it is \
    installed correctly.")
endif ()

# Make variables accessible in submodules by caching them.
set(
    CMOCKA_INCLUDE_DIR
    "${CMOCKA_INCLUDE_DIR}"
    CACHE STRING
    "Path to the CMocka include directory"
)
set(
    CMOCKA_LIBRARY
    "${CMOCKA_LIBRARY}"
    CACHE STRING
    "Path to the CMocka library"
)


#------------------------------------------------------------------------------
# CUSTOM TARGETS FOR RUNNING TESTS
#------------------------------------------------------------------------------

# Create a custom target to run all tests with verbose output. This allows for
# detailed tests result logging during the build process.
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND}
    --verbose
    --output-on-failure
    --label-exclude "memory"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS
    test_logger
    test_list
    logger
)

add_custom_target(
    check_memory
    COMMAND ${CMAKE_CTEST_COMMAND}
    --verbose
    --label-regex "memory"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS
    test_logger
    test_list
    logger
)


#------------------------------------------------------------------------------
# TEST SUPPORT UTILITIES
#------------------------------------------------------------------------------

add_subdirectory(test_support/fake_memory)


#------------------------------------------------------------------------------
# DEV UTILITIES
#------------------------------------------------------------------------------

add_custom_target(check_quick
    COMMAND ${CMAKE_CTEST_COMMAND} -LE memory --stop-on-failure --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)