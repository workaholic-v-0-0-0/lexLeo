# LexLeo

LexLeo is a modular interpreter project written in C.

It explores the design and implementation of a Lisp-inspired language
while placing strong emphasis on software architecture, testability,
and professional engineering practices.

📘 Full documentation: https://caltuli.online/lexleo

---

## Overview

LexLeo implements:

- lexical analysis and parsing,
- abstract syntax trees (AST),
- environment-based evaluation,
- runtime value representation,
- structured memory management.

The project prioritizes clarity, architectural discipline, and correctness
over feature breadth.

---

## Engineering Focus

LexLeo emphasizes:

- modular architecture with explicit boundaries,
- dependency injection (CR vs runtime separation),
- handle-based state management,
- test-driven development (TDD),
- memory abstraction through an OSAL layer,
- automated documentation with Doxygen.

---

## Getting Started

```bash
git clone https://github.com/workaholic-v-0-0-0/lexLeo.git
cd lexLeo
```

---

## Installation

LexLeo is intended to be distributed through native platform packages.

- Linux: `.deb`
- macOS: planned
- Windows: planned

📘 Installation guide:
https://caltuli.online/lexleo/user_guide.html#installation

---

## Build

### Requirements

- C compiler (GCC, Clang, MSVC)
- CMake ≥ 3.x
- Flex and Bison (for parser generation)

---

## Build the project with CMake

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

### For Development/Testing Builds and Packaging (Tests enabled)

By default, `BUILD_TESTING` is `ON`, but it can be specified explicitly for clarity.

#### For Linux:

    rm -r build
    cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
    cmake --build build
    cmake --build build --target check
    cmake --build build --target check_memory
    cmake --build build --target package

This generates the Debian package in `build/dist/`.

#### For Windows (with x64 architecture and the Visual Studio 2022 17 generator):

    rmdir /s /q build
    cmake -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=ON
    cmake --build build --config Debug
    cmake --build build --config Debug --target check
    cmake --build build --config Debug --target check_memory
    cmake --build build --config Debug --target package

This generates the Windows installer in `build\dist\`.

Note: NSIS must be installed on the Windows build machine to generate the installer.

#### For MacOS (using the default Unix Makefiles generator):

    rm -r build
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
    cmake --build build
    cmake --build build --target check
    cmake --build build --target check_memory

Packaging support for macOS is planned but not finalized yet.

## Run

After building, the CLI executable is located inside the `build/` directory.
The exact path depends on the platform and CMake generator used.

### Quick run

- Linux / macOS:

```bash
./build/lexLeo_cli
```

- Windows (Visual Studio generator):
```bash
.\build\Release\lexLeo_cli.exe
```

### Platform-specific details

#### Linux (Unix Makefiles generator)

When using the Unix Makefiles generator, the executable is created
directly inside the `build/` directory:

./build/lexLeo_cli

#### macOS (default generator)

As on Linux, the executable is generated directly inside the `build/` directory:

```bash
./build/lexLeo_cli
```

#### Windows (Visual Studio 2022 generator)

Visual Studio generators place binaries inside configuration
subdirectories (e.g., `Release/` or `Debug/`).

Release build:
```bash
.\build\Release\lexLeo_cli.exe
```

## Licensing

LexLeo is licensed under the GNU General Public License v3.0 or later.

## Third-party components:

- Flex (BSD-like license with exception)
- Bison (GPL with special exception)
- Standard C library (glibc / musl, LGPL)
