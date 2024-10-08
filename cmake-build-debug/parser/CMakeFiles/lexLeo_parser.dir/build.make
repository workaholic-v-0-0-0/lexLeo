# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/workaholic/env/clion/workspace/lexLeo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug

# Include any dependencies generated for this target.
include parser/CMakeFiles/lexLeo_parser.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include parser/CMakeFiles/lexLeo_parser.dir/compiler_depend.make

# Include the progress variables for this target.
include parser/CMakeFiles/lexLeo_parser.dir/progress.make

# Include the compile flags for this target's objects.
include parser/CMakeFiles/lexLeo_parser.dir/flags.make

/home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.tab.c: /home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.y
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "[BISON][parser_source] Building parser with bison 3.8.2"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/parser && /usr/bin/bison --defines=/home/workaholic/env/clion/workspace/lexLeo/parser/include/parser/parser.tab.h -o /home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.tab.c /home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.y

/home/workaholic/env/clion/workspace/lexLeo/parser/include/parser/parser.tab.h: /home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.tab.c
	@$(CMAKE_COMMAND) -E touch_nocreate /home/workaholic/env/clion/workspace/lexLeo/parser/include/parser/parser.tab.h

parser/CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.o: parser/CMakeFiles/lexLeo_parser.dir/flags.make
parser/CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.o: /home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.tab.c
parser/CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.o: parser/CMakeFiles/lexLeo_parser.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object parser/CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.o"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/parser && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT parser/CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.o -MF CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.o.d -o CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.o -c /home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.tab.c

parser/CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.i"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/parser && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.tab.c > CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.i

parser/CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.s"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/parser && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.tab.c -o CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.s

# Object files for target lexLeo_parser
lexLeo_parser_OBJECTS = \
"CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.o"

# External object files for target lexLeo_parser
lexLeo_parser_EXTERNAL_OBJECTS =

parser/liblexLeo_parser.a: parser/CMakeFiles/lexLeo_parser.dir/src/parser.tab.c.o
parser/liblexLeo_parser.a: parser/CMakeFiles/lexLeo_parser.dir/build.make
parser/liblexLeo_parser.a: parser/CMakeFiles/lexLeo_parser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library liblexLeo_parser.a"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/parser && $(CMAKE_COMMAND) -P CMakeFiles/lexLeo_parser.dir/cmake_clean_target.cmake
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/parser && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lexLeo_parser.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
parser/CMakeFiles/lexLeo_parser.dir/build: parser/liblexLeo_parser.a
.PHONY : parser/CMakeFiles/lexLeo_parser.dir/build

parser/CMakeFiles/lexLeo_parser.dir/clean:
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/parser && $(CMAKE_COMMAND) -P CMakeFiles/lexLeo_parser.dir/cmake_clean.cmake
.PHONY : parser/CMakeFiles/lexLeo_parser.dir/clean

parser/CMakeFiles/lexLeo_parser.dir/depend: /home/workaholic/env/clion/workspace/lexLeo/parser/include/parser/parser.tab.h
parser/CMakeFiles/lexLeo_parser.dir/depend: /home/workaholic/env/clion/workspace/lexLeo/parser/src/parser.tab.c
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/workaholic/env/clion/workspace/lexLeo /home/workaholic/env/clion/workspace/lexLeo/parser /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/parser /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/parser/CMakeFiles/lexLeo_parser.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : parser/CMakeFiles/lexLeo_parser.dir/depend

