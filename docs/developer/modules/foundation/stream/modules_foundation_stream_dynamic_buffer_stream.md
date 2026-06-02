@page modules_foundation_stream_dynamic_buffer_stream dynamic_buffer_stream

# Purpose

The `dynamic_buffer_stream` module provides an in-memory dynamic-buffer-backed
adapter for the `stream` port.

It exposes:
- Composition Root services for configuring and registering the adapter,
- direct creation of dynamic-buffer-backed `stream_t` instances,
- factory-registration support through `stream_adapter_desc_t` descriptors.

# Public API

- @ref dynamic_buffer_stream_api "dynamic_buffer_stream API"

Sub-APIs:
- @ref dynamic_buffer_stream_cr_api "CR API"

# Architectural role

The `dynamic_buffer_stream` module belongs to the `stream` adapter family.
It implements the `stream` port contract on top of an adapter-managed dynamic
memory buffer.

Typical responsibilities:
- create dynamic-buffer-backed `stream_t` instances,
- implement borrower-side read, write, flush, and close operations,
- manage the lifetime and growth of the underlying buffer,
- expose Composition Root services for direct creation and factory
  registration.

# Main concepts

## Direct adapter creation

The Composition Root can create a dynamic-buffer-backed stream directly
through:

- `dynamic_buffer_stream_create_stream()`

## Factory registration

The Composition Root can register the adapter into a `stream` factory through:

- `dynamic_buffer_stream_create_desc()`

## Injected dependencies

Runtime dependencies are grouped in `dynamic_buffer_stream_env_t`, including:

- OSAL memory operations used by the adapter,
- OSAL memory operations used by the `stream` port.

## In-memory stream behavior

The adapter stores data in an internal dynamic buffer:

- write operations append bytes to the buffer,
- read operations consume bytes from the current read cursor,
- flush is a no-op,
- close releases backend-owned resources.

# Related modules

This adapter belongs to the `stream` family and targets the `stream` port.

# See also

- @ref specifications_dynamic_buffer_stream
  "dynamic_buffer_stream specifications"
- @ref testing_foundation_stream_family_page
  "stream family tests page"
- @ref dynamic_buffer_stream_tests_group
  "dynamic_buffer_stream tests group"
