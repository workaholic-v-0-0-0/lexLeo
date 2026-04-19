@page modules_cli CLI

# Overview

The CLI module provides the command-line frontend for LexLeo.

It is responsible for:
- exposing the `lexleo` executable entry point,
- delegating execution to the CLI runtime entry function,
- resolving the effective log-file path,
- using configuration-defined values when available,
- falling back to platform defaults when needed,
- preparing the parent directory of the target log file,
- and performing the current minimal bootstrap behavior of packaged builds.

# Current scope

The current implementation is intentionally minimal.

At this stage, the CLI:
- starts the command-line process,
- resolves the log path,
- opens or creates the log file,
- writes a probe entry,
- flushes and closes the file,
- reports the resolved path on standard output.

This behavior may evolve as the frontend grows.

# Components

| Component | Description | Reference |
|-----------|-------------|-----------|
| Public API | Public CLI entry point exposed through `cli.h`. | @ref cli_group |
| Internals | Shared and platform-specific log-path resolution helpers. | @ref cli_internal_group |

# Notes

The CLI module is a frontend layer.  
It orchestrates user-facing startup behavior and relies on lower-level
foundation services such as OSAL file, environment, memory, and standard I/O
abstractions.

# See also

- @ref specifications_cli "CLI specifications"
- @ref cli_group "cli group"
- @ref cli_internal_group "cli internal group"
