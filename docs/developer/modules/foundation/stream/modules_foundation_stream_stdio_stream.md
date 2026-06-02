@page modules_foundation_stream_stdio_stream stdio_stream

# Purpose

The `stdio_stream` module provides a standard-I/O-backed adapter for the
`stream` port.

It exposes:
- Composition Root services for configuring and registering the adapter,
- direct creation of standard-I/O-backed `stream_t` instances,
- factory-registration support through `stream_adapter_desc_t` descriptors.

# Public API

- @ref stdio_stream_api "stdio_stream API"

Sub-APIs:
- @ref stdio_stream_cr_api "CR API"

# Architectural role

The `stdio_stream` module belongs to the `stream` adapter family.
It implements the `stream` port contract on top of the process standard I/O
streams.

Typical responsibilities:
- create `stream_t` instances bound to `stdin`, `stdout`, or `stderr`,
- implement borrower-side read, write, flush, and close operations,
- expose Composition Root services for direct creation and factory
  registration,
- bridge the generic `stream` abstraction to the process standard streams.

# Main concepts

## Direct stream creation

The Composition Root can create a standard-I/O-backed stream directly through:

- `stdio_stream_create_stream()`

## Factory registration

The Composition Root can register the adapter into a `stream` factory through:

- `stdio_stream_create_desc()`

## Injected dependencies

Runtime dependencies are grouped in `stdio_stream_env_t`, including:

- OSAL stdio operations,
- OSAL memory operations used by the adapter,
- OSAL memory operations used by the `stream` port.

## Standard stream behavior

The adapter exposes the process standard streams through the `stream` port:

- `stdin`
    - supports read operations,
    - rejects write and flush operations.

- `stdout` and `stderr`
    - support write and flush operations,
    - reject read operations.

Additional properties:

- standard streams are borrowed and never owned by the adapter,
- buffering behavior is delegated to the injected `osal_stdio_ops_t`,
- close releases only adapter-owned backend resources.

# Related modules

This adapter belongs to the `stream` family and targets the `stream` port.

# See also

- @ref specifications_stdio_stream
  "stdio_stream specifications"
- @ref testing_foundation_stream_family_page
  "stream family tests page"
- @ref stdio_stream_tests_group
  "stdio_stream tests group"
