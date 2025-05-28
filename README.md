# README

## Project overview

Welcome to the lexLeo programming language project! This repository contains the source code for lexLeo's compiler and
interpreter, designed to explore and implement the capabilities of this new programming language.

This project is primarily an educational initiative designed to deepen understanding and practical application of
fundamental programming language concepts, as well as the principles of compilers and interpreters. It aims to provide
an interactive learning platform where participants can explore the intricacies of language construction, understand the
stages of compilation, and master the process of interpretation.

The framework of this project allows students and computer enthusiasts to engage in a structured environment that
fosters skill development in language design, compiler construction, and the creation of efficient interpreters.

Beyond its pedagogical focus on programming languages and compilers, lexLeo is also a pretext for applying and refining
modern software engineering practices. The project emphasizes:

- Clean and scalable software architecture
- Test-Driven Development (TDD)
- Robust build and dependency management using CMake
- Automated documentation generation with Doxygen

By adhering to these professional practices, lexLeo serves as a real-world playground for mastering not only theoretical
concepts, but also the tools, workflows, and discipline that underpin high-quality software development. The ultimate
goal is to cultivate both technical expertise and rigorous methodology, equipping contributors with skills that
translate directly into industry standards.

This project includes:

- **lexLeo Compiler**: Translates lexLeo code into an intermediate or target language.
- **lexLeo Interpreter**: Executes lexLeo code directly for rapid testing and development.

## Getting Started

To get started with lexLeo, clone this repository to your local machine:

```bash
git clone https://github.com/workaholic-v-0-0-0/lexLeo.git
cd lexLeo
```

## Prerequisites

Ensure you have the following installed on your system:

- GCC (or any preferred C compiler)
- CMake
- A generator as Make or Visual Studio 2022 17

## Build the project with CMake

To build the compiler and interpreter using CMake, follow these steps:

### For Production Builds (Tests disabled)

#### For Linux:

   ```bash
   cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
   cmake --build build
   ```

#### For Windows (with x64 architecture and the Visual Studio 2022 17 generator):

   ```bash
   cmake -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=OFF
   cmake --build build --config Release
   ```

#### For MacOS (using the default “Unix Makefiles” generator):

   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
   cmake --build build
   ```

### For Development/Testing Builds (Tests enabled)

By default, BUILD_TESTING is ON, but you can specify it explicitly for clarity.

#### For Linux:

   ```bash
  rm -r build
  cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
  cmake --build build
  cmake --build build --target check
  cmake --build build --target check_memory
   ```

#### For Windows (with x64 architecture and the Visual Studio 2022 17 generator):

   ```bash
  rm -r build
  cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
  cmake --build build --config Debug
  cmake --build build --target check
  cmake --build build --target check_memory
   ```

#### For MacOS (using the default “Unix Makefiles” generator):

   ```bash
    rm -r build
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
    cmake --build build
    cmake --build build --target check
    cmake --build build --target check_memory
   ```

## Run the project

After building the project, you can run the executable directly from the project
directory:

### For Linux:

   ```bash
   ./build/lexLeo_cli
   ```

### For Windows:

   ```bash
   .\build\Release\lexLeo_cli.exe
   ```

### For MacOS:

   ```bash
   ./build/lexLeo_cli
   ```

## Rebuild, generate documentation and open it with a web browser

   ```bash
   ./scripts/re_debug_build-make_and_open_doc.sh
   ```

## Rebuild and run unit tests without memory checking

   ```bash
   ./scripts/re_debug_build-check.sh 
   ```

## Rebuild and run unit tests with memory checking

   ```bash
   ./scripts/re_debug_build-check_memory.sh
   ```

## Contribute

Contributions to the lexLeo project are welcome! Whether it involves
fixing bugs, adding new features, or improving documentation, your
help is appreciated. Please read CONTRIBUTING.md for details on our
code of conduct and the process for submitting pull requests to us.

## License

This project is licensed under the GPLv3 - see the LICENSE file for details.
