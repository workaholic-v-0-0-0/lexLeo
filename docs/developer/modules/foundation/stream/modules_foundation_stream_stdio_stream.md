@page modules_foundation_stream_stdio_stream stdio_stream

# Purpose

The `stdio_stream` module provides a standard-I/O-backed adapter for the
`stream` port in the foundation layer.

It provides:
- Composition Root helpers for building default adapter configuration and
  environment objects,
- direct creation of `stream_t` handles bound to process standard streams,
- factory-registration support through adapter descriptors compatible with the
  `stream` factory.

The module is designed to connect:
- the generic `stream` port,
- the process standard I/O streams (`stdin`, `stdout`, `stderr`),
- the Composition Root wiring used to inject stdio and memory dependencies.

# Public API

- @ref stdio_stream_api "stdio_stream API"

Sub-APIs:
- @ref stdio_stream_cr_api "CR API"

# Architectural role

The `stdio_stream` module is a foundation adapter module for the
`stream` port. It implements the `stream` backend contract on top of the
process standard I/O streams and exposes Composition Root entry points for
direct creation and factory registration.

Typical responsibilities:
- create `stream_t` handles bound to `stdin`, `stdout`, or `stderr`,
- implement stream read/write/flush/close behavior on top of standard I/O,
- package adapter configuration and injected dependencies for Composition Root
  use,
- produce `stream_adapter_desc_t` descriptors for registration into the
  `stream` factory.

# Main concepts

## Direct adapter creation

The Composition Root can create a standard-I/O-backed stream directly
through:
- `stdio_stream_create_stream()`

## Factory registration

The Composition Root can register the adapter into a `stream` factory through:
- `stdio_stream_create_desc()`

## Injected dependencies

The adapter relies on injected dependencies grouped in
`stdio_stream_env_t`, typically including:
- the stdio operations used to access standard streams,
- the memory operations used by the adapter backend,
- the underlying `stream` port environment.

## Standard stream behavior

The adapter provides `stream` behavior backed by process standard streams:

- `stdin`:
    - supports read operations,
    - rejects write and flush operations.

- `stdout` / `stderr`:
    - support write and flush operations,
    - reject read operations.

Additional properties:
- standard streams are **borrowed** and never owned by the adapter,
- no buffering is implemented by the adapter itself; behavior is delegated to
  injected `osal_stdio_ops_t`,
- close releases only the backend container, not the underlying standard stream.

# Related modules

This adapter is part of the surrounding `stream` family and targets the
`stream` port.

# See also

- @ref specifications_stdio_stream "stdio_stream specifications"
- @ref testing_foundation_stream_family_page "stream family tests page"
- @ref stdio_stream_tests_group "stdio_stream tests group"