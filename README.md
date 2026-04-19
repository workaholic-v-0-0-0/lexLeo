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
- dependency injection,
- composition-root separation,
- handle-based state management,
- test-driven development on the primary development platform,
- memory abstraction through an OSAL layer,
- automated documentation with Doxygen on supported platforms.

---

## Getting Started

```bash
git clone https://github.com/workaholic-v-0-0-0/lexLeo.git
cd lexLeo
```

---

## Installation

LexLeo is intended to be distributed through native platform packages.

Current packaging targets:

- Linux: Debian package (`.deb`)
- Windows: NSIS installer
- macOS: productbuild installer (`.pkg`)

📘 Installation guide:  
https://caltuli.online/lexleo/user_guide.html#installation

---

## Build

### Requirements

- C compiler:
    - GCC or Clang on Linux/macOS
    - MSVC on Windows
- CMake ≥ 3.x
- Flex and Bison for parser generation
- NSIS on Windows, only if generating the Windows installer

---

## Production Builds

Production builds disable tests.

### Linux

```bash
cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build
```

### Windows

With the Visual Studio 2022 generator:

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=OFF
cmake --build build --config Release
```

### macOS

Using the default Unix Makefiles generator:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build
```

---

## Development Builds

The main development workflow is currently maintained on Linux.

### Linux development build

```bash
rm -rf build
cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build
cmake --build build --target check
cmake --build build --target check_memory
```

### Documentation generation

Doxygen documentation generation is currently maintained on Linux.

```bash
cmake --build build --target docs
```

Windows documentation generation is not currently maintained.

macOS documentation generation may work depending on the local toolchain, but
is not currently treated as a guaranteed supported workflow.

---

## Packaging

### Linux package

```bash
rm -rf build
cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build
cmake --build build --target package
```

This generates the Debian package in:

```txt
build/dist/
```

### Windows installer

```bat
rmdir /s /q build
cmake -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_TESTING=OFF
cmake --build build --config Release
cmake --build build --config Release --target package
```

This generates the Windows installer in:

```txt
build\dist\
```

NSIS must be installed on the Windows build machine to generate the installer.

Tests and Doxygen documentation generation are not currently maintained as part
of the Windows workflow.

### macOS installer

```bash
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build
cmake --build build --target package
```

This generates the macOS package in:

```txt
build/dist/
```

The macOS package installs LexLeo under:

```txt
/usr/local
```

Tests and Doxygen documentation generation on macOS may require additional
toolchain setup and are not currently treated as guaranteed workflows.

---

## Run

After building, the CLI executable is named:

```txt
lexleo
```

The exact path depends on the platform and CMake generator used.

### Linux / macOS

With Unix Makefiles:

```bash
./build/bin/lexleo
```

### Windows

With the Visual Studio generator:

```bat
.\build\bin\Release\lexleo.exe
```

After installation through a native package, LexLeo can usually be launched with:

```bash
lexleo
```

---

## Licensing

LexLeo is licensed under the GNU General Public License v3.0 or later.

## Third-party components

- Flex: BSD-like license with exception
- Bison: GPL with special exception
- Standard C library: platform-dependent license
