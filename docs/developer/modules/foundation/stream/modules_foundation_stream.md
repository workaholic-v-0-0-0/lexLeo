@page modules_foundation_stream stream

# Purpose

The `stream` port defines the common byte-stream abstraction used throughout the
foundation layer.

It provides:
- a borrower-facing runtime API for reading, writing, and flushing streams;
- an owner-facing API for destroying streams and creating them through
  specialized creators;
- an adapter-facing API for binding concrete backends to the generic
  `stream` port;
- Composition Root services for registering adapters and wiring stream
  creation.

# Public API

- @ref stream_api "stream API"

Sub-APIs:
- @ref stream_borrowers_api "borrowers API"
- @ref stream_owners_api "owners API"
- @ref stream_adapters_api "adapters API"
- @ref stream_cr_api "Composition Root API"

# Architectural role

The `stream` module is a foundation port.

It defines the public byte-stream boundary used by runtime code, while
concrete adapter modules provide backend-specific implementations and the
Composition Root assembles both into complete creation workflows.

Typical responsibilities:
- expose the public `stream_t` abstraction and status model;
- dispatch borrower-facing operations through adapter-bound virtual tables;
- manage stream lifetime;
- expose factory-based wiring facilities for the Composition Root.

# Main concepts

## Borrower-facing usage

Runtime code manipulates `stream_t` handles through:
- `stream_read()`
- `stream_write()`
- `stream_flush()`

## Owner-facing usage

Owners manage stream lifetime and create streams through specialized creators:
- `stream_destroy()`
- `stream_buffer_creator_create()`
- `stream_file_creator_create()`
- `stream_io_creator_create()`

## Adapter binding

Concrete adapters bind backend implementations to the generic stream contract
through:
- `stream_vtbl_t`
- `stream_create()`
- `stream_adapter_desc_t`

## Composition Root wiring

The Composition Root registers adapters and wires stream creation through:
- `stream_factory_t`
- `stream_factory_add_adapter()`
- `stream_factory_create_stream()`

# Related modules

Concrete adapters implementing this port are documented in the surrounding
`stream` family.

# See also

- @ref specifications_stream "stream specifications"
- @ref testing_foundation_stream_family_page "stream family tests page"
- @ref stream_tests_group "stream tests group"
