# README

## Project overview

**lexLeo** is a Lisp-like programming language interpreter with a C-style syntax.
It is designed as an educational and experimental project focused on the
implementation of language semantics, evaluation models, and interpreter
architecture.

The project explores core concepts found in Lisp-inspired languages such as:

- symbolic expressions and values,
- first-class functions,
- dynamic evaluation (`eval`),
- quotation and delayed evaluation (`quote`),
- explicit environments and runtime state.

Unlike traditional Lisp dialects, lexLeo deliberately adopts a more C-like
surface syntax, while preserving Lisp-style semantics and execution principles.

📘 **Full documentation is available at:**  
<https://caltuli.online/lexleo>

---

## Project goals

lexLeo is primarily an educational project aimed at deepening the understanding
of programming language implementation, with a strong emphasis on **interpreters**
rather than compilation.

The project focuses on:

- lexical analysis and parsing,
- abstract syntax trees (AST),
- environment-based evaluation,
- runtime value representation,
- memory management and execution flow.

By concentrating on interpretation instead of compilation, lexLeo prioritizes
clarity, correctness, and architectural soundness over breadth of features.

---

## Software engineering focus

Beyond language theory, lexLeo is also a practical exercise in professional
software engineering. The project emphasizes:

- clean and modular C architecture,
- explicit dependency injection,
- test-driven development (TDD),
- robust build configuration using CMake,
- memory abstraction through an OSAL layer,
- automated documentation generation with Doxygen.

lexLeo is intended as a **serious portfolio project**, demonstrating not only
knowledge of language design, but also the ability to structure, document, and
maintain a non-trivial C codebase to professional standards.

---

## Scope

At its current stage, lexLeo provides:

- a **Lisp-like interpreter** with a well-defined evaluation model,
- a modular lexer and parser architecture,
- an explicit runtime environment and symbol management,
- a strong emphasis on correctness, testability, and maintainability.

A compiler backend is **intentionally out of scope** for now, in order to keep
the project focused and technically rigorous.

---

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

## Rebuild and update documentation on droplet (if SSH access is configured) :

   ```bash
   ./scripts/update_docs_on_droplet.sh 
   ```

## Design draft

- Lexer / Parser design draft:
    - [PDF](static_doc_pages/draft/lexer_parser_draft_design.pdf)

## Architecture (WIP section)

### Diagrams

- [c4_context (PNG)](static_doc_pages/architecture/diagrams/png/c4_context.png)
- [c4_container (PNG)](static_doc_pages/architecture/diagrams/png/c4_container.png)
- [dynamic_buffer_stream (PNG)](static_doc_pages/architecture/diagrams/png/diagram_dynamic_buffer_stream.png)
- [lexleo_flex (PNG)](static_doc_pages/architecture/diagrams/png/diagram_lexleo_flex.png)

### Standards

- [Module layout standards](@ref arch_standards_module_layout)
- [Type standards](@ref arch_standards_types)
- [Naming conventions](@ref arch_standards_naming)
- [Module context model](@ref module_context_model)
- [Lifecycle Visibility Rules (create / destroy)](@ref lifecycle_visibility_rules)

### draft section

- [lexleo_flex module](@ref lexleo_flex_module)

## Contribute

Contributions to the lexLeo project are welcome! Whether it involves
fixing bugs, adding new features, or improving documentation, your
help is appreciated. Please read CONTRIBUTING.md for details on our
code of conduct and the process for submitting pull requests to us.

## Licensing

lexLeo is licensed under the GNU General Public License v3.0 or later.

This project uses the following third-party components:

- Flex (BSD-like license with exception)
- Bison (GPL with special exception)
- Standard C library (glibc / musl, LGPL)

These components are compatible with the GPLv3 and impose no additional
restrictions on the distribution of lexLeo.
