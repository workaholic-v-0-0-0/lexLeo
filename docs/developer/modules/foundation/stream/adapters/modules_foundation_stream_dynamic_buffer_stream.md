@page modules_foundation_stream_dynamic_buffer_stream dynamic_buffer_stream

# Purpose

The `dynamic_buffer_stream` module provides an in-memory dynamic-buffer-backed
adapter for the `stream` port.

It exposes:
- Composition Root services for configuring and constructing the adapter,
- backend creation and completion services,
- factory-registration support through `stream_adapter_provider_t` providers.

# Public API

- @ref dynamic_buffer_stream_api "dynamic_buffer_stream API"

Sub-APIs:
- @ref dynamic_buffer_stream_cr_api "CR API"

# Architectural role

The `dynamic_buffer_stream` module belongs to the `stream` adapter family.
It implements the `stream` port contract on top of an adapter-managed dynamic
memory buffer.

Typical responsibilities:
- create and initialize `dynamic_buffer_stream_t` backends,
- implement borrower-side read, write, flush, and close operations,
- manage the lifetime and growth of the owned dynamic buffer,
- expose Composition Root services for backend construction and factory
  registration.

# Main concepts

## Backend construction

The Composition Root constructs a `dynamic_buffer_stream_t` backend through:

- `dynamic_buffer_stream_create()`
- `dynamic_buffer_stream_complete_default_init()`

`dynamic_buffer_stream_create()` initializes the backend's external borrowed
dependencies.

`dynamic_buffer_stream_complete_default_init()` completes initialization by
creating the owned dynamic buffer when it has not already been initialized.

## Factory registration

The Composition Root can provide the adapter to a `stream` factory through:

- `dynamic_buffer_stream_create_adapter_provider()`

The resulting `stream_adapter_provider_t` contains the backend constructor,
the adapter virtual table, and the constructor state required to create fully
initialized `dynamic_buffer_stream_t` backends.

## External borrowed dependencies

External borrowed dependencies are grouped in `dynamic_buffer_stream_env_t`:

- OSAL memory operations used by the adapter.

The dynamic buffer itself is an owned resource and is initialized during
backend completion.

## In-memory stream behavior

The adapter stores data in an owned dynamic buffer:

- write operations append bytes to the buffer,
- read operations consume bytes from the current read cursor,
- flush is a no-op,
- close releases the owned dynamic buffer and backend resources.

# Related modules

This adapter belongs to the `stream` family and targets the `stream` port.

# See also

- @ref specifications_dynamic_buffer_stream
  "dynamic_buffer_stream specifications"
- @ref testing_foundation_stream_family_page
  "stream family tests page"
- @ref dynamic_buffer_stream_tests_group
  "dynamic_buffer_stream tests group"
