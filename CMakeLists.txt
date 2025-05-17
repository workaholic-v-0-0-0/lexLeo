# CMakeLists.txt

cmake_minimum_required(VERSION 3.27)
project(lexLeo LANGUAGES C)

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

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    add_executable(lexLeo_cli src/cli/src/main.c)
endif ()

# define macro usefull in sources
target_compile_definitions(lexLeo_cli
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>
)
target_compile_options(lexLeo_cli
    PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang>>:-g>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-O3>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Zi>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2>
)

# portability
add_subdirectory(src/osal)
target_link_libraries(lexLeo_cli PRIVATE osal)
