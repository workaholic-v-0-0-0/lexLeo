@page modules_foundation_stream_fs_stream fs_stream

# Purpose

The `fs_stream` module provides a file-backed adapter for the `stream` port.

It exposes:
- Composition Root services for configuring and registering the adapter,
- direct creation of file-backed `stream_t` instances,
- factory-registration support through `stream_adapter_desc_t` descriptors.

# Public API

- @ref fs_stream_api "fs_stream API"

Sub-APIs:
- @ref fs_stream_cr_api "CR API"

# Architectural role

The `fs_stream` module belongs to the `stream` adapter family.
It implements the `stream` port contract on top of OSAL file operations.

Typical responsibilities:
- open OSAL files from `stream_file_creator_args_t` creation arguments,
- create file-backed `stream_t` instances,
- implement borrower-side read, write, flush, and close operations,
- expose Composition Root services for direct creation and factory
  registration.

# Main concepts

## Direct stream creation

The Composition Root can create a file-backed stream directly through:

- `fs_stream_create_stream()`

## Factory registration

The Composition Root can register the adapter into a `stream` factory through:

- `fs_stream_create_desc()`

## Injected dependencies

Runtime dependencies are grouped in `fs_stream_env_t`, including:

- OSAL file operations,
- OSAL memory operations used by the adapter,
- OSAL memory operations used by the `stream` port.

# Related modules

This adapter belongs to the `stream` family and targets the `stream` port.

# See also

- @ref specifications_fs_stream
  "fs_stream specifications"
- @ref testing_foundation_stream_family_page
  "stream family tests page"
- @ref fs_stream_tests_group
  "fs_stream tests group"