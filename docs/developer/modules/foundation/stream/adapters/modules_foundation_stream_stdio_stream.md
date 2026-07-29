@page modules_foundation_stream_stdio_stream stdio_stream

# Purpose

The `stdio_stream` module provides a standard-I/O-backed adapter for the
`stream` port.

It provides:

- Composition Root services for configuring and constructing the adapter;
- backend construction from external borrowed dependencies;
- completion of backend initialization through resolution of an internal
  borrowed standard stream dependency;
- factory-registration support through `stream_adapter_provider_t`.

# Public API

- @ref stdio_stream_api "stdio_stream API"

Sub-APIs:

- @ref stdio_stream_cr_api "Composition Root API"

# Architectural role

The `stdio_stream` module belongs to the `stream` adapter family.

It implements the `stream` port contract on top of the process standard I/O
streams, while the Composition Root supplies its external borrowed
dependencies and completes backend initialization by resolving the standard
stream wrapped by the backend.

Typical responsibilities:

- construct partially initialized `stdio_stream_t` backends from external
  borrowed dependencies;
- resolve and attach `stdin`, `stdout`, or `stderr` as an internal borrowed
  dependency from standard-stream creation arguments;
- implement read, write, flush, and close operations through the `stream`
  virtual table;
- expose an adapter provider suitable for registration in a `stream` factory.

# Main concepts

## Backend construction

The Composition Root constructs a `stdio_stream_t` backend through:

- `stdio_stream_default_env()`
- `stdio_stream_create()`

`stdio_stream_create()` initializes the backend's external borrowed
dependencies, including its OSAL standard I/O operations and memory operations.

## Backend initialization

The remaining backend state is initialized through:

- `stdio_stream_complete_default_init()`

For `stdio_stream`, this completion-time state consists of the internal
borrowed standard stream dependency.

## Stream port implementation

The adapter implements the generic `stream` contract through:

- `stdio_stream_vtbl()`
- `stream_vtbl_t`

The virtual table binds the `stdio_stream` implementations of the read, write,
flush, and close operations to the generic `stream` port.

## Factory registration

The Composition Root creates an adapter provider through:

- `stdio_stream_create_adapter_provider()`

The resulting `stream_adapter_provider_t` binds backend construction to the
`stdio_stream` virtual table and is suitable for registration through
`stream_factory_add_adapter()`.

## Dependencies and resources

The backend's external borrowed dependencies are grouped in
`stdio_stream_env_t`, including:

- OSAL standard I/O operations;
- OSAL memory operations.

The standard stream attached during completion is an internal borrowed
dependency of the `stdio_stream_t` backend. Its ownership remains with the
process standard I/O environment.

## Standard stream behavior

The adapter exposes the process standard streams through the `stream` port:

- `stdin`
    - supports read operations;
    - rejects write and flush operations.
- `stdout` and `stderr`
    - support write and flush operations;
    - reject read operations.

Additional properties:

- standard streams are borrowed and never owned by the adapter;
- buffering behavior is delegated to the injected `osal_stdio_ops_t`;
- close releases the backend container without closing the wrapped standard
  stream.

# Related modules

This adapter belongs to the `stream` family and implements the `stream` port.

# See also

- @ref specifications_stdio_stream
  "stdio_stream specifications"
- @ref testing_foundation_stream_family_page
  "stream family tests page"
- @ref stdio_stream_tests_group
  "stdio_stream tests group"
