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
- Make (optional, used for certain CMake generators)

## Building the Project with CMake

To build the compiler and interpreter using CMake, follow these steps:

1. Create a build directory and navigate to it:
   ```bash
   mkdir build && cd build
   ```
2. Generate the makefile and build files with CMake:
   ```bash
   cmake ..
   ```
3. Compile the project:
   ```bash
   cmake --build .
   ```


## Running the Project

After building the project, you can run the executable directly from the build
directory:
   ```bash
   ./build/lexLeo
   ```

## Contributing

Contributions to the Leo project are welcome! Whether it involves 
fixing bugs, adding new features, or improving documentation, your 
help is appreciated. Please read CONTRIBUTING.md for details on our 
code of conduct and the process for submitting pull requests to us.

## License

This project is licensed under the GPLv3 - see the LICENSE.md file for details.