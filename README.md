\mainpage Main page

# lexLeo

Welcome to the lexLeo programming language project! This repository contains the source code for lexLeo's compiler and interpreter, designed to explore and implement the capabilities of this new programming language.

## Project Overview

This project is primarily an educational initiative designed to deepen understanding and practical application of fundamental programming language concepts, as well as the principles of compilers and interpreters. It aims to provide an interactive learning platform where participants can explore the intricacies of language construction, understand the stages of compilation, and master the process of interpretation.

The framework of this project allows students and computer enthusiasts to engage in a structured environment that fosters skill development in language design, compiler construction, and the creation of efficient interpreters.

This project includes:

- **lexLeo Compiler**: Translates lexLeo code into machine code.
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

## Building the Project with CMake

To build the compiler and interpreter using CMake, follow these steps:

### For Linux:
   ```bash
   cmake -B build
   cmake --build build
   ```
### For Windows (with x64 architecture and the Visual Studio 2022 17 generator):
   ```bash
   cmake -B build -G "Visual Studio 2022 17" -A x64
   cmake --build build --config Release
   ```
### For MacOS (using the default “Unix Makefiles” generator):
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

## Running the Project

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

## Contributing

Contributions to the Leo project are welcome! Whether it involves 
fixing bugs, adding new features, or improving documentation, your 
help is appreciated. Please read CONTRIBUTING.md for details on our 
code of conduct and the process for submitting pull requests to us.

## License

This project is licensed under the GPLv3 - see the LICENSE.md file for details.