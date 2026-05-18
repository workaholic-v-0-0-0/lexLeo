@page modules_foundation_stream_fs_stream fs_stream

# Purpose

The `fs_stream` module provides a file-backed adapter for the `stream` port in
the foundation layer.

It provides:
- Composition Root helpers for building default adapter configuration and
  environment objects,
- direct creation of file-backed `stream_t` handles from `fs_stream`-specific
  creation arguments,
- factory-registration support through adapter descriptors compatible with the
  `stream` factory.

The module is designed to connect:
- the generic `stream` port,
- the OSAL file abstraction,
- the dependency injection wiring used to provide file and memory operations.

# Public API

- @ref fs_stream_api "fs_stream API"

Sub-APIs:
- @ref fs_stream_cr_api "CR API"

# Architectural role

The `fs_stream` module is a foundation adapter module for the `stream` port.
It implements the `stream` backend contract on top of OSAL file operations and
exposes Composition Root entry points for direct creation and factory
registration.

Typical responsibilities:
- open OSAL files from `fs_stream_args_t` creation arguments,
- create file-backed `stream_t` handles through the `stream` port,
- package adapter configuration and injected dependencies for Composition Root
  use,
- produce `stream_adapter_desc_t` descriptors for registration into the
  `stream` factory.

# Main concepts

## Direct stream creation

The Composition Root can create a file-backed stream directly through:
- `fs_stream_create_stream()`

## Factory registration

The Composition Root can register the adapter into a `stream` factory through:
- `fs_stream_create_desc()`

## Injected dependencies

The adapter relies on injected dependencies grouped in `fs_stream_env_t`,
typically including:
- OSAL file operations,
- adapter memory operations,
- the underlying `stream` port environment.

# Related modules

This adapter is part of the surrounding `stream` family and targets the
`stream` port.

# See also

- @ref specifications_fs_stream "fs_stream specifications"
- @ref testing_foundation_stream_family_page "stream family tests page"
- @ref fs_stream_tests_group "fs_stream tests group"
