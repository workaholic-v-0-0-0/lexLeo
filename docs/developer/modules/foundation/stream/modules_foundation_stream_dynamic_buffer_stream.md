@page modules_foundation_stream_dynamic_buffer_stream dynamic_buffer_stream

# Purpose

The `dynamic_buffer_stream` module provides an in-memory dynamic-buffer-backed
adapter for the `stream` port in the foundation layer.

It provides:
- Composition Root helpers for building default adapter configuration and
  environment objects,
- direct creation of dynamic-buffer-backed `stream_t` handles,
- factory-registration support through adapter descriptors compatible with the
  `stream` factory.

The module is designed to connect:
- the generic `stream` port,
- an in-memory dynamic buffer backend,
- the Composition Root wiring used to inject memory dependencies.

# Public API

- @ref dynamic_buffer_stream_api "dynamic_buffer_stream API"

Sub-APIs:
- @ref dynamic_buffer_stream_cr_api "CR API"

# Architectural role

The `dynamic_buffer_stream` module is a foundation adapter module for the
`stream` port. It implements the `stream` backend contract on top of an
in-memory dynamic buffer and exposes Composition Root entry points for direct
creation and factory registration.

Typical responsibilities:
- create dynamic-buffer-backed `stream_t` handles through the `stream` port,
- implement stream read/write/flush/close behavior on top of an in-memory
  dynamic buffer,
- package adapter configuration and injected dependencies for Composition Root
  use,
- produce `stream_adapter_desc_t` descriptors for registration into the
  `stream` factory.

# Main concepts

## Direct adapter creation

The Composition Root can create a dynamic-buffer-backed stream directly
through:
- `dynamic_buffer_stream_create_stream()`

## Factory registration

The Composition Root can register the adapter into a `stream` factory through:
- `dynamic_buffer_stream_create_desc()`

## Injected dependencies

The adapter relies on injected dependencies grouped in
`dynamic_buffer_stream_env_t`, typically including:
- the memory operations used by the adapter backend,
- the underlying `stream` port environment.

## In-memory stream behavior

The adapter provides `stream` behavior backed by an internal dynamic buffer:
- writes append bytes to the buffer,
- reads consume bytes from the current read cursor,
- flush is a no-op,
- close releases backend-owned buffer resources.

# Related modules

This adapter is part of the surrounding `stream` family and targets the
`stream` port.

# See also

- @ref specifications_dynamic_buffer_stream "dynamic_buffer_stream specifications"
- @ref testing_foundation_stream_family_page "stream family tests page"
- @ref dynamic_buffer_stream_tests_group "dynamic_buffer_stream tests group"
