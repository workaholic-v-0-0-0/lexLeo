@page modules_foundation_stream_fs_stream fs_stream

# Purpose

The `fs_stream` module provides a file-backed adapter for the `stream` port.

It provides:

- Composition Root services for configuring and constructing the adapter;
- backend construction from external borrowed dependencies;
- completion of backend initialization through acquisition of an owned OSAL
  file resource;
- factory-registration support through `stream_adapter_provider_t`.

# Public API

- @ref fs_stream_api "fs_stream API"

Sub-APIs:

- @ref fs_stream_cr_api "Composition Root API"

# Architectural role

The `fs_stream` module belongs to the `stream` adapter family.

It implements the `stream` port contract on top of OSAL file operations, while
the Composition Root supplies its external borrowed dependencies and completes
backend initialization by acquiring the file resource owned by the backend.

Typical responsibilities:

- construct partially initialized `fs_stream_t` backends from external borrowed
  dependencies;
- acquire and attach the owned OSAL file resource from regular-file creation
  arguments;
- implement read, write, flush, and close operations through the `stream`
  virtual table;
- expose an adapter provider suitable for registration in a `stream` factory.

# Main concepts

## Backend construction

The Composition Root constructs an `fs_stream_t` backend through:

- `fs_stream_default_env()`
- `fs_stream_create()`

`fs_stream_create()` initializes the backend's external borrowed dependencies,
including its OSAL file operations and memory operations.

## Backend initialization

The remaining backend state is initialized through:

- `fs_stream_complete_default_init()`

For `fs_stream`, this completion-time state consists of the owned OSAL file
resource.

## Stream port implementation

The adapter implements the generic `stream` contract through:

- `fs_stream_vtbl()`
- `stream_vtbl_t`

The virtual table binds the `fs_stream` implementations of the read, write,
flush, and close operations to the generic `stream` port.

## Factory registration

The Composition Root creates an adapter provider through:

- `fs_stream_create_adapter_provider()`

The resulting `stream_adapter_provider_t` binds backend construction to the
`fs_stream` virtual table and is suitable for registration through
`stream_factory_add_adapter()`.

## Dependencies and resources

The backend's external borrowed dependencies are grouped in `fs_stream_env_t`,
including:

- OSAL file operations;
- OSAL memory operations.

The OSAL file handle attached during completion is an owned resource of the
`fs_stream_t` backend.

# Related modules

This adapter belongs to the `stream` family and implements the `stream` port.

# See also

- @ref specifications_fs_stream
  "fs_stream specifications"
- @ref testing_foundation_stream_family_page
  "stream family tests page"
- @ref fs_stream_tests_group
  "fs_stream tests group"
