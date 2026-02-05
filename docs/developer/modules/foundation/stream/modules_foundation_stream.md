@page modules_foundation_stream stream

# Purpose

The `stream` port defines the common byte-stream abstraction used by the
foundation layer.

It provides:
- a borrower-facing runtime API for reading, writing, and flushing streams,
- lifecycle services for destroying `stream_t` handles,
- Composition Root services for adapter registration and factory-based stream
  creation,
- adapter-facing contracts used to bind concrete backends to the generic
  `stream` port,
- owner-style creator helpers for selected higher-level creation workflows.

The module is designed to separate:
- the generic stream contract,
- the concrete adapter implementations,
- the Composition Root wiring used to connect both.

# Public API

- @ref stream_api "stream API"

Sub-APIs:
- @ref stream_borrowers_api "borrowers API"
- @ref stream_lifecycle_api "lifecycle API"
- @ref stream_cr_api "CR API"
- @ref stream_adapters_api "adapters API"
- @ref stream_owners_api "owners API"

# Architectural role

The `stream` module is a foundation port.
It defines the public byte-stream boundary used by runtime code, while concrete
adapter modules provide backend-specific implementations.

Typical responsibilities:
- expose the public `stream_t` handle and status model,
- route borrower-facing operations through adapter-bound dispatch tables,
- manage stream handle destruction,
- support factory-based creation through public adapter descriptors and keys.

# Main concepts

## Borrower-facing usage

Runtime code manipulates `stream_t` handles through the public borrower API:
- `stream_read()`
- `stream_write()`
- `stream_flush()`

## Adapter binding

Concrete adapters bind backend logic to the generic stream port through:
- `stream_vtbl_t`
- `stream_create()`
- `stream_adapter_desc_t`

## Factory-based creation

The Composition Root can register adapters and create streams by key through:
- `stream_factory_t`
- `stream_factory_add_adapter()`
- `stream_factory_create_stream()`

# Related modules

Concrete adapters for this port are documented in the surrounding
`stream` family.

See:
- @ref stream_family_group "stream family"

# Specifications and tests

- @ref specifications_stream "stream specifications"
- @ref stream_tests_group "stream tests"
