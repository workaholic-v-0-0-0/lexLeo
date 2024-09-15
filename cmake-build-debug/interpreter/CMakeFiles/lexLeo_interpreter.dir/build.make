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
include interpreter/CMakeFiles/lexLeo_interpreter.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include interpreter/CMakeFiles/lexLeo_interpreter.dir/compiler_depend.make

# Include the progress variables for this target.
include interpreter/CMakeFiles/lexLeo_interpreter.dir/progress.make

# Include the compile flags for this target's objects.
include interpreter/CMakeFiles/lexLeo_interpreter.dir/flags.make

interpreter/CMakeFiles/lexLeo_interpreter.dir/main.c.o: interpreter/CMakeFiles/lexLeo_interpreter.dir/flags.make
interpreter/CMakeFiles/lexLeo_interpreter.dir/main.c.o: /home/workaholic/env/clion/workspace/lexLeo/interpreter/main.c
interpreter/CMakeFiles/lexLeo_interpreter.dir/main.c.o: interpreter/CMakeFiles/lexLeo_interpreter.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object interpreter/CMakeFiles/lexLeo_interpreter.dir/main.c.o"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/interpreter && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT interpreter/CMakeFiles/lexLeo_interpreter.dir/main.c.o -MF CMakeFiles/lexLeo_interpreter.dir/main.c.o.d -o CMakeFiles/lexLeo_interpreter.dir/main.c.o -c /home/workaholic/env/clion/workspace/lexLeo/interpreter/main.c

interpreter/CMakeFiles/lexLeo_interpreter.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/lexLeo_interpreter.dir/main.c.i"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/interpreter && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/workaholic/env/clion/workspace/lexLeo/interpreter/main.c > CMakeFiles/lexLeo_interpreter.dir/main.c.i

interpreter/CMakeFiles/lexLeo_interpreter.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/lexLeo_interpreter.dir/main.c.s"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/interpreter && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/workaholic/env/clion/workspace/lexLeo/interpreter/main.c -o CMakeFiles/lexLeo_interpreter.dir/main.c.s

# Object files for target lexLeo_interpreter
lexLeo_interpreter_OBJECTS = \
"CMakeFiles/lexLeo_interpreter.dir/main.c.o"

# External object files for target lexLeo_interpreter
lexLeo_interpreter_EXTERNAL_OBJECTS =

interpreter/lexLeo_interpreter: interpreter/CMakeFiles/lexLeo_interpreter.dir/main.c.o
interpreter/lexLeo_interpreter: interpreter/CMakeFiles/lexLeo_interpreter.dir/build.make
interpreter/lexLeo_interpreter: parser/liblexLeo_parser.a
interpreter/lexLeo_interpreter: lexer/liblexLeo_lexer.a
interpreter/lexLeo_interpreter: data_structures/liblexLeo_data_structures.a
interpreter/lexLeo_interpreter: logger/liblexLeo_logger.a
interpreter/lexLeo_interpreter: interpreter/CMakeFiles/lexLeo_interpreter.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable lexLeo_interpreter"
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/interpreter && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lexLeo_interpreter.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
interpreter/CMakeFiles/lexLeo_interpreter.dir/build: interpreter/lexLeo_interpreter
.PHONY : interpreter/CMakeFiles/lexLeo_interpreter.dir/build

interpreter/CMakeFiles/lexLeo_interpreter.dir/clean:
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/interpreter && $(CMAKE_COMMAND) -P CMakeFiles/lexLeo_interpreter.dir/cmake_clean.cmake
.PHONY : interpreter/CMakeFiles/lexLeo_interpreter.dir/clean

interpreter/CMakeFiles/lexLeo_interpreter.dir/depend:
	cd /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/workaholic/env/clion/workspace/lexLeo /home/workaholic/env/clion/workspace/lexLeo/interpreter /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/interpreter /home/workaholic/env/clion/workspace/lexLeo/cmake-build-debug/interpreter/CMakeFiles/lexLeo_interpreter.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : interpreter/CMakeFiles/lexLeo_interpreter.dir/depend

